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

#include "sc_osc.h"

scServer::scServer(string argIp, int argPort)
{
	ip = argIp;
	port = argPort;
	nodeCounter = 1000;
	for(int i = 0; i < MAXBUSSES; i++)
	{
		busArray[i] = 0;//all busses are free.
	}
	
	for(int i = 0; i < 512; i++)
	{
		bufferArray[i] = 0;//all buffers are free.
	}
}

int scServer::nextNodeID()
{
	nodeCounter++;
	return nodeCounter;
}

int scServer::nextFreeBus()
{
	for(int i = 0; i < MAXBUSSES; i++)
	{
		if(busArray[i] == 0)
		{
			busArray[i] = 1; //is now in use
			return i;
		}
	}
	return MAXBUSSES; //if we reach here, all busses are in use(very unlikely though). send the blackhole bus.
}

int scServer::nextFreeBuffer()
{
	for(int i = 0; i < 512; i++)
	{
		if(bufferArray[i] == 0)
		{
			bufferArray[i] = 1; //is now in use
			return i;
		}
	}
	
}

void scServer::freeBus(int busArg)
{
	busArray[busArg] = 0;
}

void scServer::freeBuffer(int bufferArg)
{
	bufferArray[bufferArg] = 0;
}


////////////////////////scSynth

scSynth::scSynth(string argSName, scServer* argServer)
{
	sName = argSName;
	boundServer = argServer;
	nodeID = boundServer->nextNodeID();
	sender.setup(boundServer->ip.c_str(), boundServer->port);
	
}

void scSynth::create()
{
	ofxOscMessage m;
	m.setAddress("/s_new");
	m.addStringArg(sName.c_str());
	m.addIntArg(nodeID);
	sender.sendMessage(m);

}

void scSynth::free()
{
	ofxOscMessage m;
	m.setAddress("/n_free");
	m.addIntArg(nodeID);
	sender.sendMessage(m);
}

void scSynth::set(string argParamName, float value)
{
	ofxOscMessage m;
	m.setAddress("/n_set");
	m.addIntArg(nodeID);
	m.addStringArg(argParamName.c_str());
	m.addFloatArg(value);
	sender.sendMessage(m);
	//printf("/n_set, %s, %f\n", argParamName.c_str(), value);

}

void scSynth::moveBefore(int idAnch)
{
	ofxOscMessage m;
	m.setAddress("/n_before");
	m.addIntArg(nodeID);
	m.addIntArg(idAnch);
	sender.sendMessage(m);
}


////////////////////////scBus
scBus::scBus(scServer* argServer)
{
	boundServer = argServer;
}

int scBus::newBus()
{
	int tempBus = boundServer->nextFreeBus();
	
	tempBus = (tempBus * 2) + 16;
	
	return(min(tempBus, (MAXBUSSES * 2) + 16));
}

void scBus::free(int busArg)
{	
	boundServer->freeBus((busArg-16) / 2);
}

/////////////////////////scBuffer

scBuffer::scBuffer(scServer* argServer)
{
	boundServer = argServer;
	sender.setup(boundServer->ip.c_str(), boundServer->port);
}

int scBuffer::newBuffer()
{
	int tempBuffer = boundServer->nextFreeBuffer();
	
	return(tempBuffer + 512);
}

void scBuffer::free(int bufferArg)
{	
	ofxOscMessage m;
	m.setAddress("/b_free");
	m.addIntArg(bufferArg);
	sender.sendMessage(m);
	boundServer->freeBuffer(bufferArg - 512);
}

void scBuffer::loadFile(string filename, int bufferNo)
{
	ofxOscMessage m;
	m.setAddress("/b_allocRead");
	m.addIntArg(bufferNo);
	m.addStringArg(filename.c_str());
	sender.sendMessage(m);

}