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

tAgent::tAgent(int argColor, int argTrigNo, int argDistance, ofTrueTypeFont* argFont, Console* argConsole, vector<Multislider*>* argSliders, vector<Agent*>* argAgents)
{
	ostringstream numTemp;
	string numStr;
	
	advanceIndex = 0;
	isSelected = false;
	sizeMultiplier = 1;
	agentType = 2;
	
	offsetX = 0;
	offsetY = 0;
	
	
	trigNo = argTrigNo;
	numTemp << argTrigNo;
	numStr = numTemp.str();
	
	trigString = "t_trig" + numStr;
	
	
	usedFont = argFont;
	usedConsole = argConsole;
	usedSliders = argSliders;
	mColor = argColor;
	agArray = argAgents;
	maxDistance = argDistance;
	
	
	uniqueID = this->getUniqueId();
	
	
	
}

void tAgent::advance()
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
		ofEllipse(currentX, currentY, 2 * sizeMultiplier, 2 * sizeMultiplier);
		ofSetColor(255, 255, 255);
		usedFont->drawString(trigString, currentX + 5, currentY - 5);
	}
	
	this->collideTest();
	advanceIndex++;
	advanceIndex = advanceIndex % clippedHistory;
	
}




void tAgent::collideTest() //collision detection for trigger parameters. sends 1 at first intercourse then sends a zero and waits for next bond. (reimplement note: when does it send 0? can't see it)
{
    int howclose;
    for(int i = 0; i < agArray->size(); i++)
    {
		if(agArray->at(i)->uniqueID != uniqueID) { //if that is not me
			//agentCursor = (myAgent) agent.get(i);
			if((agArray->at(i)->agentType == 1) || (agArray->at(i)->agentType == 3) || (agArray->at(i)->agentType == 4)) //if the comparing agent is a synth or fx
			{ 
				howclose = round(usedSliders->at(1)->getSlideVal(agIndex) * maxDistance); 
				float distance = this->dist(currentX, currentY, agArray->at(i)->currentX, agArray->at(i)->currentY);
				if( distance < howclose)
				{ 
					if(this->contains(agArray->at(i)->uniqueID, false) == false) {
						
						switch(agArray->at(i)->agentType)
						{
							case 1:
								((sAgent*)agArray->at(i))->boundSynth->set(trigString, 1);
								break;
							case 3:
								((fAgent*)agArray->at(i))->boundSynth->set(trigString, 1);
								break;
							case 4:
								((cAgent*)agArray->at(i))->boundSynth->set(trigString, 1);
								break;
							
						}
						//agArray->at(i)->agentType == 1 ? ((sAgent*)agArray->at(i))->boundSynth->set(trigString, 1) : ((fAgent*)agArray->at(i))->boundSynth->set(trigString, 1);
						zeroSent.push_back(agArray->at(i)->uniqueID);//zeroSent acts as oneSent here. 1 trigs.
							
					}
					
					
					
					ofSetColor(mColor);
					ofLine(currentX, currentY, agArray->at(i)->currentX, agArray->at(i)->currentY);
				} 
				else { this->contains(agArray->at(i)->uniqueID, true); } //the function has the second arg as true so if it is found, it will be deleted. 
			}
		}
    }
}
