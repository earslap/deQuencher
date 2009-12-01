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

#include "ofMain.h"
#include <unistd.h>

#ifndef _CCONSOLE
#define _CCONSOLE

#define CROPPATH 34 //39 if debug mode, 34 in release mode.

class Console
{
	string consData;
	string statusText;
	int topX, topY, width, height;
	ofTrueTypeFont* usedFont;
	char lastChar;
	
	//keyboard accelerator data
	char tempChar;
	int level; //should be init to 0, level from left to right 0 1 2 0 1 2 etc... while parsing
	
	vector<char> keyBinds;
	vector<int> sTypes;
	vector<string> names;
	//end of keyboard accelerator data
	
	vector <string> splitCommand;
	
	void failCommand();
	

	
	public:
	Console(int argX, int argY, int argWidth, int argHeight, ofTrueTypeFont* argFont);
	void update();
	void addChar(int k);
	void addString(string str);
	void backspace();
	void returned();
	void parseReturn();
	void slTabUpdate(int viewNo); //when slider tabs change, change status text
	void notifyMode(int mode); //print keyboard accelerator mode to user.
	
	string getWorkingPath(); //returns path of the running executable(self).
	
	//keyboard accelerator functions
	int loadToMem(string filename);
	int processKey(char pressed);
	
	
	//global synth/param vars set from console
	bool showPoints;  
	int fadeAmount;
	int maxAttract; 
	int rFrameCount;  
	int noisePx; 
	bool isQuanted;
	int agentType;  
	int agentParam;  
	string aSynthName; 
	int currentSliderTab; 
	int agentResets;  
	float attractSlider; 
	
};

#endif