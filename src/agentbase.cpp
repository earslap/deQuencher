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

Agent::Agent()
{
	advanceIndex = 0;
	isSelected = false;
	sizeMultiplier = 1;
	
}

void Agent::record(int x, int y)
{
	historyX.push_back(x);
	historyY.push_back(y);
	
	if(historyX.size() == 1)
    { 
		
		minValueX = historyX[0];
		maxValueX = historyX[0];
		minValueY = historyY[0];
		maxValueY = historyY[0];
    }
	
	
    maxValueX = max(historyX[historyX.size() - 1], maxValueX);
    minValueX = min(historyX[historyX.size() - 1], minValueX);
    maxValueY = max(historyY[historyY.size() - 1], maxValueY);
    minValueY = min(historyY[historyY.size() - 1], minValueY);
}

bool Agent::selectTest(int mX, int mY, int aX, int aY) //mouseX, mouseY, anchorX and Y
{
	if((currentX > min(aX, mX)) &&
	   (currentX < max(aX, mX)) &&
	   (currentY > min(aY, mY)) &&
	   (currentY < max(aY, mY)))
	{
		isSelected = true;
        sizeMultiplier = 2;
    } else { 
        isSelected = false;
		sizeMultiplier = 1; 
    }
    
	return isSelected;
}

float Agent::map(float value, float istart, float istop, float ostart, float ostop) //map a value to a certain range
{
	return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

void Agent::rangeMap(float value)
{
    moveRangeX = round((maxValueX - minValueX) * (value - 0.5));
    moveRangeY = round((maxValueY - minValueY) * (value - 0.5));
	
    minValueXnew = minValueX-moveRangeX;
    maxValueXnew = maxValueX+moveRangeX;
    minValueYnew = minValueY-moveRangeY;
    maxValueYnew = maxValueY+moveRangeY;
	
    if(minValueX == maxValueX) { //map() behaves odd if these are equal so we need to avoid
		maxValueX = maxValueX+1;
    } 
    else if(minValueY == maxValueY) { 
		maxValueY = maxValueY+1;
    }
	
}

float Agent::dist(float x1, float y1, float x2, float y2) //find distance between two points.
{
	return sqrt(pow((x2-x1), 2) + pow((y2-y1), 2));
}

int Agent::getUniqueId() //will return an uniqueID, will generate a random number and check against other agents. 
{	
	bool uniqueFound = false;
	unsigned int randID;
	while(!uniqueFound)
	{
		randID = ofRandom(1, 16777215); //maximum num of unique agents.
		uniqueFound = true;
		for(int i = 0; i < agArray->size(); i++)
		{
			if(agArray->at(i)->uniqueID == randID)
			{
				uniqueFound = false;
				break;
			}
		}
	}
	return randID;
}

bool Agent::contains(int item, bool deleteAction)
{
	vector<int>::iterator zIter;
	for(zIter = zeroSent.begin(); zIter != zeroSent.end(); zIter++)
	{
		if(*zIter == item)
		{
			if(deleteAction)
				zIter = zeroSent.erase(zIter);
			
			return true;
		}
	}
	return false;
}



