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

#ifndef _MULTISLIDER
#define _MULTISLIDER

#include "ofMain.h"

class Multislider
{
	int bgColor; //will be specified as hex
	vector <int> slideColor;
	vector <float> slideVal;
	vector <bool> isSlideLocked;
	vector <bool> isSlideSelected;
	
	int noOfSlides;
	int xPos, yPos, sHeight, sWidth;
	
	
	int colorCursor;
	float slideValCursor;
	bool lockCursor;
	bool slideSelCursor;
	
public:
		bool locked;//locking is necessary when instance becomes invisible(by drawing stg on top of it)
		int quantize; //number of steps = quantize + 1
		bool isSelection; //if there is no selection, unlock all sliders instead of locking them all this is global because set to false from draw loop, after the checking for loop
		int numSelected; //number of selected agents
		
		Multislider(int argXPos, int argYPos, int argSHeight, int argSWidth, int argBg);
		void addSlide(int argSColor, float argVal); //add slices by using instance method
		void updateAll(bool withQuant);
		void mouseUpdate(int mX, int mY);//updating individual sliders without updating the whole view.
		void lockSlide(int argSlideNo);
		void unlockSlides();
		float getSlideVal(int argSlideNo);
		void inSelection(int argIndex, bool argLockStatus);
		void noneSelected();//if there is no selection, unlock all.
		void removeSelected();
		void bucketDelSlider(int index);
		float quantPass(float value, int quantMode);//quantizes the incoming value according to quantize variable, or may skip operation with second argument
				
				
};


#endif