#include "GraphicElement.h"

float sgn(float value)
{
	if (value < 0)
		return -1;
	else if (value == 0)
		return 0;
	else
		return 1;
}

// -------------------------- GraphicElement -------------------------------
float MAX_PULSE_DURATION = 1; // sec
void GraphicElement::pulse()
{
	pulseTime = ofGetElapsedTimef();

}

void GraphicElement::update()
{
	float timeDif = ofGetElapsedTimef() - pulseTime;
	if (timeDif > 0 && timeDif < MAX_PULSE_DURATION)
		pulseFactor = pow(0.001f, timeDif * 2); // exponential decay.. 
	else
		pulseFactor = 0;
}

ofPolyline& GraphicElement::getPolyline()
{
	return polyline;
}
ofColor GraphicElement::getColor()
{
	return color;
}
VerticalLine::VerticalLine(float x, ofColor c)
{
	xCoord = x;
	color = c;
}

ofPolyline& VerticalLine::getPolyline()
{
	polyline.clear();

	polyline.addVertex(xCoord + pulseFactor * 20, 0);
	polyline.addVertex(xCoord + pulseFactor * 20, 800);

	return polyline;
}

SuperEllipse::SuperEllipse(ofVec2f cent, ofColor c)
{
	center = cent;
	color = c;
}

ofPolyline& SuperEllipse::getPolyline()
{
	polyline.clear();

	int NUM_STEPS = 32;
	float a = 1;
	float b = 1;
	float n = 4;
	float scale = 100 + 100 * pulseFactor;
	for (int i = 0; i < NUM_STEPS; i++)
	{
		float t = i / (float)(NUM_STEPS - 1);
		float tp = t * M_TWO_PI;
		float cost = cos(tp);
		float sint = sin(tp);
		float x = center.x + scale * pow(abs(cost), 2 / n) * a * sgn(cost);
		float y = center.y + scale * pow(abs(sint), 2 / n) * b * sgn(sint);

		polyline.addVertex(x, y, 0);
	}
	polyline.close();


	return polyline;
}

SuperEllipseSet::SuperEllipseSet()
{
	lines.push_back(new SuperEllipse(ofVec2f(100, 100), ofColor::red));
	lines.push_back(new SuperEllipse(ofVec2f(150, 500), ofColor::green));
	lines.push_back(new SuperEllipse(ofVec2f(500, 400), ofColor::orange));
}


LineSet::LineSet()
{
	lines.push_back(new VerticalLine(100, ofColor::red));
	lines.push_back(new VerticalLine(300, ofColor::green));
	lines.push_back(new VerticalLine(500, ofColor::orange));
}

