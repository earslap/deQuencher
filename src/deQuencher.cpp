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

#include "deQuencher.h"

//--------------------------------------------------------------
void deQuencher::setup(){	 
	ofSetWindowTitle("deQuencher v0.2.1");
	ofEnableAlphaBlending(); 
	ofSetBackgroundAuto(false);
	ofBackground(0, 0, 0);
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofSeedRandom();
	tempAnchorX = 0;
	tempAnchorY = 0;
	keyAccelMode = false;
	
	
	printf("deQuencher is listening for osc messages on port 9211\n");
	oscReceiver.setup(9211);
	
	mouseDraggingL = false;
	mouseDraggingM = false;
	mickey.pButton = -1; //-1 if no button is pressed.
	deleteFlag = false;
	dumpFlag = false; //for debugging purposes. will dump internals of the selected agent.
	recFrameIndex = 0; //how many frames recorded?
	currentSliderTab = 0;
	initDone = 0;
	
	franklin.loadFont("frabk.ttf", 7);
	monaco.loadFont("mono.ttf", 7);
	
	myServer = new scServer("127.0.0.1", 57110);
	
	myConsole = new Console(0, MHEIGHT - CONSOLEOFFSET, MWIDTH, 28, &monaco);
	
	
	runningPath = myConsole->getWorkingPath();
	runningPath = runningPath.substr(0, runningPath.length() - CROPPATH);
	
	
	myAgents = new vector<Agent*>;
	myMSliders = new vector<Multislider*>;
	
	
	myMSliders->push_back(new Multislider(0, MHEIGHT - CONSOLEOFFSET - SHEIGHT, SHEIGHT, MWIDTH, 0x0a141e));
	myMSliders->push_back(new Multislider(0, MHEIGHT - CONSOLEOFFSET - SHEIGHT, SHEIGHT, MWIDTH, 0x141e28));
	myMSliders->push_back(new Multislider(0, MHEIGHT - CONSOLEOFFSET - SHEIGHT, SHEIGHT, MWIDTH, 0x1e2832));
	myMSliders->push_back(new Multislider(0, MHEIGHT - CONSOLEOFFSET - SHEIGHT, SHEIGHT, MWIDTH, 0x28323c));
	
	
	myMSliders->at(1)->locked = true;
	myMSliders->at(2)->locked = true;
	myMSliders->at(3)->locked = true;
	
	myMSliders->at(0)->locked = false;
	myMSliders->at(0)->updateAll(false);
	
	
	
	
	
}

//--------------------------------------------------------------
void deQuencher::update(){
	
		
}

