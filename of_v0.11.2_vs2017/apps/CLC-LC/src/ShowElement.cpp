#include "ShowElement.h"

#include "ofxLaserUI.h"

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
	addParameter(scale.set("SVG scale", 1.0, 0.1, 6));
	addParameter(offsetX.set("Offset X", 400, 0, 800));
	addParameter(offsetY.set("Offset Y", 400, 0, 800));
	addParameter(rotate3D.set("Rotate 3D", true));
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

// -------------------------- TimelineShowElement --------------------------
TimelineShowElement::TimelineShowElement(string name) : ShowElement(name)
{

}

void TimelineShowElement::setup()
{
	// ---------------- set up the timeline ----------------
	ofxTimeline::removeCocoaMenusFromGlut("AllTracksExample");
	timeline.setup();
	timeline.setOffset(ofVec2f(0, 800));

	timeline.addAudioTrack("audio", "sound/AUD-20211025-WA0003-vnc.mp3");
	timeline.setDurationInSeconds(timeline.getAudioTrack("audio")->getDuration());
	timeline.setLoopType(ofLoopType::OF_LOOP_NONE);

	timeline.addCurves("curves", ofRange(0, 255));
	timeline.addBangs("bangs");
	timeline.enableSnapToOtherKeyframes(false);


	// we don't want the timeline to start when it is not on the active showElement
	// do we disable this here and explicity toggle this via the SpaceBarPressed() function
	timeline.setSpacebarTogglePlay(false);

	ofAddListener(timeline.events().bangFired, this, &TimelineShowElement::bangFired);
}


void TimelineShowElement::SpaceBarPressed()
{
	timeline.togglePlay();
}

void TimelineShowElement::setActive(bool a) 
{
	ShowElement::setActive(a);

	if (a == false)
	{
		if (timeline.getIsPlaying())
			timeline.stop();
	}
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
RealtimeShowElement::RealtimeShowElement(string name) : ShowElement(name)
{
}

void RealtimeShowElement::setup()
{
	// open audio input
		// 0 output channels,
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)
	soundStream.printDeviceList();

	int BUFFER_SIZE = 256;

	//if you want to set a different device id
	//soundStream.setDeviceID(4); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
	//bool result = soundStream.setup(app, 2, 1, 44100, BUFFER_SIZE, 4);

	int bufferSize = 256;

	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	volHistory.assign(400, 0.0);

	bufferCounter = 0;
	drawCounter = 0;
	smoothedVol = 0.0;
	scaledVol = 0.0;


	ofSoundStreamSettings settings;

	// if you want to set the device id to be different than the default
	// auto devices = soundStream.getDeviceList();
	// settings.device = devices[4];

	// you can also get devices for an specific api
	//auto devices = soundStream.getDeviceList(ofSoundDevice::Api::MS_ASIO);
	//settings.setInDevice(devices[0]);

	auto devices = soundStream.getDeviceList(ofSoundDevice::Api::MS_DS);
	settings.setInDevice(devices[2]);

	// or get the default device for an specific api:
	// settings.api = ofSoundDevice::Api::PULSE;

	// or by name
	//auto devices = soundStream.getMatchingDevices("default");
	//if (!devices.empty()) {
	//	settings.setInDevice(devices[0]);
	//}
	settings.numBuffers = 1;
	settings.setInListener(this);
	settings.sampleRate = 44100;
	settings.numOutputChannels = 0;
	settings.numInputChannels = 2;
	settings.bufferSize = bufferSize;
	bool result = soundStream.setup(settings);

	if (result)
		cout << "soundStream.setup() successful" << endl;
	else
		cout << "soundStream.setup() error" << endl;
}

void RealtimeShowElement::audioIn(ofSoundBuffer & input)
{
	curVol = 0.0;

	// samples are "interleaved"
	int numCounted = 0;

	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
	size_t maxSample = input.getNumFrames();
	if (maxSample != left.size())
	{
		left.resize(maxSample);
		right.resize(maxSample);
	}

	for (size_t i = 0; i < maxSample; i++) {
		left[i] = input[i * 2] * 0.5;
		right[i] = input[i * 2 + 1] * 0.5;

		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted += 2;
	}

	//this is how we get the mean of rms :) 
	curVol /= (float)numCounted;

	// this is how we get the root of rms :) 
	curVol = sqrt(curVol);

	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;

	bufferCounter++;
}

void RealtimeShowElement::update()
{
	// compute FFT
}
void RealtimeShowElement::draw()
{
}

