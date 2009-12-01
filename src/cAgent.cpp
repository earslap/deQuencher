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

cAgent::cAgent(int argColor, string argName, scServer* argServer, int argDistance, ofTrueTypeFont* argFont, Console* argConsole, vector<Multislider*>* argSliders, vector<Agent*>* argAgents)
{
	advanceIndex = 0;
	isSelected = false;
	sizeMultiplier = 1;
	agentType = 4;
	agArray = argAgents;
	
	offsetX = 0;
	offsetY = 0;
	
	countConnected = 0;
	
	isOnFxBus = false;
	isInChain = false;
	isFed = false;
	boundFxId = 0;
	
	oldControlFirst = -1;
	oldControlSecond = -1;
	oldControlThird = -1;
	
	
	synthName = argName;
	maxDistance = argDistance;
	
	usedFont = argFont;
	usedConsole = argConsole;
	usedSliders = argSliders;
	mColor = argColor;
	
	lastBoundCAgent = NULL;
	
	usedServer = argServer;
	
	inBus = new scBus(argServer);
	outBus = new scBus(argServer);
	uniqueInputBusL = inBus->newBus();
	uniqueOutputBusL = outBus->newBus();
	
	uniqueID = this->getUniqueId();
	boundSynth = new scSynth(synthName, argServer);
	boundSynth->create();
	boundSynth->set("inFxBusNo", uniqueInputBusL); //this should be set while creating the synth! change.
	boundSynth->set("outFxBusNo", uniqueOutputBusL); //this should be set while creating the synth! change
	
}

cAgent::~cAgent()
{
	delete inBus;
	delete outBus;
	delete boundSynth;
}

void cAgent::advance()
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
		//ofTriangle(currentX, currentY - 4, currentX - 4, currentY + 4, currentX + 4, currentY + 4); //behaves funky?
		ofLine(currentX - (8 * sizeMultiplier), currentY, currentX + (8 * sizeMultiplier), currentY);
		ofLine(currentX, currentY, currentX+5, currentY);
		ofSetColor(255, 255, 255);
		usedFont->drawString(synthName, currentX + 5, currentY - 5);
	}
	
	this->collideTest();
	advanceIndex++;
	advanceIndex = advanceIndex % clippedHistory;
	
}

