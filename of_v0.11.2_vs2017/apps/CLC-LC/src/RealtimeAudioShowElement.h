#pragma once

#include "ShowElement.h"
#include "Audio.h"

class RealtimeAudioShowElement : public ShowElement, ofBaseSoundInput
{
public:
	//RealtimeShowElement(ofBaseApp* theApp, string name);
	RealtimeAudioShowElement(string name, string inputDeviceName, GraphicSet* graphicsset);
	virtual ~RealtimeAudioShowElement();

	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName);

	virtual void audioIn(ofSoundBuffer & input);

	virtual void setActive(bool a);
	virtual void SpaceBarPressed();

protected:
	//ofBaseApp* app;
	string inputDeviceNameFromConstructor;
	ofParameter<string> inputDeviceName;

	GraphicSet* graphicSet;
	ofxLaser::Graphic graphic;
	ofSoundStream soundStream;

	bool audioReceived = false;
	Audio audioProcessor;

	bool isPlaying = false;
};





// maybe RealtimeShowElement is 'LiveSoundReactiveShowElement' ?
class RealtimeParticlesShowElement : public RealtimeAudioShowElement
{
public:
	RealtimeParticlesShowElement(string name, string inputdevicename, GraphicSet* graphicsset) : RealtimeAudioShowElement(name, inputdevicename, graphicsset) {}
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

class RealtimeCirclesShowElement : public RealtimeAudioShowElement
{
public:
	RealtimeCirclesShowElement(string name, string inputdevicename, GraphicSet* graphicsset);

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



