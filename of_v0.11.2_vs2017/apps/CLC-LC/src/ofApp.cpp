#include "ofApp.h"

#include "RealtimeAudioShowElement.h"
#include "TimelineWithAudioShowElement.h"

// You can use the SVGLoader utility class to load an SVG sequence
// as an animation. The example animated SVG sequence is from
// a collaboration with comedian Bec Hill, see
// https://youtu.be/U4D0wJNYtWs for the back story.

ofApp::ofApp()
{
	// listen on the given port
	cout << "OSC Listening for osc messages on port " << PORT;
	receiver.setup(PORT);

	currentMsgString = 0;
}

ofApp::~ofApp()
{
	for (int i = 0; i < showElements.size(); i++)
	{
		showElements[i]->saveParameters();
		delete showElements[i];
	}
	showElements.clear();
}

//--------------------------------------------------------------
void ofApp::setup(){

	showElements.push_back(new SvgShowElement("Mask Calibration"));
	//showElements.push_back(new SvgShowElement("SVG-LEADERS"));
	showElements.push_back(new SvgShowElement("SVG-LOGO-LC21"));
	showElements.push_back(new SvgShowElement("Leadership"));
	showElements.push_back(new SvgShowElement("Let's Talk"));
	//showElements.push_back(new TimelineWithAudioShowElement("Song-1", "sound/AUD-20211025-WA0003-vnc.mp3", new SuperEllipseSet(4, 100, 100)));
	//showElements.push_back(new TimelineWithAudioShowElement("Song-2-Lines", "sound/NextStep_CLCEdit_Richard.mp3", new LineSet()));
	//showElements.push_back(new TimelineWithAudioShowElement("Song-3-HorLines", "sound/NextStep_CLCEdit_Richard.mp3", new HorizontalLineSet()));
	//showElements.push_back(new TimelineWithAudioShowElement("Song-3-Concentric", "sound/NextStep_CLCEdit_Richard.mp3", new ConcentricCircleSet()));
	showElements.push_back(new RealtimeAudioShowElement("SuperEllipse-1", "Microphone (2- USB Audio CODEC )", new SuperEllipseSet(4, 80, 80)));
	showElements.push_back(new RealtimeAudioShowElement("VerticalLines-1", "Microphone (2- USB Audio CODEC )", new LineSet()));
	showElements.push_back(new RealtimeAudioShowElement("HorizontalLines-1", "Microphone (2- USB Audio CODEC )", new HorizontalLineSet()));
	showElements.push_back(new RealtimeAudioShowElement("SuperDiamonds-1", "Microphone (2- USB Audio CODEC )", new SuperEllipseSet(0.5, 80, 80)));
	showElements.push_back(new RealtimeAudioShowElement("SquareSupers-1", "Microphone (2- USB Audio CODEC )", new SuperEllipseSet(6, 80, 80)));
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
	laserManager.setCanvasSize(600, 800);
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
			setSelectedShowElement(i, false);
		}
	}
}

void ofApp::setSelectedShowElement(int index, bool fromOSC)
{
	// make the selected showElement index match the selected checkbox
	selectedShowElementIndex = index;

	if (fromOSC) // if the setting came from the network, we need to change the checkbox in the UI, otherwise this was already done
	{
		selectedElementBooleans[selectedShowElementIndex]->set(true);
	}

	showElements[selectedShowElementIndex]->setActive(true);

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

//--------------------------------------------------------------
void ofApp::update(){
    laserManager.update();

	showElements[selectedShowElementIndex]->update();

	// --------------------------- OSC --------------------------------
		// hide old messages
	for (int i = 0; i < NUM_MSG_STRINGS; i++) {
		if (timers[i] < ofGetElapsedTimef()) {
			msgStrings[i] = "";
		}
	}

	// check for waiting messages
	while (receiver.hasWaitingMessages()) {

		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(m);

		// check for mouse moved message
		if (m.getAddress() == "/oscControl/gridButton1") // disarm all lasers
		{
			laserManager.disarmAllLasers();
		}
		// check for mouse button message
		else if (m.getAddress() == "/oscControl/gridButton2") // arm all lasers
		{ 
			laserManager.armAllLasers();
		}
		else if (m.getAddress() == "/oscControl/maskCalib") // mask calib pattern
		{
			setSelectedShowElement(0, true);
		}
		else if (m.getAddress() == "/oscControl/clclc") // CLC-LC spinning logo
		{
			setSelectedShowElement(1, true);
		}
		else if (m.getAddress() == "/oscControl/leadership") // Leadership text
		{
			setSelectedShowElement(2, true);
		}
		else if (m.getAddress() == "/oscControl/letstalk") // Let's talk text
		{
			setSelectedShowElement(3, true);
		}
		else if (m.getAddress() == "/oscControl/superellipse") // 'TV' superellipses
		{
			setSelectedShowElement(4, true);
		}
		else if (m.getAddress() == "/oscControl/verticallines") // spinning vertical lines
		{
			setSelectedShowElement(5, true);
		}
		else if (m.getAddress() == "/oscControl/horlines") // horizontal lines
		{
			setSelectedShowElement(6, true);
		}
		else if (m.getAddress() == "/oscControl/superdiamonds") // super ellipse diamonds
		{
			setSelectedShowElement(7, true);
		}
		else if (m.getAddress() == "/oscControl/squaresupers") // rotating square-ish superellipses
		{
			setSelectedShowElement(8, true);
		}

//		else {

			// unrecognized message: display on the bottom of the screen
			string msgString;
			msgString = m.getAddress();
			msgString += ":";
			for (size_t i = 0; i < m.getNumArgs(); i++) {

				// get the argument type
				msgString += " ";
				msgString += m.getArgTypeName(i);
				msgString += ":";

				// display the argument - make sure we get the right type
				if (m.getArgType(i) == OFXOSC_TYPE_INT32) {
					msgString += ofToString(m.getArgAsInt32(i));
				}
				else if (m.getArgType(i) == OFXOSC_TYPE_FLOAT) {
					msgString += ofToString(m.getArgAsFloat(i));
				}
				else if (m.getArgType(i) == OFXOSC_TYPE_STRING) {
					msgString += m.getArgAsString(i);
				}
				else {
					msgString += "unhandled argument type " + m.getArgTypeName(i);
				}
			}

			// add to the list of strings to display
			msgStrings[currentMsgString] = msgString;
			timers[currentMsgString] = ofGetElapsedTimef() + 5.0f;
			currentMsgString = (currentMsgString + 1) % NUM_MSG_STRINGS;

			// clear the next line
			msgStrings[currentMsgString] = "";
//		}
	}
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

	// ----------------------- OSC -----------------------

	// draw recent unrecognized messages
	for (int i = 0; i < NUM_MSG_STRINGS; i++) {
		ofDrawBitmapStringHighlight(msgStrings[i], 10, 40 + 15 * i);
	}

	string buf = "listening for osc messages on port " + ofToString(PORT);
	ofDrawBitmapStringHighlight(buf, 10, 20);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == OF_KEY_SPACE) 
	{
		showElements[selectedShowElementIndex]->SpaceBarPressed();
    }
}

