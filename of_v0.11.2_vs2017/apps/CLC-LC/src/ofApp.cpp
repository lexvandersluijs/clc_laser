#include "ofApp.h"

#include "RealtimeAudioShowElement.h"
#include "TimelineWithAudioShowElement.h"

// You can use the SVGLoader utility class to load an SVG sequence
// as an animation. The example animated SVG sequence is from
// a collaboration with comedian Bec Hill, see
// https://youtu.be/U4D0wJNYtWs for the back story.

ofApp::~ofApp()
{
	for (int i = 0; i < showElements.size(); i++)
	{
		delete showElements[i];
	}
}

//--------------------------------------------------------------
void ofApp::setup(){

	showElements.push_back(new SvgShowElement("Mask Calibration"));
	showElements.push_back(new SvgShowElement("SVG-LEADERS"));
	showElements.push_back(new SvgShowElement("SVG-LOGO-LC21"));
	showElements.push_back(new TimelineWithAudioShowElement("Song-1", "sound/AUD-20211025-WA0003-vnc.mp3", new SuperEllipseSet(4, 100, 100)));
	showElements.push_back(new TimelineWithAudioShowElement("Song-2-Lines", "sound/NextStep_CLCEdit_Richard.mp3", new LineSet()));
	//showElements.push_back(new TimelineWithAudioShowElement("Song-3-Concentric", "sound/NextStep_CLCEdit_Richard.mp3", new ConcentricCircleSet()));
	showElements.push_back(new RealtimeAudioShowElement("SuperEllipse-1", "Microphone (Realtek(R) Audio)", new SuperEllipseSet(2, 100, 100)));
	//showElements.push_back(new SvgAnimationShowElement("SVG-Animation-Motorbikes"));
	//showElements.push_back(new RealtimeCirclesShowElement("RT-Circles", "Microphone (Realtek(R) Audio)", new ConcentricCircleSet()));
	//showElements.push_back(new RealtimeParticlesShowElement("RT-Particles"));

	// initialize the selected state
	selectedShowElementIndex = 0;
	showElements[selectedShowElementIndex]->setActive(true);

	for (int i = 0; i < showElements.size(); i++)
	{
		showElements[i]->setup();

		// populate the list of checkboxes with which we select the current show element
		// for convenience we add them to the laserManager, until we figure out a better way..
		ofParameter<bool>* param = new ofParameter<bool>(showElements[i]->getName(), i == selectedShowElementIndex);

		param->addListener(this, &ofApp::selectedShowElementChanged );

		selectedElementBooleans.push_back(param);
		laserManager.addCustomParameter(*param);
	}

	laserManager.renderLater = true;
}


void ofApp::selectedShowElementChanged(const void* sender, bool & newValue)
{
	cout << "ofApp::selectedShowElementChanged()" << endl;

	if (newValue == false)
		return;

	for (int i = 0; i < selectedElementBooleans.size(); i++)
	{
		if(showElements[i]->getActive() == false && selectedElementBooleans[i]->get() == true)
		{
			// make the selected showElement index match the selected checkbox
			selectedShowElementIndex = i;
			showElements[i]->setActive(true);

			// and disable the checkbox and showElement that is not selected anymore
			for (int i = 0; i < selectedElementBooleans.size(); i++)
			{
				if (i != selectedShowElementIndex)
				{
					selectedElementBooleans[i]->set(false);
					showElements[i]->setActive(false);
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::update(){
    laserManager.update();

	showElements[selectedShowElementIndex]->update();
}


//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(15,15,20);

	// draw the selected show element to the laser (manager)
	// TODO: I think we want to set the render profile on a showElement-by-showElement basis
	showElements[selectedShowElementIndex]->drawLaserGraphic(laserManager, "DEFAULT");
	
	// send the data to the laser - as little latency as possible
	laserManager.send();

	// draw the laser manager UI to the screen
	laserManager.drawUI();

	// let the selected show element draw its UI to the screen
	showElements[selectedShowElementIndex]->draw();

	ofxLaser::UI::render();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == OF_KEY_SPACE) 
	{
		showElements[selectedShowElementIndex]->SpaceBarPressed();
    }
}

