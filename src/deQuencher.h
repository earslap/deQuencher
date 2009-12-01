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

#ifndef _DEQUENCHER
#define _DEQUENCHER

#define OF_ADDON_USING_OFXOSC

#include "ofMain.h"
#include "ofAddons.h"


#include <algorithm>

#include "multislider.h"
#include "agents.h"
#include "selector.h"
#include "sc_osc.h"
#include "console.h"



#define MHEIGHT 600
#define MWIDTH 900
#define CONSOLEOFFSET 28

//slider height
#define SHEIGHT 100
//agent canvas height (mheight - consoleoffset - sheight)
#define AHEIGHT 472

class deQuencher : public ofSimpleApp{
	
	public:
		
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseWheel(int direction);
		void mouseReleased();
		//-------------------
		int mouseRegion(); //returns current anchor region of mouse. 0 is canvas, 1 is multislider, 2 is other(console).
		void switchTab(int direction);
	
		ofxOscReceiver oscReceiver;
				
		int initDone; //to see if we drew the gui elements at startup, as we can't draw them in setup. so need to check it in draw();
		
		bool isRecording;
		bool mouseDraggingL;
		bool mouseDraggingM; //middle mouse dragging.
		bool deleteFlag;
		bool dumpFlag;
		struct myMouse { int x, y, pButton; };
		struct myAnchor { int x, y; };
		int recFrameIndex;
		int currentSliderTab;
		int agentResets;
		int tempAnchorX, tempAnchorY; //needed for offset addition calculations, will contain the mouse pos for the previous frames.
	
	
	
		bool keyAccelMode;
	
		string runningPath; //current executable path.
	
			
		myMouse mickey;
		myAnchor anchor;
		vector<Agent*>* myAgents;
		vector<Multislider*>* myMSliders;
		
		ofTrueTypeFont franklin;
		ofTrueTypeFont monaco;
		
		Selector* mySelect;
		Console* myConsole;
		Multislider* myMSlider;
		
		
		scServer* myServer;
		
};



#endif