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

#include "console.h"
#include <fstream>


Console::Console(int argX, int argY, int argWidth, int argHeight, ofTrueTypeFont* argFont)
{
	showPoints = true; 
	fadeAmount = 150; 
	maxAttract = 100; 
	rFrameCount = 0; //number of rec frames. 0 is infinite.
	noisePx = 0; 
	isQuanted = false; 
	agentType = 0; //controller by default
	agentParam = 0; //param0 by default
	aSynthName = "noNameGiven"; 
	currentSliderTab = 0;; 
	agentResets = 1; 
	attractSlider = 0.5; 
	
	level = 0;

	
	consData = "deQuencher:>";
	statusText = "Ready...";
	lastChar = 'a';
	
	topX = argX; topY = argY; width = argWidth; height = argHeight;
	usedFont = argFont;
	
	//this->update(); skipping update at constructor now, because we create the instance at setup() and you can't draw something onto screen before draw(), instead update will be called at draw() once.
}

void Console::update()
{
	ofSetColor(10, 10, 10);
	ofRect(topX, topY, width, height);
	ofSetColor(255, 255, 255);
	usedFont->drawString(statusText, 2, ofGetHeight() - 20);
	usedFont->drawString(consData, 2, ofGetHeight() - 8);
	
}

void Console::addChar(int k)
{
	char temp = (char)k;
	if(!((lastChar == ' ') && (temp == ' '))) //to avoid multiple spaces.
	{
		consData = consData + temp;
		this->update();
	}
	lastChar = temp;
}

void Console::addString(string str)
{
	consData = "dequencher:>" + str;
}

void Console::backspace()
{
	if(consData.size() > 12)
	{
		consData.erase(consData.size() - 1);
		this->update();
	}
}

