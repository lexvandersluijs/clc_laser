#include "ShowElement.h"

#include "ofxLaserUI.h"
#include "ofxClipper.h"



ShowElement::ShowElement(string n) 
{ 
	name = n;

	loadParameterJson();
}

ShowElement::~ShowElement()
{
	saveParameters();
}

void ShowElement::loadParameterJson()
{
	string filename = "showElements/" + name + ".json";
	if (ofFile(filename).exists()) {
		parameterJson = ofLoadJson(filename);
	}
	// if the json didn't load then this shouldn't do anything
	ofDeserialize(parameterJson, parameters);

}

void ShowElement::addParameter(ofAbstractParameter& param)
{
	parameters.add(param);

	if (!parameterJson.empty()) {
		if (parameterJson.contains("group")) 
		{
			try {
				ofDeserialize(parameterJson["group"], param);
			}
			catch (...) 
			{
				cout << "Problem deserializing parameter for ShowElement: '" + name + "' param: " + param.getName();
			}
		}
	}
}

void ShowElement::saveParameters()
{
	ofJson json;
	ofSerialize(json, parameters);

	bool savesuccess = ofSavePrettyJson("showElements/" + name + ".json", json);
}

// -------------------------- SvgShowElement --------------------------
SvgShowElement::SvgShowElement(string name) : ShowElement(name)
{

}

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


	addParameter(currentSVG.set("Current SVG", 0, 0, laserGraphics.size() - 1));
	addParameter(currentSVGFilename.set("Filename", currentSVGFilename.get()));

	addParameter(drawTwoSided.set("Draw two-sided", false));
	addParameter(otherSideSVG.set("Other side SVG", 0, 0, laserGraphics.size() - 1));
	addParameter(otherSideSVGFilename.set("Other side filename", otherSideSVGFilename.get()));

	addParameter(scale.set("SVG scale", 1.0, 0.1, 6));
	addParameter(offsetX.set("Offset X", 400, 0, 800));
	addParameter(offsetY.set("Offset Y", 400, 0, 800));
	addParameter(rotate3D.set("Rotate 3D", true));
	addParameter(rotate360.set("Rotate 360", false));
	addParameter(renderProfileLabel.set("Render Profile name", ""));
	addParameter(renderProfileIndex.set("Render Profile", 1, 0, 2));

}


void SvgShowElement::update()
{

}
void SvgShowElement::draw()
{
	float x = 0;
	float spacing = 8;
	UI::startWindow("Settings - " + name, ImVec2(x, 804), ImVec2(ofGetWindowWidth()/2, 0));

	ofxLaser::UI::addParameterGroup(parameters);

	UI::endWindow();
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

	ofTranslate(offsetX, offsetY);
	ofScale(scale, scale);

	float modulo = rotate360 ? 360 : 180;
	float angle = fmod(ofGetElapsedTimef() * 30, modulo) - 90;

	int indexToDraw = currentSVG;
	if (drawTwoSided.get())
	{
		if (angle > 90 && angle < 270)
		{
			indexToDraw = otherSideSVG;
			angle += 180;
		}
	}

	if (rotate3D) {
		ofRotateYDeg(angle);
	}

	if (laserGraphics.size() > currentSVG) {
		laserManager.drawLaserGraphic(laserGraphics[indexToDraw], 1, renderProfileName);
	}
	ofPopMatrix();

	laserManager.endDraw();

}

// ----------------------- SvgAnimationShowElement -------------------------
SvgAnimationShowElement::SvgAnimationShowElement(string name) : ShowElement(name)
{

}

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
	//UI::startWindow("Settings", ImVec2(x, spacing), ImVec2(ofGetWindowWidth(), 0), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize, true);

	//UI::endWindow();

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





