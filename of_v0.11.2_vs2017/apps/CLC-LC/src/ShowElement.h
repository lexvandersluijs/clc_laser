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
	ShowElement(string n);
	virtual ~ShowElement();

	string getName() { return name; }

	virtual void setup() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName) = 0;

	void loadParameterJson();
	void addParameter(ofAbstractParameter& param);
	void saveParameters();

	bool selected = false;

	void setActive(bool a) { active = a; }
	bool getActive() { return active; }

protected:
	string name;
	bool active = false;

	ofJson parameterJson;
	ofParameterGroup parameters;
};

class SvgShowElement : public ShowElement
{
public:
	SvgShowElement(string name);

	virtual void setup();
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

class SvgAnimationShowElement : public ShowElement
{
public:
	SvgAnimationShowElement(string name);

	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName);

protected:
	int svgType = 0; // 0 is static, 1 is animation
	SVGLoader svgLoader;
};


class RealtimeShowElement : public ShowElement
{
public:
	RealtimeShowElement(string name);

	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName);

protected:
	ofxLaser::Graphic graphic;
};

class TimelineShowElement : public ShowElement
{
public:
	TimelineShowElement(string name);

	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName);

protected:
	ofxTimeline timeline;
	ofxLaser::Graphic graphic;

	void bangFired(ofxTLBangEventArgs& args);

};