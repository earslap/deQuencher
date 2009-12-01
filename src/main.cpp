#include "ofMain.h"
#include "deQuencher.h"

//========================================================================
int main( ){

	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofSetupOpenGL(MWIDTH, MHEIGHT, OF_WINDOW);			// <-------- setup the GL context
	
	// this kicks off the running of my app
	ofRunApp(new deQuencher);
	
}
