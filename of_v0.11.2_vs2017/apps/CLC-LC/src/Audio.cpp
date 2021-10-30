#include "Audio.h"

Audio::Audio() {
	left.assign(BUFFER_SIZE, 0.0);
	right.assign(BUFFER_SIZE, 0.0);

	data = new float[BUFFER_SIZE];

	volume.setRange(0.0, 0.17);
	bass.setRange(20, 30);
	mid.setRange(-20, 17);
	high.setRange(-40, 5);

	// set up the onset detection
	onsetD.setup(OFX_ODS_ODF_RCOMPLEX, 2, 256);
}

void Audio::update(float * input, int bufferSize) {
	curVol = 0.0;

	// samples are "interleaved"
	int numCounted = 0;

	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume
	for (int i = 0; i < bufferSize; i++) {
		left[i] = input[i * 2] * 0.5;
		right[i] = input[i * 2 + 1] * 0.5;
		data[i] = left[i] + right[i];

		curVol += left[i] * left[i];
		curVol += right[i] * right[i];

		numCounted += 2;
	}
	curVol /= (float)numCounted;
	curVol = sqrt(curVol);
}


void Audio::calc() {
	avg_power = 0.0f;
	bass_amount = 0;
	mid_amount = 0;
	high_amount = 0.0f;

	int bass_count = 0;
	int mid_count = 0;
	int high_count = 0;

	myfft.powerSpectrum(0, (int)BUFFER_SIZE / 2, data, BUFFER_SIZE, &magnitude[0], &phase[0], &power[0], &avg_power);


	for (int i = 1; i < (int)(BUFFER_SIZE / 2); i++) {
		if (i < 3) {
			bass_amount += magnitude[i];
			bass_count++;
		}
		else if (i < (int)(BUFFER_SIZE / 6)) {
			mid_amount += magnitude[i];
			mid_count++;
		}
		else {
			high_amount += magnitude[i];
			high_count++;
		}
	}

	bass_amount /= bass_count;
	mid_amount /= mid_count;
	high_amount /= high_count;

	bass_amount = toDB(bass_amount);
	mid_amount = toDB(mid_amount);
	high_amount = toDB(high_amount);

	bass.update(bass_amount);
	mid.update(mid_amount);
	high.update(high_amount);
	volume.update(curVol);

	if (false)
	{
		cout << "----------------- Audio.cpp -------------------" << endl;
		for (int q = 0; q < BUFFER_SIZE; q++)
		{
			cout << magnitude[q] << ", ";
		}
		cout << "----------------- /Audio.cpp -------------------" << endl;
	}

	isOnsetting = onsetD.isOnsetting(&magnitude[0]);
}

SmoothValue Audio::getBass() {
	return bass;
}

SmoothValue Audio::getMid() {
	return mid;
}

SmoothValue Audio::getHigh() {
	return high;
}

SmoothValue Audio::getVol() {
	return volume;
}

float Audio::toDB(float amp) {
	return 20 * log10(amp);
}