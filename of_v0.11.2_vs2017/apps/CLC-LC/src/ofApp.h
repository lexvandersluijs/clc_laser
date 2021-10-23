#pragma once

#include "ofMain.h"
#include "ofxLaserManager.h"

#include "ShowElement.h"

class ofApp : public ofBaseApp
{

public:
    void setup();
    void update();
    void draw();

    

    void keyPressed(int key);
    
    
protected:
	int selectedShowElementIndex = 0;

	SvgShowElement svgShowElement;
	TimelineShowElement timelineShowElement;
	RealtimeShowElement realtimeShowElement;

    ofxLaser::Manager laserManager;
    

};
