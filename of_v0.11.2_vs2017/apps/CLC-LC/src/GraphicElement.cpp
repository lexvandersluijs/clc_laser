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

void GraphicElement::drawToGraphic(ofxLaser::Graphic& graphic)
{
	ofPushMatrix();
	ofTranslate(center);
	ofRotateZRad(rotation);

	graphic.addPolyline(getPolyline(), getColor(), false, true);
	ofPopMatrix();
}
// ------------------------------- verticalLine ------------------
VerticalLine::VerticalLine(float x, ofColor c)
{
	xCoord = x;
	color = c;
}

ofPolyline& VerticalLine::getPolyline()
{
	polyline.clear();

	polyline.addVertex(xCoord + pulseFactor * pulseResponseInPixels, -1500); // extend from boundaries of 'screen', these are at 0 and 800
	polyline.addVertex(xCoord + pulseFactor * pulseResponseInPixels, 2200);

	return polyline;
}

HorizontalLine::HorizontalLine(float y, ofColor c)
{
	yCoord = y;
	color = c;
}

ofPolyline& HorizontalLine::getPolyline()
{
	polyline.clear();

	polyline.addVertex(0,   yCoord - pulseFactor * pulseResponseInPixels);
	polyline.addVertex(800, yCoord - pulseFactor * pulseResponseInPixels);

	return polyline;
}

SuperEllipse::SuperEllipse(float n, float a, float b, ofVec2f cent, ofColor c)
{
	param_n = n;
	param_a = a;
	param_b = b;

	center = cent;
	color = c;
}

ofPolyline& SuperEllipse::getPolyline()
{
	polyline.clear();

	int NUM_STEPS = 32;

	float a = param_a + pulseResponseInPixels * pulseFactor; // max value for pulsefactor is 1, so shape can become 50% larger
	float b = param_b + pulseResponseInPixels * pulseFactor;

	for (int i = 0; i < NUM_STEPS; i++)
	{
		float t = i / (float)(NUM_STEPS - 1);
		float tp = t * M_TWO_PI;
		float cost = cos(tp);
		float sint = sin(tp);
		float x = pow(abs(cost), 2 / param_n) * a * sgn(cost);
		float y = pow(abs(sint), 2 / param_n) * b * sgn(sint) * 0.8f; // hack to scale down vertically: looks more like TV tube

		polyline.addVertex(x, y, 0);
	}
	polyline.close();


	return polyline;
}

// ---------------------------- sets ----------------------------
void GraphicSet::drawToGraphic(ofxLaser::Graphic& graphic)
{
	for (int i = 0; i < lines.size(); i++)
	{
		lines[i]->drawToGraphic(graphic);
	}
}


SuperEllipseSet::SuperEllipseSet(float n, float a, float b)
{
	lines.push_back(new SuperEllipse(n, a * 0.9f, b * 0.9f,  ofVec2f(0, 0), ofColor::red));
	lines.push_back(new SuperEllipse(n, a,        b,         ofVec2f(0, 0), ofColor::green));
	lines.push_back(new SuperEllipse(n, a * 1.1f, b * 1.1f,  ofVec2f(0, 0), ofColor::orange));

	lines.push_back(new SuperEllipse(n, a * 0.9f, b * 0.9f,  ofVec2f(0, 0), ofColor::red));
	lines.push_back(new SuperEllipse(n, a,        b,          ofVec2f(0, 0), ofColor::green));
	lines.push_back(new SuperEllipse(n, a * 1.1f, b * 1.1f,   ofVec2f(0, 0), ofColor::orange));

	lines.push_back(new SuperEllipse(n, a * 1.1f,  b * 1.1f,  ofVec2f(0, 0), ofColor::magenta));
	lines.push_back(new SuperEllipse(n, a * 0.85f, b * 0.85f, ofVec2f(0, 0), ofColor::orange));
	//lines.push_back(new SuperEllipse(n, a * 1.1f,  b * 1.1f,  ofVec2f(0, 0), ofColor::green));

	//startTime = ofGetElapsedTimef();

	allParameters.clear();
	for (int i = 0; i < NR_OF_ELLIPSES; i++)
	{
		char buff[100];
		snprintf(buff, sizeof(buff), "Ellipse %d start X", i);
		string name = buff;
		allParameters.add(startX[i].set(name, 0, 0, 800));

		snprintf(buff, sizeof(buff), "Ellipse %d start time", i);
		name = buff;
		allParameters.add(motionStartTime[i].set(name, 0, 0, 20));

		snprintf(buff, sizeof(buff), "Ellipse %d duration", i);
		name = buff;
		allParameters.add(motionDuration[i].set(name, 0, 0, 20));

		snprintf(buff, sizeof(buff), "Ellipse %d color", i);
		name = buff;
		allParameters.add(color[i].set(name, ofColor::green));
	}
}

void SuperEllipseSet::drawUI()
{
	for (int i = 0; i < NR_OF_ELLIPSES; i++)
	{
		ImGui::PushItemWidth(ImGui::GetWindowWidth() / 6.f);
		ofxLaser::UI::addParameter(startX[i]);
		ImGui::SameLine();
		ofxLaser::UI::addParameter(motionStartTime[i]);
		ImGui::SameLine();
		ofxLaser::UI::addParameter(motionDuration[i]);
		ImGui::SameLine();
		ofxLaser::UI::addParameter(color[i]);
		ImGui::PopItemWidth();
	}
}

void SuperEllipseSet::setParams(float n, float a, float b)
{
	for (int i = 0; i < lines.size(); i++)
	{
		(dynamic_cast<SuperEllipse*>(lines[i]))->setParams(n, a, b);
	}
}

