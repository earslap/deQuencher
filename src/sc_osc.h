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

#ifndef _SCOSC
#define _SCOSC


#include <string>
#include "ofxOsc.h"

#define MAXBUSSES 512 // was: max number of busses/2 (*2 = 112 busses (+16 = 128 so goes from 16 to 128))
                      // now: 512. So SC needs 1024 + 16 = 1040 busses alocated.

using namespace std;

class scServer
{
	int nodeCounter;
	int busArray[MAXBUSSES];
	int bufferArray[512];
	
public:
	string ip;
	int port;
	scServer(string argIp, int argPort);
	int nextNodeID();
	int nextFreeBus();
	int nextFreeBuffer();
	void freeBus(int busArg);
	void freeBuffer(int bufferArg);
	
};

class scSynth
{
private:
	
	scServer* boundServer;
	string sName;
	ofxOscSender sender;
	
public:
	scSynth(string argSName, scServer* argServer);
	int nodeID;
	void create();
	void set(string argParamName, float value);
	void free();
	void moveBefore(int idAnch);
	
};

class scBus //to allocate a stereo bus from server
{
private:
scServer* boundServer;

public:
	scBus(scServer* argServer);
	int newBus();
	void free(int busArg); //this expects a bus number between 16 - MAXBUSSES
};
	
class scBuffer
	{
	private:
		scServer* boundServer;
		ofxOscSender sender;
		
	public:
		scBuffer(scServer* argServer);
		int newBuffer();
		void loadFile(string filename, int bufferNo);
		void free(int bufferArg);
		
	};


#endif