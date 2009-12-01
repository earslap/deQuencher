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

#include "agents.h"
#include "deQuencher.h"

fAgent::fAgent(int argColor, string argName, scServer* argServer, int argDistance, ofTrueTypeFont* argFont, Console* argConsole, vector<Multislider*>* argSliders, vector<Agent*>* argAgents)
{
	advanceIndex = 0;
	isSelected = false;
	sizeMultiplier = 1;
	agentType = 3;
	agArray = argAgents; //needed here to get a unique Id.
	
	offsetX = 0;
	offsetY = 0;
	
	synthName = argName;
	maxDistance = argDistance;
	
	usedFont = argFont;
	usedConsole = argConsole;
	usedSliders = argSliders;
	mColor = argColor;
	
	myBus = new scBus(argServer);
	uniqueInputBusL = myBus->newBus();
	
	uniqueID = this->getUniqueId();
	boundSynth = new scSynth(synthName, argServer);
	boundSynth->create();
	boundSynth->set("inFxBusNo", uniqueInputBusL); //this should be set while creating the synth! change.
	
	
	
	
}

fAgent::~fAgent()
{
	delete myBus;
	delete boundSynth;
}

void fAgent::advance()
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
	oldY = fmod(((this->map(historyY[max(0, advanceIndex - 1)], minValueY, maxValueY, minValueYnew, maxValueYnew) + addNoiseY + offsetY) + AHEIGHT), AHEIGHT);
	
	if(usedConsole->showPoints && (abs(oldX-currentX) < 150) && (abs(oldY-currentY) < 150))
	{	ofSetColor(mColor);
		ofLine(oldX, oldY, currentX, currentY);
		ofEllipse(currentX, currentY, 4 * sizeMultiplier, 4 * sizeMultiplier);
		ofSetColor(255, 255, 255);
		usedFont->drawString(synthName, currentX + 5, currentY - 5);
	}
	
	this->collideTest();
	advanceIndex++;
	advanceIndex = advanceIndex % clippedHistory;
	
}



void fAgent::collideTest() //the same collision detection thing for Fx synths.  sends the affecting synth a parameter between 0.1
//used for wet-dry ratio.
{
	float howclose;
	float controlVal;
    
    for(int i = 0; i < agArray->size(); i++)
    {
		if(agArray->at(i)->uniqueID != uniqueID) //if that is not me
		{ 
			
			howclose = round(usedSliders->at(1)->getSlideVal(agIndex) * maxDistance);
			float distance = this->dist(currentX, currentY, agArray->at(i)->currentX, agArray->at(i)->currentY);
			
			if((agArray->at(i)->agentType == 1) && (distance < howclose))
			{
				if(((sAgent*)agArray->at(i))->boundFxId == 0)
					((sAgent*)agArray->at(i))->boundFxId = uniqueID;
			}
			if((agArray->at(i)->agentType == 1) && (((sAgent*)agArray->at(i))->boundFxId == uniqueID)) //if the comparing agent is a synth and not bound to another fx
			{ 
				
				if( distance < howclose)
				{ 
					if(this->contains(agArray->at(i)->uniqueID, false) == false) 
					{
						((sAgent*)agArray->at(i))->boundSynth->moveBefore(boundSynth->nodeID);
						((sAgent*)agArray->at(i))->boundSynth->set("fxBusNo", uniqueInputBusL);
						((sAgent*)agArray->at(i))->isOnFxBus = true;
						zeroSent.push_back(agArray->at(i)->uniqueID);
					}
					
					controlVal = 1 - (distance / howclose);
					//printf("fxmul: %f", controlVal);
					((sAgent*)agArray->at(i))->boundSynth->set("fxMul", controlVal);
					
					ofSetColor(mColor);
					ofLine(currentX, currentY, agArray->at(i)->currentX, agArray->at(i)->currentY);
				} 
				else 
				{ 
					if(this->contains(agArray->at(i)->uniqueID, true) == true) 
					{ 
						((sAgent*)agArray->at(i))->boundFxId = 0;
						((sAgent*)agArray->at(i))->boundSynth->set("fxMul", 0);
						((sAgent*)agArray->at(i))->boundSynth->set("fxBusNo", (MAXBUSSES * 2) + 14); //send output to the blackhole bus.
						((sAgent*)agArray->at(i))->isOnFxBus = false;
						
					}  
				}
			}
			
			if((agArray->at(i)->agentType == 5) && (distance < howclose))
			{
				if(((frsAgent*)agArray->at(i))->synthCreated == true)
				{
					if(((frsAgent*)agArray->at(i))->boundFxId == 0)
						((frsAgent*)agArray->at(i))->boundFxId = uniqueID;
				}
			}
			if((agArray->at(i)->agentType == 5) && (((frsAgent*)agArray->at(i))->boundFxId == uniqueID)) //if the comparing agent is a synth and not bound to another fx
			{ 
				
				if( distance < howclose)
				{ 
					if(this->contains(agArray->at(i)->uniqueID, false) == false) 
					{
						((frsAgent*)agArray->at(i))->boundSynth->moveBefore(boundSynth->nodeID);
						((frsAgent*)agArray->at(i))->boundSynth->set("fxBusNo", uniqueInputBusL);
						((frsAgent*)agArray->at(i))->isOnFxBus = true;
						zeroSent.push_back(agArray->at(i)->uniqueID);
					}
					
					controlVal = 1 - (distance / howclose);
					//printf("fxmul: %f", controlVal);
					((frsAgent*)agArray->at(i))->boundSynth->set("fxMul", controlVal);
					
					ofSetColor(mColor);
					ofLine(currentX, currentY, agArray->at(i)->currentX, agArray->at(i)->currentY);
				} 
				else 
				{ 
					if(this->contains(agArray->at(i)->uniqueID, true) == true) 
					{ 
						((frsAgent*)agArray->at(i))->boundFxId = 0;
						((frsAgent*)agArray->at(i))->boundSynth->set("fxMul", 0);
						((frsAgent*)agArray->at(i))->boundSynth->set("fxBusNo", (MAXBUSSES * 2) + 14); //send output to the blackhole bus.
						((frsAgent*)agArray->at(i))->isOnFxBus = false;
						
					}  
				}
			}
		}
    }
}


void fAgent::cleanUp()
{
	
	for(int i = 0; i < agArray->size(); i++)
	{
		if(agArray->at(i)->agentType == 1)
		{
			if(((sAgent*)agArray->at(i))->boundFxId == uniqueID)
			{
				((sAgent*)agArray->at(i))->boundFxId = 0;
				((sAgent*)agArray->at(i))->boundSynth->set("fxMul", 0);
				((sAgent*)agArray->at(i))->boundSynth->set("fxBusNo", (MAXBUSSES * 2) + 14); //send output to the blackhole bus.
				((sAgent*)agArray->at(i))->isOnFxBus = false;
			}
		}
		
		if(agArray->at(i)->agentType == 5)
		{
			if(((frsAgent*)agArray->at(i))->boundFxId == uniqueID) 
			{
				((frsAgent*)agArray->at(i))->boundFxId = 0;
				((frsAgent*)agArray->at(i))->boundSynth->set("fxMul", 0);
				((frsAgent*)agArray->at(i))->boundSynth->set("fxBusNo", (MAXBUSSES * 2) + 14); //send output to the blackhole bus.
				((frsAgent*)agArray->at(i))->isOnFxBus = false;
			}
		}
	}
	
	boundSynth->set("t_trig100", 1);
	myBus->free(uniqueInputBusL);
}