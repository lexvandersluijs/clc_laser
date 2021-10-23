#pragma once

#include "ofMain.h"
#include "ofxLaserManager.h"
#include "SVGLoader.h"

#include "ofxTimeline.h"
#include "ofxTLAudioTrack.h"
#include "ofxTLVideoTrack.h"

using namespace ofxLaser;

class ShowElement
{
public:
	virtual void setup() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName) = 0;
};

class SvgShowElement
{
public:
	virtual void setup();
	void setup2(ofxLaser::Manager& laserManager);
	virtual void update();
	virtual void draw();
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName);

protected:

	ofParameter<int> currentSVG;
	ofParameter<string> currentSVGFilename;
	ofParameter<float> scale;
	ofParameter<bool> rotate3D;
	ofParameter<int> renderProfileIndex;
	ofParameter<string> renderProfileLabel;

	vector<ofxLaser::Graphic> laserGraphics;
	vector<string> fileNames;
};

class SvgAnimationShowElement
{
public:
	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName);

protected:
	int svgType = 0; // 0 is static, 1 is animation
	SVGLoader svgLoader;
};


class RealtimeShowElement
{
public:
	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName);

protected:
	ofxLaser::Graphic graphic;
};

class TimelineShowElement
{
public:
	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName);

protected:
	ofxTimeline timeline;
	ofxLaser::Graphic graphic;

	void bangFired(ofxTLBangEventArgs& args);

};