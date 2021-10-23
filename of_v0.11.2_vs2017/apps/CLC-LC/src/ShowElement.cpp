#include "ShowElement.h"

#include "ofxLaserUI.h"


// -------------------------- SvgShowElement --------------------------
void SvgShowElement::setup()
{
	// get the filenames of all the svgs in the data/svgs folder
	string path = "svgs/";
	ofDirectory dir(path);
	dir.allowExt("svg");
	dir.listDir();
	dir.sort();

	// and load them all
	const vector<ofFile>& files = dir.getFiles();

	dir.close();

	//svgs.resize(files.size());

	for (int i = 0; i < files.size(); i++) {
		const ofFile & file = files.at(i);

		laserGraphics.emplace_back();

		// addSvgFromFile(string filename, bool optimise, bool subtractFills)
		laserGraphics.back().addSvgFromFile(file.getAbsolutePath(), false, true);

		// this centres all the graphics
		laserGraphics.back().autoCentre();

		ofLog(OF_LOG_NOTICE, file.getAbsolutePath());
		fileNames.push_back(file.getFileName());
	}

	currentSVG = 0;

}

void SvgShowElement::setup2(ofxLaser::Manager& laserManager)
{
	//	TODO: we want to save these parameters in our own JSON, and present them in our own GUI

	laserManager.addCustomParameter(currentSVG.set("Current SVG", 0, 0, laserGraphics.size() - 1));
	laserManager.addCustomParameter(currentSVGFilename.set("Filename"));
	laserManager.addCustomParameter(scale.set("SVG scale", 1.0, 0.1, 6));
	laserManager.addCustomParameter(rotate3D.set("Rotate 3D", true));
	laserManager.addCustomParameter(renderProfileLabel.set("Render Profile name", ""));
	laserManager.addCustomParameter(renderProfileIndex.set("Render Profile", 1, 0, 2));

	ofParameter<string> description;
	description.set("description", "INSTRUCTIONS : \nLeft and Right Arrows to change current SVG \nTAB to toggle output editor \nF to toggle full screen");
	laserManager.addCustomParameter(description);
}

void SvgShowElement::update()
{

}
void SvgShowElement::draw()
{
	float x = 0;
	float spacing = 8;
	//UI::startWindow("Settings", ImVec2(x, spacing), ImVec2(ofGetWindowWidth(), 0), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize, true);

	//UI::endWindow();
}

void SvgShowElement::drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName)
{
	string renderProfile;
	switch (renderProfileIndex) {
	case 0:
		renderProfile = OFXLASER_PROFILE_DEFAULT;
		break;
	case 1:
		renderProfile = OFXLASER_PROFILE_DETAIL;
		break;
	case 2:
		renderProfile = OFXLASER_PROFILE_FAST;
		break;
	}
	renderProfileLabel = "Render Profile : OFXLASER_PROFILE_" + renderProfile;

	ofxLaser::Graphic& laserGraphic = laserGraphics[currentSVG];

	currentSVGFilename = fileNames[currentSVG];

	laserManager.beginDraw();

	ofPushMatrix();

	ofTranslate(400, 400);
	ofScale(scale, scale);
	if (rotate3D) {
		float angle = fmod(ofGetElapsedTimef() * 30, 180) - 90;
		ofRotateYDeg(angle);
	}
	if (laserGraphics.size() > currentSVG) {
		laserManager.drawLaserGraphic(laserGraphics[currentSVG], 1, renderProfileName);
	}
	ofPopMatrix();

	laserManager.endDraw();

}

// ----------------------- SvgAnimationShowElement -------------------------
void SvgAnimationShowElement::setup()
{
	// the first time it runs, the SVGLoader will load the SVGs
	// and save each one as an optimised .ofxlg file
	// (in an "/optimised" subfolder). The next time it runs
	// it will load these instead (and is MUCH faster). If you
	// don't want this behaviour, use :
	//svgLoader.setLoadOptimisation(false);

	svgLoader.startLoad("svgs/motorbikes/");
}

void SvgAnimationShowElement::update()
{

}
void SvgAnimationShowElement::draw()
{
	if (!svgLoader.hasFinishedLoading()) {
		ofDrawRectangle(0, 0, ofMap(svgLoader.getLoadedPercent(), 0, 100, 0, ofGetWidth()), 8);
	}

	float x = 0;
	float spacing = 8;
	UI::startWindow("Settings", ImVec2(x, spacing), ImVec2(ofGetWindowWidth(), 0), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize, true);

	UI::endWindow();

}

void SvgAnimationShowElement::drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName)
{
	laserManager.beginDraw();

	int frameNumToShow = ofGetElapsedTimef() * 30; // 30 frames per second
	frameNumToShow = frameNumToShow % svgLoader.getTotalFileCount();
	ofxLaser::Graphic& graphic = svgLoader.getLaserGraphic(frameNumToShow);

	// animation overrrides render profile name..
	laserManager.drawLaserGraphic(graphic, 1, OFXLASER_PROFILE_FAST);

	laserManager.endDraw();
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

void TimelineShowElement::drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName)
{
	laserManager.beginDraw();

	laserManager.drawLaserGraphic(graphic, 1, renderProfileName);

	laserManager.endDraw();
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

void RealtimeShowElement::drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName)
{
	laserManager.beginDraw();

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

	laserManager.drawLaserGraphic(graphic, 1, renderProfileName);


	laserManager.endDraw();
}
