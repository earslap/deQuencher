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

#include "multislider.h"


Multislider::Multislider(int argXPos, int argYPos, int argSHeight, int argSWidth, int argBg)
{
	locked = false;
	quantize = 0;
	noOfSlides = 0;
	isSelection = false;
	numSelected = 0;
	
		
    xPos = argXPos;
    yPos = argYPos;
    sHeight = argSHeight;
    sWidth = argSWidth;
    bgColor = argBg;
}

void Multislider::addSlide(int argSColor, float argVal) //add slices by using instance method
{
    slideColor.push_back(argSColor);
    //argVal = this.quantPass(argVal, false); //quantize if needed, falsed for now COMMENTED OUT UNTIL QUANT IMPLEMENTATION
    slideVal.push_back(argVal);
    isSlideLocked.push_back(0);
	isSlideSelected.push_back(0);
    noOfSlides = noOfSlides + 1;
    this->updateAll(false);//we don't reiterate quantization in global update as we've taken care of the values in this method
}

void Multislider::updateAll(bool withQuant)//to update the whole view. the argument if true, will quantise all sliders with quantize variable.
{
    if(!locked) {
		float divisor = float(sWidth)/float(noOfSlides);
		if(divisor > 0)
		{
			
			ofSetColor(bgColor);
			ofRect(xPos, yPos, sWidth, sHeight);
			for(int i = 0; i < noOfSlides; i++)
			{
				colorCursor = slideColor[i];
				slideValCursor = slideVal[i];
				lockCursor = isSlideLocked[i];
				slideSelCursor = isSlideSelected[i];
				
				ofSetColor(colorCursor);
				if(withQuant) { //only quiantize if method arg == true
					
					slideVal[i] = this->quantPass(slideValCursor, quantize > 0); 
				}
				float h = slideValCursor * sHeight;
				ofSetColor(lockCursor ? bgColor + 100 : bgColor); //differ background if it is locked
				ofRect((divisor*i) + xPos, yPos, divisor, sHeight);
				ofSetColor(colorCursor);
				ofRect((divisor*i) + xPos, yPos + (sHeight - h), divisor, h); 
			}
		}
    }
}

void Multislider::inSelection(int argIndex, bool argLockStatus)
{
	switch(argLockStatus)
	{
		case true:
			isSlideSelected[argIndex] = true;
			isSlideLocked[argIndex] = false;
			numSelected++;
			break;
		case false:
			isSlideSelected[argIndex] = false;
			isSlideLocked[argIndex] = true;
			break;
	}
}

void Multislider::noneSelected()
{
	for(int i = 0; i < isSlideLocked.size(); i++)
	{
		isSlideLocked[i] = false;
	}
}

void Multislider::removeSelected()
{
	vector<bool>::iterator slider_iter1;
	vector<int>::iterator slider_iter2 = slideColor.begin();
	vector<float>::iterator slider_iter3 = slideVal.begin();
	vector<bool>::iterator slider_iter4 = isSlideLocked.begin();
	
	for(slider_iter1 = isSlideSelected.begin(); slider_iter1 != isSlideSelected.end();)
	{
		if((*slider_iter1))
		{	
			slider_iter1 = isSlideSelected.erase(slider_iter1);
			slider_iter2 = slideColor.erase(slider_iter2);
			slider_iter3 = slideVal.erase(slider_iter3);
			slider_iter4 = isSlideLocked.erase(slider_iter4);
			noOfSlides--;
			
		} else { slider_iter1++; slider_iter2++; slider_iter3++; slider_iter4++; }
		
		
	}
	this->updateAll(false);//caution quant hardcoded	
		
}

void Multislider::mouseUpdate(int mX, int mY)//updating individual sliders without updating the whole view.
{
	
    if(!locked && (slideVal.size() > 0))//update view only if visible and unlocked
    {	
		float relPosHeight = (min(max(mY, yPos), yPos+sHeight) - yPos);
		float toTarget = 1 - (relPosHeight / sHeight);
		float divisor = (float)sWidth/slideVal.size();
		float relPosX = (min(max(mX, xPos), xPos + sWidth-1) - xPos);
		int currentSlide = floor(relPosX / divisor);
		
		lockCursor = isSlideLocked[currentSlide];
		colorCursor = slideColor[currentSlide];
		
		if(lockCursor == false) {//skip if locked, handle unlocked ones
			ofSetColor(bgColor);
			ofRect(xPos + (currentSlide * divisor), yPos, divisor, sHeight);
			ofSetColor(colorCursor);
			slideVal[currentSlide] = this->quantPass(toTarget, quantize > 0);
			float h = slideVal[currentSlide] * sHeight;
			ofRect(xPos + (currentSlide * divisor), yPos + (sHeight - h), divisor, h);
		}
    } 
	
}

float Multislider::quantPass(float value, int quantMode)//quantizes the incoming value according to quantize variable, or may skip operation with second argument
{
    if(quantMode) { 
		
		float qReciprocal = 1.0/quantize;
		float remainder;
		if(value > qReciprocal)
		{
			remainder = fmodf(value, qReciprocal);
			
		} 
		else { 
			remainder = value; 
		}
		if(remainder > (qReciprocal/2))
		{
			value = value + (qReciprocal - remainder);
		} 
		else { 
			
			value  = value - remainder; 
		}
    }
	return value; //we return the result (between 0-1)
}

float Multislider::getSlideVal(int argSlideNo)
{
	return slideVal[argSlideNo];
}
