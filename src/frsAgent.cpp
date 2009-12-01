/*    
 Copyright 2008 Batuhan Bozkurt
 batuhan@batuhanbozkurt.com
 
 This file is a part of deQuencher v0.2.1
 
 "deQuencher" is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 "deQuencher" is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */   

//synth agent, type 1

#include "agents.h"
#include "deQuencher.h"

frsAgent::frsAgent(int argColor, string argName, scServer* argServer, ofTrueTypeFont* argFont, Console* argConsole, vector<Multislider*>* argSliders, vector<Agent*>* argAgents, string runPath)
{
	advanceIndex = 0;
	isSelected = false;
	sizeMultiplier = 1;
	agentType = 5;
	agArray = argAgents; //needed here to get a unique Id.
	dlFailed = false; //if downloading a file fails, this will be set to true.
	
	offsetX = 0;
	offsetY = 0;
	
	isOnFxBus = false;
	isOnChainFxBus = false;
	boundFxId = 0;
	keyword = argName;
	
	lastBoundCAgent = NULL;
	
	usedFont = argFont;
	usedConsole = argConsole;
	usedSliders = argSliders;
	mColor = argColor;
	
	boundServer = argServer;
	
	uniqueID = this->getUniqueId();
	
	synthCreated = false;
	
	stringstream idString;
	idString << uniqueID;
	string scriptCommand = "python \"" + runPath + "/" + "fsounder.py\" " + keyword + " " + idString.str();
	system((scriptCommand + "&").c_str());
	
	
}

frsAgent::~frsAgent()
{
	if(synthCreated)
	{
		delete myBuffer;
		delete boundSynth;
	}
}

void frsAgent::createSynth(string fileName, int numChannels)
{
	myBuffer = new scBuffer(boundServer);
	uniqueBufferNo = myBuffer->newBuffer();
	myBuffer->loadFile(fileName, uniqueBufferNo);
	
	switch(numChannels)
	{
		case 1:
			boundSynth = new scSynth("fsplayer_mono", boundServer);
			break;
		case 2:
			boundSynth = new scSynth("fsplayer_stereo", boundServer);
			break;
	}
	
	boundSynth->create();
	boundSynth->set("bufferNo", uniqueBufferNo);
	
	synthCreated = true;
}

void frsAgent::advance()
{
	int clippedHistory; //if we are clipping frames, then this will contain framesize.
	
	playLenRatio = usedSliders->at(2)->getSlideVal(agIndex);
	clippedHistory = max(((int)historyX.size() * playLenRatio), 1.0F);
	
	addNoiseX = round(ofRandom(-5, 5) * usedSliders->at(3)->getSlideVal(agIndex));
	addNoiseY = round(ofRandom(-5, 5) * usedSliders->at(3)->getSlideVal(agIndex));
	
	this->rangeMap(usedSliders->at(0)->getSlideVal(agIndex));
	
	currentX = fmod(((this->map(historyX[advanceIndex], minValueX, maxValueX, minValueXnew, maxValueXnew) + addNoiseX + offsetX)  + MWIDTH), MWIDTH);
	currentY = fmod(((this->map(historyY[advanceIndex], minValueY, maxValueY, minValueYnew, maxValueYnew) + addNoiseY + offsetY) + AHEIGHT), AHEIGHT);
	oldX = fmod(((this->map(historyX[max(0, advanceIndex - 1)], minValueX, maxValueX, minValueXnew, maxValueXnew) + addNoiseX + offsetX)  + MWIDTH), MWIDTH);
	oldY = fmod(((this->map(historyY[max(0, advanceIndex - 1)], minValueY, maxValueY, minValueYnew, maxValueYnew)  + addNoiseY + offsetY) + AHEIGHT), AHEIGHT);
	
	if(usedConsole->showPoints && (abs(oldX-currentX) < 150) && (abs(oldY-currentY) < 150))
	{	
		if(synthCreated)
		{
			ofFill(); 
		} 
		else
		{
			ofNoFill();
		}
		
		ofSetColor(mColor);
		if(dlFailed)
		{
			ofLine(currentX - 5, currentY - 5, currentX + 5, currentY + 5);
		}
		ofRect(currentX-(2 * sizeMultiplier), currentY-(2 * sizeMultiplier), 4 * sizeMultiplier, 4 * sizeMultiplier);
		ofSetColor(255, 255, 255);
		ofFill();
		usedFont->drawString(keyword, currentX + 5, currentY - 5);
	}
	advanceIndex++;
	advanceIndex = advanceIndex % clippedHistory;
	
}

void frsAgent::cleanUp()
{
	if(isOnChainFxBus)
	{
		lastBoundCAgent->isFed = false;
		lastBoundCAgent->countConnected -= 1;
	}
	
	if(synthCreated)
	{
		boundSynth->set("fxBusNo", (MAXBUSSES * 2) + 14); //send to blackhole
		boundSynth->set("t_trig100", 1);
		myBuffer->free(uniqueBufferNo);
	}
}


