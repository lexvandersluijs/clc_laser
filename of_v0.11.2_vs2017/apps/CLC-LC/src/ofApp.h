#pragma once

const int OF_KEY_SPACE = 32;


#include "ofMain.h"
#include "ofxLaserManager.h"

#include "ShowElement.h"

#include <list>

class ofApp : public ofBaseApp
{

public:
	~ofApp();

    void setup();
    void update();
    void draw();

    

    void keyPressed(int key);
    
protected:
	int selectedShowElementIndex = 0;

	std::vector<ShowElement*> showElements;
	std::vector<ofParameter<bool>*> selectedElementBooleans;

    ofxLaser::Manager laserManager;
    
	void selectedShowElementChanged(const void* sender, bool & newValue);
};