//--------------------------------------------------------------
void deQuencher::draw(){
	
	if(initDone < 3)
	{
		myConsole->update();
		myMSliders->at(0)->updateAll(false);
		initDone++;		
	}
	
	ofSetColor(0, 0, 0, myConsole->fadeAmount);
	ofRect(0, 0, MWIDTH, MHEIGHT - CONSOLEOFFSET - SHEIGHT); 
	
	for(int i = 0; i < 4; i++)
		myMSliders->at(i)->numSelected = 0; //this is at the beginning of draw so that mousereleased handler will report the correct number of selections.
	
	if(mouseDraggingL && (this->mouseRegion() == 0))
	{
		ofSetColor(200, 0, 0);
		ofEllipse(anchor.x, anchor.y, 2, 2);
		myAgents->at(myAgents->size() - 1)->record((mickey.x + MWIDTH) % MWIDTH, (mickey.y + MHEIGHT - CONSOLEOFFSET - SHEIGHT) % (MHEIGHT - CONSOLEOFFSET - SHEIGHT));
		recFrameIndex++;
		if((myConsole->rFrameCount > 0) && (recFrameIndex == myConsole->rFrameCount - 1))
		{
			mouseDraggingL = false;
			recFrameIndex = 0;
		}
	}
	
	if(mouseDraggingM)
	{
		for(int i = 0; i < myAgents->size(); i++)
		{
			if(myAgents->at(i)->isSelected)
			{
				myAgents->at(i)->offsetX += mickey.x - tempAnchorX;
				myAgents->at(i)->offsetY += mickey.y - tempAnchorY;
				
			}
			
		}
		tempAnchorX = mickey.x;
		tempAnchorY = mickey.y;
	}
	
	if(mickey.pButton == 2)
	{
		mySelect->inDrag(mickey.x, mickey.y);
		
	}
	
	
	
	for(int i = 0; i < myAgents->size(); i++)
	{
		if(mickey.pButton == 2)
		{	
			for(int k = 0; k < 4; k++)
				myMSliders->at(k)->inSelection(i, myAgents->at(i)->selectTest(mickey.x, mickey.y, mySelect->anchorX, mySelect->anchorY));
			
		}
		
		switch(myAgents->at(i)->agentType)
		{
			case 0:
				((pAgent *) myAgents->at(i))->advance();
				break;
			case 1:
				((sAgent *) myAgents->at(i))->advance();
				break;
			case 2:
				((tAgent *) myAgents->at(i))->advance();
				break;
			case 3:
				((fAgent *) myAgents->at(i))->advance();
				break;
			case 4:
				((cAgent *) myAgents->at(i))->advance();
				break;
			case 5:
				((frsAgent *) myAgents->at(i))->advance();
				break;
				
			default:
				break;
		}
	}
	
	
	
	
	if(deleteFlag)
	{	
		
		vector<Agent*>::iterator agent_iter;
		for(agent_iter = myAgents->begin(); agent_iter != myAgents->end();)
		{
			if((*agent_iter)->isSelected)
			{
				switch((*agent_iter)->agentType)
				{
					case 1:
						((sAgent*)*agent_iter)->cleanUp();
						delete ((sAgent*)*agent_iter);
						
						break;
					case 3:
						((fAgent*)*agent_iter)->cleanUp();
						delete ((fAgent*)*agent_iter);
						break;
					case 4:
						((cAgent*)*agent_iter)->cleanUp();
						delete ((cAgent*)*agent_iter);
						break;
					case 5:
						((frsAgent*)*agent_iter)->cleanUp();
						delete ((frsAgent*)*agent_iter);
						break;
				}
				
				agent_iter = myAgents->erase(agent_iter);
				
			} else { agent_iter++; }
			
		}
		
		for(int newIndexes = 0; newIndexes < myAgents->size(); newIndexes++)
		{			
			myAgents->at(newIndexes)->agIndex = newIndexes;
		}
		
		for(int i = 0; i < 4; i++)
			myMSliders->at(i)->removeSelected();
		
		
		deleteFlag = false;
		
		
	}
	
	if(myConsole->isLoadingPreset)
	{
		myConsole->iterateLoadedItems();
	}
	
	if(myConsole->isPutIssued == true)
	{
		this->addAgent(myConsole->putX, myConsole->putY);
		myConsole->isPutIssued = false;		
	}
	
	if(dumpFlag)
	{
		vector<Agent*>::iterator agent_iter;
		for(agent_iter = myAgents->begin(); agent_iter != myAgents->end();)
		{
			
			if((*agent_iter)->isSelected)
			{
				
				switch((*agent_iter)->agentType)
				{
						
					case 1:
						printf("\nDEBUG INFO:\n");
						printf("Type: sAgent\n");
						printf("synthName: %s", ((sAgent*)(*agent_iter))->synthName.c_str());
						printf("\nisOnFxBus: %i", ((sAgent*)(*agent_iter))->isOnFxBus);
						printf("\nboundFxId: %i", ((sAgent*)(*agent_iter))->boundFxId);
						printf("\nuniqueID: %i", ((cAgent*)(*agent_iter))->uniqueID);
						printf("\n-----------------------");
						agent_iter++;
						
						break;
					case 3:
						printf("\nDEBUG INFO:\n");
						printf("Type: fAgent\n");
						printf("synthName: %s", ((fAgent*)(*agent_iter))->synthName.c_str());
						printf("\nuniqueID: %i", ((fAgent*)(*agent_iter))->uniqueID);
						printf("\n-----------------------");
						agent_iter++;
						
						break;
					case 4:
						
						printf("\nDEBUG INFO:\n");
						printf("Type: cAgent\n");
						printf("synthName: %s", ((cAgent*)(*agent_iter))->synthName.c_str());
						printf("\nisOnFxBus: %i", ((cAgent*)(*agent_iter))->isOnFxBus);
						printf("\nboundFxId: %i", ((cAgent*)(*agent_iter))->boundFxId);
						printf("\ninBus: %i", ((cAgent*)(*agent_iter))->uniqueInputBusL);
						printf("\nisInChain: %i", ((cAgent*)(*agent_iter))->isInChain);
						printf("\nisFed: %i", ((cAgent*)(*agent_iter))->isFed);
						printf("\noutBus: %i", ((cAgent*)(*agent_iter))->uniqueOutputBusL);
						printf("\ncountConnected: %i", ((cAgent*)(*agent_iter))->countConnected);
						printf("\nuniqueID: %i", ((cAgent*)(*agent_iter))->uniqueID);
						printf("\n-----------------------");
						agent_iter++;
						break;
					default:
						break;
				}
				
				
				
			} else { agent_iter++; }
			
		}
		
		dumpFlag = false;
	}
	
	while( oscReceiver.hasWaitingMessages() )
	{
		ofxOscMessage m;
		oscReceiver.getNextMessage(&m);
		
		char msgString[16384];
		strcpy(msgString, m.getAddress());
		
		int status = m.getArgAsInt32(0); // 1 if file is downloeded, or else 0
		int idReceived = m.getArgAsInt32(1); //id for calling synth.
		int numChan = m.getArgAsInt32(3); //number of channels in downloaded file.
		string dlFileName = m.getArgAsString(2); //path to the downloaded file.
		
		vector<Agent*>::iterator agent_iter;
		for(agent_iter = myAgents->begin(); agent_iter != myAgents->end();)
		{
			if((*agent_iter)->agentType == 5)
			{
				if(((frsAgent*)*agent_iter)->uniqueID == idReceived)
				{
					if(status)
						((frsAgent*)*agent_iter)->createSynth(runningPath + "/freeSound/" + dlFileName, numChan);
					else
						((frsAgent*)*agent_iter)->dlFailed = true;
				}
				agent_iter++;
			}	else { agent_iter++; }
		}
	
				
	}
	
}

