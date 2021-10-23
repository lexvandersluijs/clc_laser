#include "ofApp.h"

// You can use the SVGLoader utility class to load an SVG sequence
// as an animation. The example animated SVG sequence is from
// a collaboration with comedian Bec Hill, see
// https://youtu.be/U4D0wJNYtWs for the back story.


//--------------------------------------------------------------
void ofApp::setup(){

	svgShowElement.setup();
	svgShowElement.setup2(laserManager);
	timelineShowElement.setup();
	realtimeShowElement.setup();
    


}

//--------------------------------------------------------------
void ofApp::update(){
    laserManager.update();

	svgShowElement.update();
	timelineShowElement.update();
	realtimeShowElement.update();
}


//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(15,15,20);

	// draw the selected show element to the laser (manager)
	// TODO: I think we want to set the render profile on a showElement-by-showElement basis
	svgShowElement.drawLaserGraphic(laserManager, "DEFAULT");

	// let the selected show element draw its UI to the screen
	svgShowElement.draw();

	// send the data to the laser
    laserManager.send();

	// draw the laser manager UI to the screen
    laserManager.drawUI();
 

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
//    if(key == OF_KEY_TAB) {
//        laserManager.selectNextLaser();
//    }
}

