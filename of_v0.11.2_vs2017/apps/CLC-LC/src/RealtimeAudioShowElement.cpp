#include "RealtimeAudioShowElement.h"

// --------------------------- RealtimeAudioShowElement ------------------
RealtimeAudioShowElement::RealtimeAudioShowElement(string name, string inputdevicename, GraphicSet* graphicset) : ShowElement(name)
{
	inputDeviceNameFromConstructor = inputdevicename;
	graphicSet = graphicset;

	onsetD.setup(OFX_ODS_ODF_RCOMPLEX, 2, 256);
}

RealtimeAudioShowElement::~RealtimeAudioShowElement()
{
	if(graphicSet != NULL)
		delete graphicSet;
}

void PrintDirectShowAudioDeviceList(const ofBaseSoundStream& bss)
{
	auto devices = bss.getDeviceList(ofSoundDevice::MS_DS);
	if (!devices.empty()) {
		ofLogNotice("ofBaseSoundStream::printDeviceList") << "Api: " << toString(ofSoundDevice::MS_DS);
		ofLogNotice("ofBaseSoundStream::printDeviceList") << devices;
	}
}

void RealtimeAudioShowElement::setup()
{
	// open audio input
	// 0 output channels,
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)
	//soundStream.printDeviceList();
	PrintDirectShowAudioDeviceList(*soundStream.getSoundStream());
	
	addParameter(inputDeviceName.set("Input device name", inputDeviceNameFromConstructor));

	bd.enableBeatDetect();
}

void RealtimeAudioShowElement::audioIn(ofSoundBuffer & input)
{
	bd.audioReceived(&(input.getBuffer()[0]), input.getBuffer().size());

	//vector<float> buffer = input.getBuffer();

	// amplify the microphone input..
	//for (int i = 0; i < buffer.size(); i++)
	//	buffer[i] *= 5;

	// divide by two, since this function will split left and right, so the actual number of stereo samples is 256 if input size is 512
	//audioProcessor.update(&buffer[0], buffer.size() / 2); 

	audioReceived = true;
}

void RealtimeAudioShowElement::update()
{
	if (audioReceived)
	{
		// compute FFT and such
		//audioProcessor.calc();
		bd.updateFFT();

		bool isOnsetting = onsetD.isOnsetting(bd.getMagnitude());
		if(isOnsetting)
			cout << "isOnSetting = " << isOnsetting << endl;

		if (bd.isKick())
		{
			cout << "RealtimeAudioShowElement bd.isKick() bd.magnitude[10] = " << bd.magnitude[10] << endl;
			//graphicSet->pulse(0);
			//float newRadius = 100 * (1.0f + 100 * audioProcessor.getAvgPower());
			//float newRadius = 100 * (1.0f + bd.magnitude[10]);
			//(dynamic_cast<SuperEllipseSet*>(graphicSet))->setParams(4, newRadius, newRadius);
			graphicSet->pulse(0);
		}
		if (bd.isSnare())
		{
			graphicSet->pulse(1);
		}
		if (bd.isHat())
		{
			graphicSet->pulse(2);
		}
		graphicSet->update();
	}
}

void RealtimeAudioShowElement::draw()
{
	float x = 0;
	float spacing = 8;
	UI::startWindow("Settings - " + name, ImVec2(x, 804), ImVec2(ofGetWindowWidth() / 2, 0));

	ofxLaser::UI::addParameterGroup(parameters);

	UI::endWindow();

	int testApp_fft_size = 512;

	ofSetColor(155, 155, 75);
	for (int i = 1; i < (int)testApp_fft_size / 2; i++) 
	{
		if (i % 16 == 0) {
			ofSetColor(200, 0, 0);
		}
		else {
			ofSetColor(155, 155, 75);
		}
		ofLine(10 + (i * 3), 150, 10 + (i * 3), 150 - bd.magnitude[i] * 10.0f);
		//printf("%f \n", magnitude_average[i]);
	}
}

void RealtimeAudioShowElement::setActive(bool a)
{
	ShowElement::setActive(a);
}