//--------------------------------------------------------------
void deQuencher::keyPressed  (int key){
	if(keyAccelMode == false) //if not in keyboard accelerators mode
	{
		
		switch(key)
		{
			case 8: //if del pressed
				deleteFlag = true;
				for(int i = 0; i < 4; i++)
				{
					myMSliders->at(i)->noneSelected(); //clear all selection after deletion.
					myMSliders->at(i)->updateAll(false);//caution quant hardcoded
				}
				break;
			case 127: //if backspace
				myConsole->backspace();
				break;
			case 13: //if return
				myConsole->parseReturn();
				//myConsole->returned();
				break;
			case 356://left arrow
				this->switchTab(-1);
				break;
			case 358://right arrow
				this->switchTab(1);
				break;
			case 34:// " pressed to dump. (debug)
				dumpFlag = true;
				break;
			case 258: //F2 pressed, switch keyboard accelerators
				keyAccelMode = !keyAccelMode;
				myConsole->notifyMode(1); //print accel status
				break;
			default:
				//printf("%d ", key);
				if( ((key > 45) && (key < 58)) || ((key > 64) && (key < 91)) || ((key > 96) && (key < 123)) || (key == 32))
				{
					
					myConsole->addChar(key);
				}
				break;
		}
	} else if(keyAccelMode)
	{
		switch(key)
		{
			case 258://if F2 pressed, get out of accelerator mode.
				keyAccelMode = !keyAccelMode;
				myConsole->notifyMode(0); //print accel status
				break;
			case 8: //if del pressed
				deleteFlag = true;
				for(int i = 0; i < 4; i++)
				{
					myMSliders->at(i)->noneSelected(); //clear all selection after deletion.
					myMSliders->at(i)->updateAll(false);//caution quant hardcoded
				}
				break;
			case 356://left arrow
				this->switchTab(-1);
				break;
			case 358://right arrow
				this->switchTab(1);
				break;
			case 34:// " pressed to dump. (debug)
				dumpFlag = true;
				break;
			default:
				myConsole->processKey((char)key);
		}
		
	}
}

//--------------------------------------------------------------
void deQuencher::keyReleased  (int key){ 
	
}

//--------------------------------------------------------------
void deQuencher::mouseMoved(int x, int y ){
	mickey.x = x;
	mickey.y = y;
}

//--------------------------------------------------------------
void deQuencher::mouseDragged(int x, int y, int button){
	
	mickey.x = x;
	mickey.y = y;
	
	if(this->mouseRegion() == 1)
	{
		myMSliders->at(currentSliderTab)->mouseUpdate(mickey.x, mickey.y);
	}
	
}

