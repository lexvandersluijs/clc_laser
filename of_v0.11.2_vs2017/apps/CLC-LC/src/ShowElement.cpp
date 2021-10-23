#include "ShowElement.h"

// -------------------------- SvgShowElement --------------------------
void SvgShowElement::setup()
{

	// the first time it runs, the SVGLoader will load the SVGs
	// and save each one as an optimised .ofxlg file
	// (in an "/optimised" subfolder). The next time it runs
	// it will load these instead (and is MUCH faster). If you
	// don't want this behaviour, use :
	//svgLoader.setLoadOptimisation(false);

	svgLoader.startLoad("svgs/motorbikes/");
}

void SvgShowElement::update()
{

}
void SvgShowElement::draw()
{
	if (!svgLoader.hasFinishedLoading()) {
		ofDrawRectangle(0, 0, ofMap(svgLoader.getLoadedPercent(), 0, 100, 0, ofGetWidth()), 8);
	}
}

ofxLaser::Graphic& SvgShowElement::getLaserGraphic()
{
	int frameNumToShow = ofGetElapsedTimef() * 30; // 30 frames per second
	frameNumToShow = frameNumToShow % svgLoader.getTotalFileCount();
	ofxLaser::Graphic& graphic = svgLoader.getLaserGraphic(frameNumToShow);

	return graphic;
}
// -------------------------- TimelineShowElement --------------------------

void TimelineShowElement::setup()
{
	// ---------------- set up the timeline ----------------
	ofxTimeline::removeCocoaMenusFromGlut("AllTracksExample");
	timeline.setup();
	timeline.setOffset(ofVec2f(0, ofGetWindowHeight() * 0.75f));

	timeline.addCurves("curves", ofRange(0, 255));
	timeline.addBangs("bangs");
	timeline.enableSnapToOtherKeyframes(false);
	timeline.setLoopType(OF_LOOP_NORMAL);

	ofAddListener(timeline.events().bangFired, this, &TimelineShowElement::bangFired);
}

void TimelineShowElement::update()
{

}

void TimelineShowElement::draw()
{
	// --------- draw the timeline --------------
	timeline.draw(true);//false removed tickers, true(default) adds them
}

void TimelineShowElement::bangFired(ofxTLBangEventArgs& args) 
{
	cout << "bang fired!" << args.flag << endl;
}

ofxLaser::Graphic& TimelineShowElement::getLaserGraphic()
{
	return graphic;
}

// --------------------------- RealtimeShowElement ------------------
void RealtimeShowElement::setup()
{
	// open audio input
}

void RealtimeShowElement::update()
{
	// compute FFT
}
void RealtimeShowElement::draw()
{
}


float sgn(float value)
{
	if (value < 0)
		return -1;
	else if (value == 0)
		return 0;
	else
		return 1;
}

ofxLaser::Graphic& RealtimeShowElement::getLaserGraphic()
{
	graphic.clear();

	ofPolyline superEllipse;
	int NUM_STEPS = 100;
	float a = 1;
	float b = 1;
	float n = 4;
	float scale = 100;
	float offsetX = 200;
	float offsetY = 200;
	for (int i = 0; i < NUM_STEPS; i++)
	{
		float t = i / (float)(NUM_STEPS - 1);
		float tp = t * M_TWO_PI;
		float cost = cos(tp);
		float sint = sin(tp);
		float x = offsetX + scale * pow(abs(cost), 2 / n) * a * sgn(cost);
		float y = offsetY + scale * pow(abs(sint), 2 / n) * b * sgn(sint);

		superEllipse.addVertex(x, y, 0);
	}
	superEllipse.close();
	//graphic2.connectLineSegments();

	graphic.addPolyline(superEllipse, ofColor(255.0f, 255.0f, 0.0f, 255.0f), false, true);

	return graphic;
}
