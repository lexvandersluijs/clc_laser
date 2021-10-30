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

protected:
	float pulseTime;
	float pulseFactor;

	ofPolyline polyline;
	ofColor color;
};

class VerticalLine : public GraphicElement
{
public:
	VerticalLine(float x, ofColor c);
	virtual ofPolyline& getPolyline();

protected:
	float xCoord;
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

	ofVec2f center;
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

	virtual void drawToGraphic(ofxLaser::Graphic& graphic)
	{
		for (int i = 0; i < lines.size(); i++)
		{
			graphic.addPolyline(lines[i]->getPolyline(), lines[i]->getColor(), false, true);
		}
	}

protected:
	vector<GraphicElement*> lines;
};

class SuperEllipseSet : public GraphicSet
{
public:
	SuperEllipseSet(float n, float a, float b);
	void setParams(float n, float a, float b);

protected:

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

protected:

};