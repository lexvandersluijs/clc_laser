#include "TimelineWithAudioShowElement.h"

// -------------------------- TimelineShowElement --------------------------
TimelineWithAudioShowElement::TimelineWithAudioShowElement(string name, string audiofilepath, GraphicSet* graphicset) : ShowElement(name)
{
	audioFilePath = audiofilepath;
	
	graphicSet = graphicset;
}

TimelineWithAudioShowElement::~TimelineWithAudioShowElement()
{
	if(graphicSet != NULL)
		delete graphicSet;
}

void TimelineWithAudioShowElement::setup()
{
	// set up the onset detection
	onsetD.setup(OFX_ODS_ODF_RCOMPLEX, 2, 256);

	// ----------- register parameters -----------------
	addParameter(Mask1L.set("Mask 1 L", 50, 0, 800));
	addParameter(Mask1R.set("Mask 1 R", 250, 0, 800));
	addParameter(Mask2L.set("Mask 2 L", 350, 0, 800));
	addParameter(Mask2R.set("Mask 2 R", 650, 0, 800));

	// ---------------- set up the timeline ----------------
	ofxTimeline::removeCocoaMenusFromGlut("AllTracksExample");
	timeline.setup();
	timeline.setOffset(ofVec2f(0, 800));

	timeline.addAudioTrack("audio", audioFilePath);
	timeline.setDurationInSeconds(timeline.getAudioTrack("audio")->getDuration());
	timeline.setLoopType(ofLoopType::OF_LOOP_NONE);

	timeline.addCurves("curves", ofRange(0, 255));
	timeline.addBangs("bangs");
	timeline.enableSnapToOtherKeyframes(false);


	// we don't want the timeline to start when it is not on the active showElement
	// do we disable this here and explicity toggle this via the SpaceBarPressed() function
	timeline.setSpacebarTogglePlay(false);

	ofAddListener(timeline.events().bangFired, this, &TimelineWithAudioShowElement::bangFired);
}


void TimelineWithAudioShowElement::SpaceBarPressed()
{
	timeline.togglePlay();
}

void TimelineWithAudioShowElement::setActive(bool a)
{
	ShowElement::setActive(a);

	if (a == false)
	{
		if (timeline.getIsPlaying())
			timeline.stop();
	}
}

void TimelineWithAudioShowElement::update()
{
	if (timeline.getIsPlaying())
	{
		//float* spectrum = ofSoundGetSpectrum(nBands);
		vector<float>spectrumVector = timeline.getAudioTrack("audio")->getFFT();
		int nBands = spectrumVector.size();

		// the address of the first element is the start of the array
		float* spectrum = &(spectrumVector[0]);

		float avg = 0;
		float avgLow = 0;
		float avgMid = 0;
		float avgHigh = 0;
		int nrOfBlocks = 10;
		float bandsBlock = nBands / nrOfBlocks;
		int bandwidth1 = bandsBlock * 2;
		int bandwidth2 = bandsBlock * 5; // all the rest are higher frequencies
		int bandwidth3 = nBands - bandwidth2;
		for (int i = 0; i < nBands; ++i)
		{
			if (i < bandwidth1)
			{
				avgLow += spectrum[i];
			}
			else if (i >= bandwidth1 && i < bandwidth2)
			{
				avgMid += spectrum[i];
			}
			else
			{
				avgHigh += spectrum[i];
			}
			avg += spectrum[i];
		}

		avg /= (float)nBands;
		avgLow /= (float)bandwidth1;
		avgMid /= (float)bandwidth2;
		avgHigh /= (float)bandwidth3;

		cout << "l: " << avgLow << ", m: " << avgMid << ", h: " << avgHigh << ", avg: " << avg << endl;
		float normalizationFactor = 10.f; // let's try to get these in the 0 to 1 range..
		avg *= normalizationFactor;
		avgLow *= normalizationFactor;
		avgMid *= normalizationFactor;
		avgHigh *= normalizationFactor * 2.f;


		if (onsetD.isOnsetting(spectrum))
		{
			cout << "----------------- TimelineWithAudioShowElement.cpp -------------------" << endl;
			for (int q = 0; q < spectrumVector.size(); q++)
			{
				cout << spectrumVector[q] << ", ";
			}
			cout << "----------------- /TimelineWithAudioShowElement.cpp -------------------" << endl;
			//musicShader->pulse(0);

			// depending on which frequency band is strongest
			// we pulse a certain graphic element
			if ((avgLow > avgMid) && (avgLow > avgHigh))
				graphicSet->pulse(0);
			else if ((avgMid > avgLow) && (avgMid > avgHigh))
				graphicSet->pulse(1);
			else
				graphicSet->pulse(2);
		}
	}

	graphicSet->update();
}

void TimelineWithAudioShowElement::draw()
{
	// --------- draw the timeline --------------
	timeline.draw(true);//false removed tickers, true(default) adds them

	float x = 0;
	float spacing = 8;
	UI::startWindow("Settings - " + name, ImVec2(x, 804), ImVec2(ofGetWindowWidth() / 2, 0));

	ofxLaser::UI::addParameterGroup(parameters);

	UI::endWindow();
}

void TimelineWithAudioShowElement::bangFired(ofxTLBangEventArgs& args)
{
	cout << "bang fired!" << args.flag << endl;
}

void TimelineWithAudioShowElement::drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName)
{
	laserManager.beginDraw();

	graphic.clear();

	bool clip = true;
	graphicSet->drawToGraphic(graphic);

	if (clip)
	{
		ofPath clippingPath1, clippingPath2;
		clippingPath1.rectangle(Mask1L, 0, Mask1R - Mask1L, 800);
		clippingPath2.rectangle(Mask2L, 0, Mask2R - Mask2L, 800);
		vector<ofPath> clippingPaths;
		clippingPaths.push_back(clippingPath1);
		clippingPaths.push_back(clippingPath2);

		graphic.intersectPaths(clippingPaths);
	}
	laserManager.drawLaserGraphic(graphic, 1, renderProfileName);

	laserManager.endDraw();
}
