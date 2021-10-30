#include "SmoothValue.h"

SmoothValue::SmoothValue() {
	value = 0;
}

void SmoothValue::setRange(float rmin, float rmax) {
	min = rmin;
	max = rmax;
}

void SmoothValue::update(float val) {
	if (val > max) {
		val = max;
	}

	if (val < min) {
		val = min;
	}
	value *= 0.93;
	value += 0.07 * val;
	threshold.compare(scale());
}

float SmoothValue::scale() {
	return ofMap(value, min, max, 0.0f, 1.0f, true);
}

float SmoothValue::getVal() {
	return value;
}

Threshold SmoothValue::getThreshold() {
	return threshold;
}