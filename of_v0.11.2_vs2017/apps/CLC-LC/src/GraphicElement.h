#pragma once

#include "ofMain.h"
#include "ofxLaserManager.h"

float sgn(float value);

class GraphicElement
{
public:
	virtual ~GraphicElement() {}
	virtual void pulse();
	virtual void update();
	virtual ofPolyline& getPolyline();
	ofColor getColor();

	void setCenter(ofVec2f c) { center = c; }
	void setRotation(float r) { rotation = r; }
	ofVec2f getCenter() { return center; }
	float getRotation() { return rotation; }

	void drawToGraphic(ofxLaser::Graphic& graphic);

	void setPulseResponseInPixels(float pulseResponse) { pulseResponseInPixels = pulseResponse; }

protected:
	float pulseTime;
	float pulseFactor;
	float pulseResponseInPixels;

	ofPolyline polyline;
	ofColor color;

	ofVec2f center;
	float rotation = 0;
};

class VerticalLine : public GraphicElement
{
public:
	VerticalLine(float x, ofColor c);
	virtual ofPolyline& getPolyline();

protected:
	float xCoord;
};

class HorizontalLine : public GraphicElement
{
public:
	HorizontalLine(float y, ofColor c);
	virtual ofPolyline& getPolyline();

protected:
	float yCoord;
};

class SuperEllipse : public GraphicElement
{
public:
	SuperEllipse(float n, float a, float b, ofVec2f center, ofColor c);
	virtual ofPolyline& getPolyline();

	void setParams(float n, float a, float b) { param_n = n; param_a = a; param_b = b; }

protected:
	float param_n;
	float param_a;
	float param_b;
};

class GraphicSet
{
public:
	virtual ~GraphicSet()
	{
		for (int i = 0; i < lines.size(); i++)
			delete lines[i];
	}
	virtual void pulse(int index)
	{
		if (index < lines.size())
		{
			lines[index]->pulse();
		}
	}
	virtual void update()
	{
		// process the pulse responses
		for (int i = 0; i < lines.size(); i++)
			lines[i]->update();
	}

	virtual void drawToGraphic(ofxLaser::Graphic& graphic);

	int getLineCount() { return lines.size(); }
	void setVelocityFactor(float factor) { velocityFactor = factor; }
	void setPulseResponseInPixels(float pixels)
	{
		for (int i = 0; i < lines.size(); i++)
			lines[i]->setPulseResponseInPixels(pixels);
	}

protected:
	vector<GraphicElement*> lines;
	float velocityFactor = 1.0f;
};

class SuperEllipseSet : public GraphicSet
{
public:
	SuperEllipseSet(float n, float a, float b);
	void setParams(float n, float a, float b);
	virtual void drawToGraphic(ofxLaser::Graphic& graphic);

protected:
	float startTime;

};

class ConcentricCircleSet : public GraphicSet
{
public:
	ConcentricCircleSet();

protected:

};

class LineSet : public GraphicSet
{
public:
	LineSet();
	virtual void drawToGraphic(ofxLaser::Graphic& graphic);

protected:

};

class HorizontalLineSet : public GraphicSet
{
public:
	HorizontalLineSet();

protected:

};