void cAgent::collideTest()
{
	float howclose;
	float controlValFirst;
	float controlValSecond;
	float controlValThird;
    
    for(int i = 0; i < agArray->size(); i++)
    {
		if(agArray->at(i)->uniqueID != uniqueID) { //if that is not me
			
			howclose = round(usedSliders->at(1)->getSlideVal(agIndex) * maxDistance);
			float distance = this->dist(currentX, currentY, agArray->at(i)->currentX, agArray->at(i)->currentY);
			
			if((agArray->at(i)->agentType == 1) && (distance < howclose) && isInChain == false)
			{
				if(((sAgent*)agArray->at(i))->boundFxId == 0)
				{
					((sAgent*)agArray->at(i))->boundFxId = uniqueID;
					((sAgent*)agArray->at(i))->lastBoundCAgent = this;
				}
			} 
			
			
			if(((agArray->at(i))->agentType == 1) && (((sAgent*)agArray->at(i))->boundFxId == uniqueID) && (isInChain == false)) //if the comparing agent is a synth and not bound to another fx
			{ 
				
				if( distance < howclose)
				{ 
					
					if(this->contains(agArray->at(i)->uniqueID, false) == false) {
						
						
						((sAgent*)agArray->at(i))->boundSynth->moveBefore(boundSynth->nodeID);
						((sAgent*)agArray->at(i))->boundSynth->set("fxBusNo", uniqueInputBusL);
						((sAgent*)agArray->at(i))->isOnFxBus = true;
						((sAgent*)agArray->at(i))->isOnChainFxBus = true;
						isFed = true;
						countConnected += 1;
						zeroSent.push_back(agArray->at(i)->uniqueID);
					}
					
					controlValFirst = 1 - (distance / howclose);
					//printf("fxmul: %f", controlVal);
					if(controlValFirst != oldControlFirst)
					{
						((sAgent*)agArray->at(i))->boundSynth->set("fxMul", controlValFirst);
						oldControlFirst = controlValFirst;
					}
					
					ofSetColor(mColor);
					ofLine(currentX, currentY, agArray->at(i)->currentX, agArray->at(i)->currentY);
				} 
				else { 
					if(this->contains(agArray->at(i)->uniqueID, true) == true) 
					{ 
						((sAgent*)agArray->at(i))->boundFxId = 0;
						((sAgent*)agArray->at(i))->boundSynth->set("fxMul", 0);
						((sAgent*)agArray->at(i))->boundSynth->set("fxBusNo", (MAXBUSSES * 2) + 14); //send output to the blackhole bus.
						((sAgent*)agArray->at(i))->isOnFxBus = false;
						((sAgent*)agArray->at(i))->isOnChainFxBus = false;
						countConnected -= 1;
						if(countConnected == 0)
						{
							isInChain = false;
							isFed = false;
						}
						
						
						
					}  
				}
			}
			
			if((agArray->at(i)->agentType == 5) && (distance < howclose)  && (isInChain == false))
			{
				if(((frsAgent*)agArray->at(i))->synthCreated == true)
				{
					if(((frsAgent*)agArray->at(i))->boundFxId == 0)
					{
						
						((frsAgent*)agArray->at(i))->boundFxId = uniqueID;
						((frsAgent*)agArray->at(i))->lastBoundCAgent = this;
					}
				}
			} 
			
			
			if(((agArray->at(i))->agentType == 5) && (((frsAgent*)agArray->at(i))->boundFxId == uniqueID) && (isInChain == false)) //if the comparing agent is a frssynth and not bound to another fx
			{ 
				
				if( distance < howclose)
				{ 
					
					if(this->contains(agArray->at(i)->uniqueID, false) == false) {
						
						
						((frsAgent*)agArray->at(i))->boundSynth->moveBefore(boundSynth->nodeID);
						((frsAgent*)agArray->at(i))->boundSynth->set("fxBusNo", uniqueInputBusL);
						((frsAgent*)agArray->at(i))->isOnFxBus = true;
						((frsAgent*)agArray->at(i))->isOnChainFxBus = true;
						isFed = true;
						countConnected += 1;
						zeroSent.push_back(agArray->at(i)->uniqueID);
					}
					
					controlValThird = 1 - (distance / howclose);
					//printf("fxmul: %f", controlVal);
					if(controlValThird != oldControlThird)
					{
						((frsAgent*)agArray->at(i))->boundSynth->set("fxMul", controlValThird);
						oldControlThird = controlValThird;
					}
					
					ofSetColor(mColor);
					ofLine(currentX, currentY, agArray->at(i)->currentX, agArray->at(i)->currentY);
				} 
				else { 
					if(this->contains(agArray->at(i)->uniqueID, true) == true) 
					{ 
						((frsAgent*)agArray->at(i))->boundFxId = 0;
						((frsAgent*)agArray->at(i))->boundSynth->set("fxMul", 0);
						((frsAgent*)agArray->at(i))->boundSynth->set("fxBusNo", (MAXBUSSES * 2) + 14); //send output to the blackhole bus.
						((frsAgent*)agArray->at(i))->isOnFxBus = false;
						((frsAgent*)agArray->at(i))->isOnChainFxBus = false;
						countConnected -= 1;
						if(countConnected == 0)
						{
							isInChain = false;
							isFed = false;
						}
						
						
					}  
				}
			}
			
			
			
			
			if((agArray->at(i)->agentType == 4) && (distance < howclose) && (isInChain == false) && (isFed == false))
			{
				if(currentY > ((cAgent*)agArray->at(i))->currentY)
				{
					if(((cAgent*)agArray->at(i))->boundFxId == 0)
					{
						((cAgent*)agArray->at(i))->boundFxId = uniqueID;
						((cAgent*)agArray->at(i))->lastBoundCAgent = this;
					}
				}
			}
			
			if(((agArray->at(i))->agentType == 4) && (((cAgent*)agArray->at(i))->boundFxId == uniqueID))
			{
				if((distance < howclose) && (currentY > ((cAgent*)agArray->at(i))->currentY))
				{ 
					if(this->contains(agArray->at(i)->uniqueID, false) == false) {
						
						((cAgent*)agArray->at(i))->boundSynth->set("outFxBusNo", uniqueInputBusL);
						((cAgent*)agArray->at(i))->isOnFxBus = true;
						isInChain = true;
						zeroSent.push_back(agArray->at(i)->uniqueID);
						//printf("connected!\n");
					}
					
					controlValSecond = 1 - (distance / howclose);
					
					if(controlValSecond != oldControlSecond)
					{
						((cAgent*)agArray->at(i))->boundSynth->set("fxMul", controlValSecond);
						oldControlSecond = controlValSecond;
					}
					
					ofSetColor(mColor);
					ofLine(currentX, currentY, agArray->at(i)->currentX, agArray->at(i)->currentY);
				} 
				else { 
					if(this->contains(agArray->at(i)->uniqueID, true) == true) 
					{ 
						((cAgent*)agArray->at(i))->boundFxId = 0;
						((cAgent*)agArray->at(i))->boundSynth->set("fxMul", 0);
						((cAgent*)agArray->at(i))->boundSynth->set("fxBusNo", (MAXBUSSES * 2) + 14); //send output to the blackhole bus.
						((cAgent*)agArray->at(i))->isOnFxBus = false;
						isInChain = false;
						
						
						
					}  
				}
				
				
			}
		}
    }
	
}