void RealtimeAudioShowElement::SpaceBarPressed()
{
	isPlaying = !isPlaying;

	if (isPlaying)
	{
		//if you want to set a different device id
		//soundStream.setDeviceID(4); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
		//bool result = soundStream.setup(app, 2, 1, 44100, BUFFER_SIZE, 4);

		int bufferSize = 512;


		bool useNewDeviceSelection = true;

		if (useNewDeviceSelection)
		{
 			std::vector<ofSoundDevice> matchingDevices = soundStream.getMatchingDevices(inputDeviceName, 2, 0, ofSoundDevice::Api::MS_DS);

			ofSoundStreamSettings settings;
			settings.setInDevice(matchingDevices[0]);
			settings.numBuffers = 1;
			settings.setInListener(this);
			settings.sampleRate = 44100;
			settings.numOutputChannels = 0;
			settings.numInputChannels = 2;
			settings.bufferSize = bufferSize;
			bool result = soundStream.setup(settings);
		}
		else
		{
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
	}
	else
	{
		audioReceived = false;
		soundStream.stop();
	}
}

void RealtimeAudioShowElement::drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName)
{
	laserManager.beginDraw();

	graphic.clear();

	graphicSet->drawToGraphic(graphic);

	laserManager.drawLaserGraphic(graphic, 1, renderProfileName);

	laserManager.endDraw();
}


// -------------------------- RealtimeParticlesShowElement ---------------------

void RealtimeParticlesShowElement::setup()
{
	RealtimeAudioShowElement::setup();

	int w = 800;
	int h = 800;
	_particleManager = new ParticleManager(20);
	_particleGenerator = new ParticleGenerator(_particleManager);
	_particleGenerator->SetGenerationRate(1000);
	_particleGenerator->SetParticleLifeTime(1.f);
	SimulationAnimator* simAnim = new SimulationAnimator(_particleManager);
	simAnim->AddForceField(new PointForceField(ofVec2f(400.f, 400.f), 5.f));
	//simAnim->AddForceField(new VortexForceField(ofVec2f(400.f, 400.f), 5.f));
	_particleAnimator = simAnim;
	//SimulationAnimator* fluidSimAnim = new SimulationAnimator(_particleManager);
	//_particleAnimator = fluidSimAnim;
	_particleRenderer = new ParticleRenderer(_particleManager);

	width = w;
	height = h;

}

void RealtimeParticlesShowElement::update()
{
	RealtimeAudioShowElement::update();

	float currentTime = ofGetElapsedTimef();
	float timeStep = currentTime - prevTime;
	prevTime = currentTime;

	if (timeStep < 1.0f) // ignore timesteps that are too large
	{
		ofVec2f direction = ofVec2f(ParticleGenerator::RandomCentered(5.f), ParticleGenerator::RandomCentered(5.f));
		direction.normalize();
		_particleGenerator->Generate(timeStep, ofVec2f(400, 400), direction, 1.f);
		_particleManager->Update(timeStep);
		_particleAnimator->Update(currentTime, timeStep); // compute new properties (position, color, age, etc) all particles
	}
}

void RealtimeParticlesShowElement::draw()
{
	RealtimeAudioShowElement::draw();

	// draw the glows of the lines
	//glow.draw_dontEnableAlphaBlending(x, y, width, height);

	// draw the lines themselves
	_particleRenderer->Draw(ofVec2f(0, 0), 0.5f);
}

void RealtimeParticlesShowElement::drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName)
{
	graphic.clear();

	ofPolyline line;

	Particle* particles = _particleManager->GetParticlesArray();
	for (int i = 0; i < _particleManager->GetNrOfActiveParticles(); i++)
	{
		line.clear();
		line.addVertex(particles[i].Position.x, particles[i].Position.y);
		line.addVertex(particles[i].Position.x + particles[i].Speed.x * 5, particles[i].Position.y + particles[i].Speed.y * 5);

		graphic.addPolyline(line, ofColor(255.0f, 255.0f, 0.0f, 255.0f), false, true);
	}

	laserManager.beginDraw();
	laserManager.drawLaserGraphic(graphic, 1, renderProfileName);
	laserManager.endDraw();

}

// --------------------------- RealtimeCirclesShowElement ------------------
RealtimeCirclesShowElement::RealtimeCirclesShowElement(string name, string inputdevicename, GraphicSet* graphicsset) 
	: RealtimeAudioShowElement(name, inputdevicename, graphicSet)
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

	if (a == false)
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
	pulsing = false;

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

				pulsing = true;
			}
		}
	}
}

void RealtimeCirclesShowElement::draw()
{
}

void RealtimeCirclesShowElement::UpdateGraphic(ofColor color)
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

	ellipseGraphic.addPolyline(ellipse, color, false, true);
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
		ofColor c = ofColor(255.0f, 255.0f, 0.0f, 255.0f);
		if (snd.getIsPlaying())
			c = ofColor::red;

		UpdateGraphic(c);

		ofTranslate(400, 400);

		float s = scale;
		if (pulsing)
			s = scale * 1.2;

		ofScale(s, s);

		float angle = fmod(ofGetElapsedTimef() * 30, 180) - 90;
		ofRotateYDeg(angle);

		laserManager.drawLaserGraphic(ellipseGraphic, 1, renderProfileName);

		ofPopMatrix();

	}
	laserManager.endDraw();
}
