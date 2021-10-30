#pragma once

#include "ofMain.h"
#include "ofxLaserManager.h"
#include "SVGLoader.h"

#include "ofxOnsetDetection.h"
#include "ofxTimeline.h"
#include "ofxTLAudioTrack.h"
#include "ofxTLVideoTrack.h"

#include "ParticleSystem/ParticleGenerator.h"
#include "ParticleSystem/ParticleRenderer.h"
#include "ParticleSystem/SimulationAnimator.h"

#include "GraphicElement.h"

using namespace ofxLaser;
using namespace ParticleSystem;

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

	virtual void setActive(bool a) { active = a; }
	bool getActive() { return active; }

	virtual void SpaceBarPressed() {}

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
	ofParameter<float> offsetX;
	ofParameter<float> offsetY;
	ofParameter<bool> rotate3D;
	ofParameter<bool> rotate360;
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