void Console::parseReturn()
{	
	string tempStr, tempStr2;
	
	tempStr2 = consData.substr(12, 1024); //trim the prompt "dequencher:>"
	
	stringstream stream(tempStr2);
	
				
	while( getline(stream, tempStr, ' ') )
	{
        
		splitCommand.push_back(tempStr);
	}
	
	
	if (splitCommand.size() == 0)
    {
		statusText = ("Weee nuthin!");
	} else {
		
		
		switch(splitCommand.size())
		{
			case 2:
				if(splitCommand[0] == "/point")
				{
					switch(atoi(splitCommand[1].c_str()))
					{
						case 1:
							showPoints = true;
							statusText = "point 1 ---> ok";
							break;
						case 0:
							showPoints = false;
							statusText = "point 0 ---> ok";
							break;
						default:
							statusText = "Error, /point [0, 1]";
							break;
					}
					
				} 
				
				else if(splitCommand[0] == "/fade")
				{
					fadeAmount = atoi(splitCommand[1].c_str());
					statusText = "Fade amount set...";
					
				}
				else if(splitCommand[0] == "b")//you will need to constrain these for safety.
				{
					maxAttract = atoi(splitCommand[1].c_str());
					statusText = "Min Bond distance updated for future agents...";
					
					
				}
				else if(splitCommand[0] == "q")
				{
					rFrameCount = atoi(splitCommand[1].c_str());
					statusText = "Rec. frame count updated for future agents...";
					
				}
				else if(splitCommand[0] == "n")
				{
					noisePx = atoi(splitCommand[1].c_str());
					statusText = "Noise amount updated for future agents...";
					
				}
				else if(splitCommand[0] == "quant")
				{
					if(atoi(splitCommand[1].c_str()) > 0)
					{/* this is java code
					 isQuanted = true;
					 sliderCursor = (MultiSlider) sliders.get(0);
					 sliderCursor.quantize = int(splitCommand[1]);
					 sliderCursor = (MultiSlider) sliders.get(1);
					 sliderCursor.quantize = int(splitCommand[1]);
					 sliderCursor = (MultiSlider) sliders.get(2);
					 sliderCursor.quantize = int(splitCommand[1]);
					 sliderCursor = (MultiSlider) sliders.get(3);
					 sliderCursor.quantize = int(splitCommand[1]); */
						statusText = "Quant set";
						
					} 
					else {/* this is java code
						isQuanted = false;
					 sliderCursor = (MultiSlider) sliders.get(0);
					 sliderCursor.quantize = 0;
					 sliderCursor = (MultiSlider) sliders.get(1);
					 sliderCursor.quantize = 0;
					 sliderCursor = (MultiSlider) sliders.get(2);
					 sliderCursor.quantize = 0;
					 sliderCursor = (MultiSlider) sliders.get(3);
					 sliderCursor.quantize = 0;*/
						statusText = "Quant disabled";
						
					}
					
					
					
					
				}
				
				else if((splitCommand[0] == "type") || (splitCommand[0] == "t"))
				{
					agentType = atoi(splitCommand[1].c_str());
					statusText = "Type set to " + splitCommand[1];
					
					
				}
				
				else if((splitCommand[0] == "param") || (splitCommand[0] == "trig") || (splitCommand[0] == "p"))
				{
					agentParam = atoi(splitCommand[1].c_str());
					statusText = "Parameter/trigger set to " + splitCommand[1];
					
					
				}
				else if((splitCommand[0] == "synth") || splitCommand[0] == "s")
				{
					aSynthName = splitCommand[1];
					statusText = "Synth name set to " + splitCommand[1];
					
					
				}
				
				else if(splitCommand[0] == "z")
				{
					agentResets = atoi(splitCommand[1].c_str());
					
					statusText = "Idle: " + splitCommand[1];
					
				}  
				else if(splitCommand[0] == "accel")
				{
					string wPath = getWorkingPath();
					wPath = wPath.substr(0, wPath.length() - CROPPATH);
					
					string tempFile = wPath + "/" + splitCommand[1];
					
					//LOAD THE FILE!
					if(this->loadToMem(tempFile) == 0)
						statusText = "Keyboard accelerators file loaded ok...";
					else
						statusText = "Failed to load file";
				}
				
				else { this->failCommand(); }
				break;
				
			case 1:
				
				if(splitCommand[0] == "scale")
				{/*this is java code
				 
				 currentSliderTab = 0;
				 sliderCursor = (MultiSlider) sliders.get(0);
				 sliderCursor.locked = false;
				 sliderCursor = (MultiSlider) sliders.get(1);
				 sliderCursor.locked = true;
				 sliderCursor = (MultiSlider) sliders.get(2);
				 sliderCursor.locked = true;
				 sliderCursor = (MultiSlider) sliders.get(3);
				 sliderCursor.locked = true;
				 sliderCursor = (MultiSlider) sliders.get(0);
				 sliderCursor.updateAll(false);// caution! quant hardcoded*/
				 statusText = "On scale view...";
				 
				} 
				 
				 
				 
				 else if(splitCommand[0] == "bondd")
				 {/*this is java code
				  currentSliderTab = 1;
				  
				  sliderCursor = (MultiSlider) sliders.get(0);
				  sliderCursor.locked = true;
				  sliderCursor = (MultiSlider) sliders.get(1);
				  sliderCursor.locked = false;
				  sliderCursor = (MultiSlider) sliders.get(2);
				  sliderCursor.locked = true;
				  sliderCursor = (MultiSlider) sliders.get(3);
				  sliderCursor.locked = true;
				  sliderCursor = (MultiSlider) sliders.get(1);
				  sliderCursor.updateAll(false);// caution! quant hardcoded*/
				  statusText = "On bond distance scaling view...";
				  
				 } 
				  else if(splitCommand[0] == "noise")
				  {/*this is java code
				   currentSliderTab = 3;
				   
				   sliderCursor = (MultiSlider) sliders.get(0);
				   sliderCursor.locked = true;
				   sliderCursor = (MultiSlider) sliders.get(1);
				   sliderCursor.locked = true;
				   sliderCursor = (MultiSlider) sliders.get(2);
				   sliderCursor.locked = true;
				   sliderCursor = (MultiSlider) sliders.get(3);
				   sliderCursor.locked = false;
				   sliderCursor.updateAll(false);// caution! quant hardcoded*/
				   statusText = "On noise amount view...";
				   
				  }
				   else if(splitCommand[0] == "sync")
				   {/*this is java code
					currentSliderTab = 2;
					
					sliderCursor = (MultiSlider) sliders.get(0);
					sliderCursor.locked = true;
					sliderCursor = (MultiSlider) sliders.get(1);
					sliderCursor.locked = true;
					sliderCursor = (MultiSlider) sliders.get(2);
					sliderCursor.locked = false;
					sliderCursor = (MultiSlider) sliders.get(3);
					sliderCursor.locked = true;
					sliderCursor = (MultiSlider) sliders.get(2);
					sliderCursor.updateAll(false);// caution! quant hardcoded*/
					statusText = "Frame clipping view...";
					
				   } else if(splitCommand[0] == "about")
				   {
					   statusText = "deQuencher v0.2.1 - Batuhan Bozkurt 2008";
				   } 
				   else if(splitCommand[0] == "sbaitso2")
				   {//wohoo an easter egg here.
					   statusText = "SO, TELL ME ABOUT YOUR PROBLEMS.";
				   } else { this->failCommand(); }
					break;
					
					case 6:
						if((splitCommand[0] == "c") && (splitCommand[1] == "a"))
						{
							maxAttract = atoi(splitCommand[2].c_str());
							attractSlider = atof(splitCommand[3].c_str());
							rFrameCount = atoi(splitCommand[4].c_str());
							noisePx = atoi(splitCommand[5].c_str());
							statusText = "Agent parameters set...";
							
							
						} else { this->failCommand(); }
						break;
					default:
						this->failCommand();
						break;
						
						
		}
					
					
	}
					
					consData = "deQuencher:>";
					this->update();
					splitCommand.clear();
}

