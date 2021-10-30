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


class RealtimeShowElement : public ShowElement, ofBaseSoundInput
{
public:
	//RealtimeShowElement(ofBaseApp* theApp, string name);
	RealtimeShowElement(string name);

	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName);

	virtual void audioIn(ofSoundBuffer & input);

	virtual void setActive(bool a);

protected:
	//ofBaseApp* app;
	ofxLaser::Graphic graphic;
	ofSoundStream soundStream;

	vector <float> left;
	vector <float> right;
	vector <float> volHistory;

	int 	bufferCounter;
	int 	drawCounter;

	float curVol = 0.0;
	float smoothedVol;
	float scaledVol;
};

// maybe RealtimeShowElement is 'LiveSoundReactiveShowElement' ?
class RealtimeParticlesShowElement : public RealtimeShowElement
{
public:
	RealtimeParticlesShowElement(string name) : RealtimeShowElement(name) {}
	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName);

protected:
	int     width, height;
	float	prevTime = 0;

	ParticleManager* _particleManager;
	ParticleGenerator* _particleGenerator;
	ParticleAnimator* _particleAnimator;
	ParticleRenderer* _particleRenderer;
};

class RealtimeCirclesShowElement : public ShowElement
{
public:
	RealtimeCirclesShowElement(string name);

	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName);

	virtual void SpaceBarPressed();

	virtual void setActive(bool a);

	void start();
	void stop();

protected:
	void UpdateGraphic(ofColor color);

	ofxLaser::Graphic ellipseGraphic;
	//ofxLaser::Graphic circle2;
	//ofxLaser::Graphic circle3;

	// --- onset detection ---
	bool playing = false;
	int nBands = 1024;
	ofSoundPlayer snd;
	ofxOnsetDetection onsetD;

	bool pulsing = false;
};

class GraphicElement
{
public:
	virtual ~GraphicElement() {}
	virtual void pulse();
	virtual void update();
	virtual ofPolyline& getPolyline();
	ofColor getColor();

protected:
	float pulseTime;
	float pulseFactor;

	ofPolyline polyline;
	ofColor color;
};

class VerticalLine : public GraphicElement
{
public:
	VerticalLine(float x, ofColor c);
	virtual ofPolyline& getPolyline();

protected:
	float xCoord;
};

class SuperEllipse : public GraphicElement
{
public:
	SuperEllipse(ofVec2f center, ofColor c);
	virtual ofPolyline& getPolyline();

protected:
	ofVec2f center;
};

class TimelineShowElement : public ShowElement
{
public:
	TimelineShowElement(string name);
	~TimelineShowElement();

	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName);

	virtual void SpaceBarPressed();

	virtual void setActive(bool a);

protected:
	ofxTimeline timeline;
	ofxLaser::Graphic graphic;
	ofxOnsetDetection onsetD;

	ofParameter<float> Mask1L;
	ofParameter<float> Mask1R;
	ofParameter<float> Mask2L;
	ofParameter<float> Mask2R;

	void bangFired(ofxTLBangEventArgs& args);

	vector<GraphicElement*> lines;
};