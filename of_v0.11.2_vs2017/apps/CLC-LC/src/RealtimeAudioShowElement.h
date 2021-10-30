#pragma once

#include "ShowElement.h"

class RealtimeAudioShowElement : public ShowElement, ofBaseSoundInput
{
public:
	//RealtimeShowElement(ofBaseApp* theApp, string name);
	RealtimeAudioShowElement(string name, string inputDeviceName);

	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName);

	virtual void audioIn(ofSoundBuffer & input);

	virtual void setActive(bool a);

protected:
	//ofBaseApp* app;
	string inputDeviceName;
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
class RealtimeParticlesShowElement : public RealtimeAudioShowElement
{
public:
	RealtimeParticlesShowElement(string name, string inputdevicename) : RealtimeAudioShowElement(name, inputdevicename) {}
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
	RealtimeCirclesShowElement(string name, string inputdevicename);

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