void SuperEllipseSet::drawToGraphic(ofxLaser::Graphic& graphic)
{
	// set transform
	float timeElapsed = ofGetElapsedTimef();

	//float distance = velocityFactor * timeElapsed * 50.f; // 50 pixels / second, 16 seconds to cover 800 px
	//float maxDistance = 1200;
	//distance = fmod(distance, maxDistance);

	ofPushMatrix();
	//ofTranslate(0, maxDistance - distance);

	for (int i = 0; i < NR_OF_ELLIPSES; i++)
	{
		lines[i]->setColor(color[i].get());

		// compute all object positions based on current time, X position, start time and animation duration
		float offsetTime = timeElapsed - motionStartTime[i].get();
		if (offsetTime > 0)
		{
			float duration = motionDuration[i].get() / velocityFactor; // use velocityFactor to slow down or speed up the whole set
			float modTime = fmod(offsetTime, duration);
			float motionFraction = modTime / duration;

			float position = ofLerp(900, -100, motionFraction);
			lines[i]->setCenter(ofVec2f(startX[i].get(), position));
		}
		else
			lines[i]->setCenter(ofVec2f(startX[i].get(), 1000)); // set off-screen
	}

	// have base class draw lines
	GraphicSet::drawToGraphic(graphic);

	// pop transform
	ofPopMatrix();
}

ConcentricCircleSet::ConcentricCircleSet()
{
	lines.push_back(new SuperEllipse(2, 80, 80, ofVec2f(400, 400), ofColor::red));
	lines.push_back(new SuperEllipse(2, 120, 120, ofVec2f(400, 400), ofColor::green));
	lines.push_back(new SuperEllipse(2, 140, 140, ofVec2f(400, 400), ofColor::orange));
}


LineSet::LineSet()
{
	lines.push_back(new VerticalLine(60, ofColor::red));
	lines.push_back(new VerticalLine(-100, ofColor::green));
	lines.push_back(new VerticalLine(150, ofColor::orange));
	lines.push_back(new VerticalLine(-220, ofColor::magenta));

	lines[0]->setCenter(ofVec2f(400, 400));
	lines[1]->setCenter(ofVec2f(400, 400));
	lines[2]->setCenter(ofVec2f(400, 400));


	allParameters.clear();
	for (int i = 0; i < NR_OF_VER_LINES; i++)
	{
		char buff[100];
		snprintf(buff, sizeof(buff), "Line %d color", i);
		string name = buff;
		allParameters.add(color[i].set(name, ofColor::green));
	}
}

void LineSet::drawUI()
{
	for (int i = 0; i < NR_OF_VER_LINES; i++)
	{
		ImGui::PushItemWidth(ImGui::GetWindowWidth() / 5.f);
		ofxLaser::UI::addParameter(color[i]);
		ImGui::PopItemWidth();
	}
}

void LineSet::drawToGraphic(ofxLaser::Graphic& graphic)
{
	// set transform
	double timeElapsed = ofGetElapsedTimed() * velocityFactor;
	double timeElapsedModTwoPi = fmod(timeElapsed, TWO_PI*10);

	float maxRotationRadians = 0.8f;
	//float angle = sin(timeElapsedModTwoPi) * maxRotationRadians;
	float angle = timeElapsedModTwoPi; // 1 radian per second, so 2*pi seconds = ~6.3 seconds for a full rotation

	//float maxDistance = 800;
	//distance = fmod(distance, maxDistance);

	ofPushMatrix();
	//ofTranslate(400, 400);
	//ofRotateZRad(angle);
	//ofTranslate(-400, -400);

	// have base class draw lines
	//GraphicSet::drawToGraphic(graphic);

	float finalAngle = angle;
	for (int i = 0; i < lines.size(); i++)
	{
		switch (i)
		{
		case 0:
			finalAngle = -angle*1.05f;
			break;
		case 1:
			finalAngle = angle*0.8f + 1; // 1 radian
			break;
		case 2:
			finalAngle = angle*0.9f - 2;
			break;

		case 3:
			finalAngle = angle*1.1f;
			break;
		}
		finalAngle = fmod(finalAngle, TWO_PI);
		lines[i]->setColor(color[i].get());
		lines[i]->setRotation(finalAngle);
		lines[i]->drawToGraphic(graphic);
	}
	// pop transform
	ofPopMatrix();
}

HorizontalLineSet::HorizontalLineSet()
{
	lines.push_back(new HorizontalLine(100, ofColor::red));
	lines.push_back(new HorizontalLine(300, ofColor::green));
	lines.push_back(new HorizontalLine(400, ofColor::orange));

	allParameters.clear();
	for (int i = 0; i < NR_OF_HOR_LINES; i++)
	{
		char buff[100];
		snprintf(buff, sizeof(buff), "Line %d color", i);
		string name = buff;
		allParameters.add(color[i].set(name, ofColor::green));
	}
}

void HorizontalLineSet::drawUI()
{
	for (int i = 0; i < NR_OF_HOR_LINES; i++)
	{
		ImGui::PushItemWidth(ImGui::GetWindowWidth() / 5.f);
		ofxLaser::UI::addParameter(color[i]);
		ImGui::PopItemWidth();
	}
}

void HorizontalLineSet::drawToGraphic(ofxLaser::Graphic& graphic)
{
	for (int i = 0; i < lines.size(); i++)
	{
		lines[i]->setColor(color[i].get());
		lines[i]->drawToGraphic(graphic);
	}
}