//--------------------------------------------------------------
void deQuencher::mousePressed(int x, int y, int button){
	
	mickey.x = x;
	mickey.y = y;
	anchor.x = x;
	anchor.y = y;
	
	//shift and ctrl keys hack for 2 or less button mice
	if ((button == 0) && glutGetModifiers() == GLUT_ACTIVE_SHIFT) { 
		button = 2; 
	}
	if ((button == 0) && glutGetModifiers() == GLUT_ACTIVE_ALT) { 
		button = 1; 
	}
	//end hack
	
	switch(button)
	{
		case 0:
			if(this->mouseRegion() == 0)
			{
				this->addAgent(x, y);
			} 
			
			mouseDraggingL = true;
			break;
		case 1:
			tempAnchorX = mickey.x;
			tempAnchorY = mickey.y;
			mouseDraggingM = true;
			break;
		case 2:
			
			mySelect = new Selector(mickey.x, mickey.y);
			
			mickey.pButton = button;
			break;
		default:
			break;
	}
}

//--------------------------------------------------------------
void deQuencher::mouseReleased(){
	mouseDraggingL = false;
	mouseDraggingM = false;
	recFrameIndex = 0;
	if(mickey.pButton == 2)
	{
		for(int i = 0; i < 4; i++)
		{
			if(myMSliders->at(i)->numSelected == 0)
			{
				myMSliders->at(i)->noneSelected(); //if no selection, unlock all.
			}
		}
		
		
		myMSliders->at(currentSliderTab)->updateAll(false);//caution quant hardcoded
		delete mySelect;
	}
	mickey.pButton = -1;
	
}

/////////////////////////////////////////////////////////////////

void deQuencher::mouseWheel(int direction)
{
	this->switchTab(direction);
}

int deQuencher::mouseRegion(){
	if(anchor.y < MHEIGHT - CONSOLEOFFSET - SHEIGHT)
	{
		return 0;
	} 
	else if((anchor.y > MHEIGHT - CONSOLEOFFSET - SHEIGHT) && (anchor.y < MHEIGHT - CONSOLEOFFSET))
	{
		return 1;
	} 
	else 
	{ 
		return 2; //on console 
	}
	
}

void deQuencher::switchTab(int direction) //switch multislider tabs direction -1 or 1
{
	currentSliderTab = (currentSliderTab + 4 + direction) % 4;
	
	myMSliders->at((currentSliderTab + 5) % 4)->locked = true;
	myMSliders->at((currentSliderTab + 6) % 4)->locked = true;
	myMSliders->at((currentSliderTab + 7) % 4)->locked = true;
	
	myMSliders->at(currentSliderTab)->locked = false;
	myMSliders->at(currentSliderTab)->updateAll(false);
	
	myConsole->slTabUpdate(currentSliderTab);
	
	
}

void deQuencher::addAgent(int x, int y)
{
	string fakedCommand = "put "; //constructing a fake put command for preset saving.
	stringstream fakeX, fakeY;
	fakeX << x; fakeY << y;
	fakedCommand = fakedCommand + fakeX.str() + " " + fakeY.str();
	myConsole->issuedCommands.push_back(fakedCommand);
	
	unsigned int rndColor = ofRandom(0, 16777215);
	
	switch(myConsole->agentType % 6)
	{
	case 0://if param agent
	myAgents->push_back(new pAgent(rndColor, myConsole->agentParam, myConsole->maxAttract, myConsole->agentResets, &franklin, myConsole, myMSliders, myAgents));
	break;
	case 1://if synth agent
	myAgents->push_back(new sAgent(rndColor, myConsole->aSynthName, myServer, &franklin, myConsole, myMSliders, myAgents));
	break;
	case 2:
	myAgents->push_back(new tAgent(rndColor, myConsole->agentParam, myConsole->maxAttract, &franklin, myConsole, myMSliders, myAgents));
	break;
	case 3:
	myAgents->push_back(new fAgent(rndColor, myConsole->aSynthName, myServer, myConsole->maxAttract, &franklin, myConsole, myMSliders, myAgents));
	break;
	case 4:
	myAgents->push_back(new cAgent(rndColor, myConsole->aSynthName, myServer, myConsole->maxAttract, &franklin, myConsole, myMSliders, myAgents));
	break;
	case 5:
	myAgents->push_back(new frsAgent(rndColor, myConsole->aSynthName, myServer, &franklin, myConsole, myMSliders, myAgents, runningPath));
	break;
	}
	myAgents->at(myAgents->size() - 1)->agIndex = myAgents->size() - 1;
	myAgents->at(myAgents->size() - 1)->record(x, y);

	myMSliders->at(0)->addSlide(rndColor, 0.5);
	myMSliders->at(1)->addSlide(rndColor, 0.5);
	myMSliders->at(2)->addSlide(rndColor, 1); //frame clipping slider
	myMSliders->at(3)->addSlide(rndColor, 0);
}

