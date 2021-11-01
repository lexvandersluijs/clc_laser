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
	//PrintDirectShowAudioDeviceList(*soundStream.getSoundStream());
	
	addParameter(inputDeviceName.set("Input device name", inputDeviceNameFromConstructor));

	//std::vector<ofSoundDevice> matchingDevices = soundStream.getMatchingDevices(inputDeviceName, 2, 0, ofSoundDevice::Api::MS_DS);
	auto devices = soundStream.getSoundStream()->getDeviceList(ofSoundDevice::MS_DS);

	bool inputDeviceFound = false;
	for (int i = 0; i < devices.size(); i++)
	{
		cout << "input device[" << i << "]: " << devices << endl;
		inputDeviceNamesAvailable.push_back(devices[i].name);
		if (devices[i].name == inputDeviceName.get())
			cout << "Input device named \"" << inputDeviceName << "\" found on the system" << endl;
	}
	addParameter(startPlayingOnActivation.set("Start playing on activation", false));
	addParameter(reactToAudio.set("React to audio", true));
	addParameter(velocityFactor.set("Velocity factor", 1.0f, 0, 5.0f));
	addParameter(pulseResponseInPixels.set("Pulse response (px)", 20.0f, 0, 100.0f));
	addParameter(color1.set("Color 1", 0, 0, ofColor::aliceBlue));
	addParameter(color2.set("Color 2", 0, 0, ofColor::aliceBlue));
	addParameter(color3.set("Color 3", 0, 0, ofColor::aliceBlue));
	addParameter(color4.set("Color 4", 0, 0, ofColor::aliceBlue));
	addParameter(color5.set("Color 5", 0, 0, ofColor::aliceBlue));

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
		if (reactToAudio.get())
		{
			// compute FFT and such
			//audioProcessor.calc();
			bd.updateFFT();

			//bool isOnsetting = onsetD.isOnsetting(bd.getMagnitude());
			//if (isOnsetting)
			//	cout << "isOnSetting = " << isOnsetting << endl;

			if (bd.isKick())
			{
				//cout << "RealtimeAudioShowElement bd.isKick() bd.magnitude[10] = " << bd.magnitude[10] << endl;
				//graphicSet->pulse(0);
				//float newRadius = 100 * (1.0f + 100 * audioProcessor.getAvgPower());
				//float newRadius = 100 * (1.0f + bd.magnitude[10]);
				//(dynamic_cast<SuperEllipseSet*>(graphicSet))->setParams(4, newRadius, newRadius);
				for (int i = 0; i < graphicSet->getLineCount(); i++)
					if (i % 3 == 0)
						graphicSet->pulse(i);
			}
			if (bd.isSnare())
			{
				for (int i = 0; i < graphicSet->getLineCount(); i++)
					if (i % 2 == 1)
						graphicSet->pulse(i);
			}
			if (bd.isHat())
			{
				for (int i = 0; i < graphicSet->getLineCount(); i++)
					if (i % 3 == 2)
						graphicSet->pulse(i);
			}
		}
		graphicSet->setVelocityFactor(velocityFactor.get());
		graphicSet->setPulseResponseInPixels(pulseResponseInPixels.get());
		graphicSet->update();
	}
}

void RealtimeAudioShowElement::draw()
{
	float x = 0;
	float spacing = 8;
	UI::startWindow("Settings - " + name, ImVec2(x, 804), ImVec2(ofGetWindowWidth() / 2, 0));

	if (ImGui::BeginCombo("##Audio input devices", inputDeviceName.get().c_str())) { // The second parameter is the label previewed before opening the combo.

		for (const string input : inputDeviceNamesAvailable) {

			if (ImGui::Selectable(input.c_str(), inputDeviceName.get() == input)) {
				//get the preset and make a copy of it
				// uses operator overloading to create a clone
				//laser->scannerSettings = *presetManager.getPreset(presetName);
				inputDeviceName = input;
			}
		}

		ImGui::EndCombo();
	}

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

	// automatically turn on the input (if possible.. input source already set correctly in UI) when activated?

	if (startPlayingOnActivation.get() && a == true)
		StartAudioInput();

	// let's try not turning the input off at the end... reason: when we turn it on, the first data
	// we get is garbage, causing erratic movement. Could also choose to ignore the first second or so,
	// but let's try it this way first.
	//if (a == false)
	//	StopAudioInput();
}

void RealtimeAudioShowElement::StartAudioInput()
{
	//if you want to set a different device id
//soundStream.setDeviceID(4); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
//bool result = soundStream.setup(app, 2, 1, 44100, BUFFER_SIZE, 4);

	int bufferSize = 512;
	bool useNewDeviceSelection = true;

	if (useNewDeviceSelection)
	{
		std::vector<ofSoundDevice> matchingDevices = soundStream.getMatchingDevices(inputDeviceName, 2, 0, ofSoundDevice::Api::MS_DS);

		cout << "Selected input sound device: " << matchingDevices[0] << endl;

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

	isPlaying = true;
}

void RealtimeAudioShowElement::StopAudioInput()
{
	isPlaying = false;

	audioReceived = false;
	soundStream.stop();
}

void RealtimeAudioShowElement::SpaceBarPressed()
{
	if (isPlaying)
	{
		StopAudioInput();
	}
	else
	{
		StartAudioInput();
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