void cAgent::cleanUp()
{
	if(isOnFxBus) //cAgents only connect to cAgents so we can check for isOnFxBus only(not isOnChainFxBus), bad design but meh, it works.
		lastBoundCAgent->isInChain = false;
	
	for(int i = 0; i < agArray->size(); i++)
	{
		switch(agArray->at(i)->agentType) {
			case 1:
				if(((sAgent*)agArray->at(i))->boundFxId == uniqueID) 
				{ 
					((sAgent*)agArray->at(i))->boundFxId = 0;
					((sAgent*)agArray->at(i))->boundSynth->set("fxMul", 0);
					((sAgent*)agArray->at(i))->boundSynth->set("fxBusNo", (MAXBUSSES * 2) + 14); //send output to the blackhole bus.
					((sAgent*)agArray->at(i))->isOnFxBus = false;
				}
				break;
			case 4:
				if(((cAgent*)agArray->at(i))->boundFxId == uniqueID) 
				{ 
					((cAgent*)agArray->at(i))->boundFxId = 0;
					((cAgent*)agArray->at(i))->boundSynth->set("fxMul", 0);
					((cAgent*)agArray->at(i))->boundSynth->set("fxBusNo", (MAXBUSSES * 2) + 14); //send output to the blackhole bus.
					((cAgent*)agArray->at(i))->isOnFxBus = false;
					((cAgent*)agArray->at(i))->isInChain = false;
				}
				break;
			case 5:
				if(((frsAgent*)agArray->at(i))->boundFxId == uniqueID) 
				{
					((frsAgent*)agArray->at(i))->boundFxId = 0;
					((frsAgent*)agArray->at(i))->boundSynth->set("fxMul", 0);
					((frsAgent*)agArray->at(i))->boundSynth->set("fxBusNo", (MAXBUSSES * 2) + 14); //send output to the blackhole bus.
					((frsAgent*)agArray->at(i))->isOnFxBus = false;
				}
				break;
				
		}
	}
	
	boundSynth->set("fxBusNo", (MAXBUSSES * 2) + 14); //send to blackhole.
	boundSynth->set("t_trig100", 1);
	inBus->free(uniqueInputBusL);
	outBus->free(uniqueOutputBusL);
	
}