void RealtimeShowElement::setActive(bool a)
{
	ShowElement::setActive(a);

	if (a == false)
		soundStream.stop();
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
	float scale = 100 + 10000 * curVol;
	float offsetX = 400;
	float offsetY = 400;
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

// -------------------------- RealtimeParticlesShowElement ---------------------

void RealtimeParticlesShowElement::setup()
{
	int w = 800;
	int h = 800;
	_particleManager = new ParticleManager(50000);
	_particleGenerator = new ParticleGenerator(_particleManager);
	_particleGenerator->SetGenerationRate(1000);
	_particleGenerator->SetParticleLifeTime(3.f);
	//SimulationAnimator* simAnim = new SimulationAnimator(_particleManager);
	//simAnim->AddForceField(new VortexForceField(ofVec2f(200.f, 200.f), 30.f));
	//_particleAnimator = simAnim;
	SimulationAnimator* fluidSimAnim = new SimulationAnimator(_particleManager);
	_particleAnimator = fluidSimAnim;
	_particleRenderer = new ParticleRenderer(_particleManager);

	width = w;
	height = h;

}

void RealtimeParticlesShowElement::update()
{
	// TODO: if prevTime == 0 then don't actually update -> timestep too large, but do set prevTime for next time
	float currentTime = ofGetElapsedTimef();
	float timeStep = currentTime - prevTime;
	prevTime = currentTime;

	if (timeStep < 1.0f) // ignore timesteps that are too large
	{
		_particleManager->Update(timeStep);
		_particleAnimator->Update(currentTime, timeStep); // compute new properties (position, color, age, etc) all particles
	}
}

void RealtimeParticlesShowElement::draw()
{
	// draw the glows of the lines
	//glow.draw_dontEnableAlphaBlending(x, y, width, height);

	// draw the lines themselves
	_particleRenderer->Draw(ofVec2f(400, 400), 0.5f);
}

// --------------------------- RealtimeCirclesShowElement ------------------
RealtimeCirclesShowElement::RealtimeCirclesShowElement(string name) : ShowElement(name)
{

}

void RealtimeCirclesShowElement::setup()
{
	onsetD.setup(OFX_ODS_ODF_RCOMPLEX);
	snd.loadSound("sound/NextStep_CLCEdit_Richard.mp3");
	snd.setLoop(false);
}

void RealtimeCirclesShowElement::setActive(bool a)
{
	ShowElement::setActive(a);

	if(a == false)
		if (playing == true)
		{
			stop();
		}
}

void RealtimeCirclesShowElement::SpaceBarPressed()
{
	if (playing == false)
	{
		start();
	}
	else
	{
		stop();
	}
}

void RealtimeCirclesShowElement::start()
{
	snd.play();
	playing = true;
}

void RealtimeCirclesShowElement::stop()
{
	snd.stop();
	playing = false;
}

void RealtimeCirclesShowElement::update()
{
	// compute FFT
	if (playing)
	{
		if (snd.getIsPlaying())
		{
			float* spectrum = ofSoundGetSpectrum(nBands);

			if (onsetD.isOnsetting(spectrum))
			{
				//musicShader->pulse(0);
				cout << "Onset detected" << endl;
			}
		}
	}
}

void RealtimeCirclesShowElement::draw()
{
}

void RealtimeCirclesShowElement::UpdateGraphic()
{
	ellipseGraphic.clear();

	ofPolyline ellipse;
	int NUM_STEPS = 100;
	float a = 1;
	float b = 1;
	float n = 4;
	float scale = 200;
	//float offsetX = 200;
	//float offsetY = 200;
	for (int i = 0; i < NUM_STEPS; i++)
	{
		float t = i / (float)(NUM_STEPS - 1);
		float tp = t * M_TWO_PI;
		float cost = cos(tp);
		float sint = sin(tp);
		float x = scale * cost;
		float y = scale * sint;

		ellipse.addVertex(x, y, 0);
	}
	ellipse.close();
	//graphic2.connectLineSegments();

	ellipseGraphic.addPolyline(ellipse, ofColor(255.0f, 255.0f, 0.0f, 255.0f), false, true);
}

void RealtimeCirclesShowElement::drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName)
{
	laserManager.beginDraw();

	for (int i = 0; i < 3; i++)
	{
		ofPushMatrix();

		float scale = 1;
		switch (i)
		{
		case 0:
			break;
		case 1:
			scale = 1.2f;
			break;
		case 2:
			scale = 0.8f;
			break;
		}

		// polylines get projected inside the drawLaserGraphic function, so we need a new set each time!
		UpdateGraphic();

		ofTranslate(400, 400);
		ofScale(scale, scale);

		float angle = fmod(ofGetElapsedTimef() * 30, 180) - 90;
		ofRotateYDeg(angle);


		laserManager.drawLaserGraphic(ellipseGraphic, 1, renderProfileName);

		ofPopMatrix();

	}
	laserManager.endDraw();
}