void Console::failCommand()
{
	statusText = "";
    for(int i = 0; i < splitCommand.size(); i++)
    {
		if(i == 0) {
			statusText = statusText + splitCommand[i];
		} 
		else 
		{
			statusText = statusText + " " + splitCommand[i];
		}
	}	
	stringstream numTemp;
	numTemp << splitCommand.size();
	
	statusText = statusText + " ---> unrecognised(" + numTemp.str() + " item(s)).";
	
	
}

void Console::slTabUpdate(int viewNo)
{
	string texts[4];
	texts[0] = "On movement scale view.";
	texts[1] = "On bond distance scaling view.";
	texts[2] = "Frame clipping/sync view.";
	texts[3] = "Noise view.";
	
	statusText = texts[viewNo];
	this->update();

}

int Console::loadToMem(string filename)
{
	keyBinds.clear();
	sTypes.clear();
	names.clear();
	level = 0;
	ifstream inFile (filename.c_str());
	string concatWord;
	
	
	if(inFile.is_open() == false)
		return 1;
	
	
	do
	{
		do
		{
			
			tempChar = (char)inFile.get();
			if((tempChar == ' ') || (tempChar == '\n'))
			{
				break;
			}
			concatWord = concatWord + tempChar;
			if((concatWord == "end") && (level != 2))
				return 0;
				
			
		} while(! inFile.eof());
		
		switch(level)
		{
			case 0:
				if(concatWord.length() == 1)
				{
					const char* key = concatWord.c_str(); 
					keyBinds.push_back(key[0]);
				} else
				{
					inFile.close();
					return 1; //error occured
				}
				break;
			case 1:
			{
				stringstream temp(concatWord);
				int tempInt;
				temp >> tempInt;
				if((tempInt >= 0) && (tempInt <= 5))
				{
					sTypes.push_back(tempInt);
				} else
				{
					//cout << word << " has an error. Types can be 0, 1, 2, 3 or 4." << endl;
					inFile.close();
					
					return 1; //error occured
				}
			}
				break;
			case 2:
				names.push_back(concatWord);
				break;
		}
		level = (level + 1) % 3;
		concatWord = "";
		
		
	} while(! inFile.eof());
	
	
	inFile.close();
	return 0;
}

int Console::processKey(char pressed)
{
	
	
	for(int i = 0; i < keyBinds.size(); i++)
	{
		if(keyBinds[i] == pressed)
		{
			switch(sTypes[i])
			{
				case 0:
					consData = consData + "type 0";
					this->parseReturn();
					consData = consData + "param " + names[i]; //THIS REQUIRES INTEGER VALIDATION!!
					this->parseReturn();
					return 0;
					break;
				case 1:
					consData = consData + "type 1";
					this->parseReturn();
					consData = consData + "synth " + names[i];
					this->parseReturn();
					break;
				case 2:
					consData = consData + "type 2";
					this->parseReturn();
					consData = consData + "trig " + names[i]; //THIS REQUIRES INTEGER VALIDATION!!
					this->parseReturn();
					break;
				case 3:
					consData = consData + "type 3";
					this->parseReturn();
					consData = consData + "synth " + names[i];
					this->parseReturn();
					break;
				case 4:
					consData = consData + "type 4";
					this->parseReturn();
					consData = consData + "synth " + names[i];
					this->parseReturn();
					break;
				case 5:
					consData = consData + "type 5";
					this->parseReturn();
					consData = consData + "synth " + names[i];
					this->parseReturn();
					break;
				default:
					break;
			}
		}
	}
}
	

void Console::notifyMode(int mode)
{
	switch(mode)
	{
		case 0:
			statusText = "Normal typing mode...";
			break;
		case 1:
			statusText = "Keyboard accelerators mode...";
			break;
	}
	consData = "deQuencher:>";
	this->update();
	splitCommand.clear();
	
}

string Console::getWorkingPath()
{
	string temp;
	char tempPath[512];
	getcwd(tempPath, 512);
	temp = temp + tempPath;
	return temp;
}