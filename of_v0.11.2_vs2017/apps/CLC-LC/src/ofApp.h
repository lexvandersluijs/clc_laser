#pragma once

const int OF_KEY_SPACE = 32;


#include "ofMain.h"
#include "ofxLaserManager.h"
#include "ofxOsc.h"

#include "ShowElement.h"

#include <list>

// listening port
#define PORT 12345

// max number of strings to display
#define NUM_MSG_STRINGS 20

class ofApp : public ofBaseApp
{

public:
	ofApp();
	~ofApp();

    void setup();
    void update();
    void draw();

    
	void setSelectedShowElement(int index, bool fromOSC);
    void keyPressed(int key);
    
protected:
	int selectedShowElementIndex = 0;

	std::vector<ShowElement*> showElements;
	std::vector<ofParameter<bool>*> selectedElementBooleans;

    ofxLaser::Manager laserManager;
    
	void selectedShowElementChanged(const void* sender, bool & newValue);


	// OSC stuff
	ofTrueTypeFont font;
	ofxOscReceiver receiver;

	int currentMsgString;
	string msgStrings[NUM_MSG_STRINGS];
	float timers[NUM_MSG_STRINGS];
};
