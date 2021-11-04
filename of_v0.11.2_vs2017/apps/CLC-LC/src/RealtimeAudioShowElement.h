#pragma once

#include "ShowElement.h"
#include "Audio.h"
#include "BeatDetect.h"

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

	void StartAudioInput();
	void StopAudioInput();

	virtual void saveParameters();

protected:
	//ofBaseApp* app;
	string inputDeviceNameFromConstructor;
	ofParameter<string> inputDeviceName;

	vector<string> inputDeviceNamesAvailable;

	ofParameter<bool> startPlayingOnActivation;
	ofParameter<bool> reactToAudio;
	ofParameter<float> velocityFactor;
	ofParameter<float> pulseResponseInPixels;
	ofParameter<float> scaleX;
	ofParameter<float> scaleY;
	ofParameter<float> rotationVelocityFactor;

	GraphicSet* graphicSet;
	ofxLaser::Graphic graphic;
	ofSoundStream soundStream;

	bool audioReceived = false;
	//Audio audioProcessor;
	int fft_size = 512;
	beatDetect bd;
	ofxOnsetDetection onsetD;

	bool isPlaying = false;
};



