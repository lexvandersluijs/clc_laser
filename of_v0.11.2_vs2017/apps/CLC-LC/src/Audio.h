#pragma once

#include "ofMain.h"
#include "fft.h"
#include "SmoothValue.h"
#include "ofxOnsetDetection.h"

#define BUFFER_SIZE 256
#define NUM_WINDOWS 80

class Audio {

public:
	Audio();
	vector <float> left;
	vector <float> right;


	void update(float * input, int bufferSize);
	void calc();
	SmoothValue getBass();
	SmoothValue getMid();
	SmoothValue getHigh();
	SmoothValue getVol();
	static float toDB(float amp);

	bool getIsOnsetting() { return isOnsetting; }

	float getRawBass() { return bass_amount; }
	float getRawMid() { return mid_amount; }
	float getRawHigh() { return high_amount; }
	float getAvgPower() { return avg_power; }

private:
	fft		myfft;
	float * data;

	float avg_power = 0.0f;
	float bass_amount = 0;
	float mid_amount = 0;
	float high_amount = 0.0f;

	SmoothValue volume;
	SmoothValue bass;
	SmoothValue mid;
	SmoothValue high;

	float curVol;

	float phase[BUFFER_SIZE];
	float power[BUFFER_SIZE];
	float magnitude[BUFFER_SIZE];
	//float freq[NUM_WINDOWS][BUFFER_SIZE / 2];
	//float freq_phase[NUM_WINDOWS][BUFFER_SIZE / 2];

	bool isOnsetting = false;
	ofxOnsetDetection onsetD;
};