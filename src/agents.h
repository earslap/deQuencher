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

#ifndef _AGENTS
#define _AGENTS

#include "ofMain.h"
#include "console.h"
#include "multislider.h"
#include "sc_osc.h"

class Agent 
	{
	protected:
		vector<int> historyX;
		vector<int> historyY;
		int advanceIndex;
		int sizeMultiplier;
		int oldX, oldY;
		int mColor;
		float playLenRatio; //frame clipping after recording, gets its value from 3rd slider view.
		int maxValueX, minValueX, maxValueY, minValueY;
		int moveRangeX, moveRangeY;
		int maxValueXnew, minValueXnew, maxValueYnew, minValueYnew;
		int addNoiseX, addNoiseY;
		int doesReset; //if doesReset = 1 0 is sent once and this is flagged as true so we don't send 0 again until a new bond is creted, at creation time this is passed to constructor. set by [z 0-1] at console. default is 0.
		
		
		
		
		ofTrueTypeFont* usedFont;
		Console* usedConsole;
		vector<Multislider*>* usedSliders;
		vector<Agent*>* agArray; //the array for the current agents held. collide function will need to be aware of other running agents.
		vector<int> zeroSent;
		
		float map(float value, float istart, float istop, float ostart, float ostop); //map a value to a certain range
		void rangeMap(float value);
		float dist(float x1, float y1, float x2, float y2); //find distance between two points.
		int getUniqueId(); //will return an uniqueID, will generate a random number and check against other agents.
		bool contains(int item, bool deleteAction); //search for item in zeroSent vector, return true if found. id deleteaction == true, that item will be deleted.
		int maxDistance; //will be passed to constructor, buond distance px scale. will be multiplied with slider(1)
		
		virtual void collideTest() {}; //to detect proximity, take actions, will differ for each type.
		virtual void advance() {};
		
	public:
		bool recordEnded;
		bool isSelected;
		int agentType;
		int agIndex; //index of the agent, will change after a delete event.
		int uniqueID; //will be used for interobject communications.
		int currentX;
		int currentY;
		int offsetX, offsetY; //adds to position, to move around objects.
		
		
		Agent();
		~Agent() { };
		void record(int x, int y);
		bool selectTest(int mX, int mY, int aX, int aY);
		
		
	};

class pAgent : public Agent
	{
		string paramString;
	public:
		pAgent(int argColor, int argParamNo, int argDistance, int argResets, ofTrueTypeFont* argFont, Console* argConsole, vector<Multislider*>* argSliders, vector<Agent*>* argAgents);
		void advance();
		void collideTest(); //to detect proximity, take actions.
		int paramNo;
		float oldControl; //previous control value. if same with previous frame, no need to send osc message again.
	};

class tAgent : public Agent//this is like pAgent, but it sends t_trig messages instead of param messages. and sends no values for proximity.
	{
		string trigString;
	public:
		tAgent(int argColor, int argTrigNo, int argDistance, ofTrueTypeFont* argFont, Console* argConsole, vector<Multislider*>* argSliders, vector<Agent*>* argAgents);
		void advance();
		void collideTest();
		int trigNo;
		
	};

class fAgent : public Agent
	{
		
		scBus* myBus;
	public:
		fAgent(int argColor, string argName, scServer* argServer, int argDistance, ofTrueTypeFont* argFont, Console* argConsole, vector<Multislider*>* argSliders, vector<Agent*>* argAgents);
		fAgent::~fAgent();
		void advance();
		void collideTest();
		void cleanUp();
		scSynth* boundSynth;
		string synthName;
		int uniqueInputBusL; //inputs will be uniqueInputBusL and uniqueInputBusL+1
	};

class cAgent : public Agent //chaining fx agent
	{
		
		scBus* inBus;
		scBus* outBus;
		
	public:
		cAgent(int argColor, string argName, scServer* argServer, int argDistance, ofTrueTypeFont* argFont, Console* argConsole, vector<Multislider*>* argSliders, vector<Agent*>* argAgents);
		cAgent::~cAgent();
		bool isOnFxBus; // if this synth is sending audio to another one. to make sure not this is not connected to more than one at the same time.
		bool isInChain; //if this synth is getting input from another cAgent. to make sure this is not connected to more than one at the same time.
		bool isFed; //if this synth is getting input from any other sAgent or frsAgent
		int boundFxId;
		void advance();
		void collideTest();
		void cleanUp();
		scSynth* boundSynth;
		scServer* usedServer;
		string synthName;
		
		cAgent* lastBoundCAgent;
		
		int uniqueInputBusL; //inputs will be uniqueInputBusL and uniqueInputBusL+1
		int uniqueOutputBusL; //inputs will be uniqueInputBusL and uniqueInputBusL+1
		
		float oldControlFirst; //previous control value. if same with previous frame, no need to send osc message again. for agent type 1(sAgent)
		float oldControlSecond; //this is for interactions with agent type 4(cAgent).
		float oldControlThird; //the same for type 5. (blargh, fix this mess...)
		
		int countConnected; //number of sAgents and frsAgents connected at a time. if not 0, isFed will not be set to false when one gets disconnected
	};

class sAgent : public Agent
	{
		
		
	public:
		sAgent(int argColor, string argName, scServer* argServer, ofTrueTypeFont* argFont, Console* argConsole, vector<Multislider*>* argSliders, vector<Agent*>* argAgents);
		~sAgent();
		bool isOnFxBus;
		bool isOnChainFxBus;
		int boundFxId;
		void advance();
		void cleanUp();
		scSynth* boundSynth;
		cAgent* lastBoundCAgent;
		
		string synthName;
		
	};

class frsAgent : public Agent
	{
	public:
		frsAgent(int argColor, string argName, scServer* argServer, ofTrueTypeFont* argFont, Console* argConsole, vector<Multislider*>* argSliders, vector<Agent*>* argAgents, string runPath);
		~frsAgent();
		bool isOnFxBus;
		bool isOnChainFxBus;
		bool synthCreated; //did we download the file and create the synth in SC?
		int boundFxId;
		int uniqueBufferNo;
		void advance();
		void cleanUp();
		void createSynth(string fileName, int numChannels);
		scSynth* boundSynth;
		scServer* boundServer;
		scBuffer* myBuffer;
		
		cAgent* lastBoundCAgent;
		
		bool receivedMessage; //this will set the status for downloaded file. When file is downloaded, createSynth is invoked by application and this is set to true to enable collidetest etc...
		bool dlFailed;
		string rFileName; //the filename of the downloaded file.
		string keyword; //the keyword entered by user.
		int numChannels; //channel count of downloaded file.
		
		
		
	};


#endif