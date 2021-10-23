#pragma once

#include "ofMain.h"
#include "ofxLaserManager.h"
#include "SVGLoader.h"

#include "ofxTimeline.h"
#include "ofxTLAudioTrack.h"
#include "ofxTLVideoTrack.h"


class ShowElement
{
public:
	virtual void setup() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual ofxLaser::Graphic& getLaserGraphic() = 0;
};

class SvgShowElement
{
public:
	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual ofxLaser::Graphic& getLaserGraphic();

protected:
	SVGLoader svgLoader;
};

class RealtimeShowElement
{
public:
	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual ofxLaser::Graphic& getLaserGraphic();

protected:
	ofxLaser::Graphic graphic;
};

class TimelineShowElement
{
public:
	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual ofxLaser::Graphic& getLaserGraphic();

protected:
	ofxTimeline timeline;
	ofxLaser::Graphic graphic;

	void bangFired(ofxTLBangEventArgs& args);

};