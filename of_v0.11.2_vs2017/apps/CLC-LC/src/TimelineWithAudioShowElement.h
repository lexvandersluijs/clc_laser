#pragma once

#include "ShowElement.h"
#include "Audio.h"

class TimelineWithAudioShowElement : public ShowElement
{
public:
	TimelineWithAudioShowElement(string name, string audiofilePath, GraphicSet* graphicset);
	~TimelineWithAudioShowElement();

	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual void drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName);

	virtual void SpaceBarPressed();

	virtual void setActive(bool a);

protected:
	ofxTimeline timeline;
	string audioFilePath;
	ofxLaser::Graphic graphic;
	ofxOnsetDetection onsetD;

	void bangFired(ofxTLBangEventArgs& args);

	//vector<GraphicElement*> lines;
	GraphicSet* graphicSet;
};

