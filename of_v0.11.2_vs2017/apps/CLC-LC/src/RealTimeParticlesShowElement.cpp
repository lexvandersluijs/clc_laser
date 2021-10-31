#include "RealtimeParticlesShowElement.h"

// -------------------------- RealtimeParticlesShowElement ---------------------

void RealtimeParticlesShowElement::setup()
{
	RealtimeAudioShowElement::setup();

	int w = 800;
	int h = 800;
	_particleManager = new ParticleManager(20);
	_particleGenerator = new ParticleGenerator(_particleManager);
	_particleGenerator->SetGenerationRate(1000);
	_particleGenerator->SetParticleLifeTime(1.f);
	SimulationAnimator* simAnim = new SimulationAnimator(_particleManager);
	simAnim->AddForceField(new PointForceField(ofVec2f(400.f, 400.f), 5.f));
	//simAnim->AddForceField(new VortexForceField(ofVec2f(400.f, 400.f), 5.f));
	_particleAnimator = simAnim;
	//SimulationAnimator* fluidSimAnim = new SimulationAnimator(_particleManager);
	//_particleAnimator = fluidSimAnim;
	_particleRenderer = new ParticleRenderer(_particleManager);

	width = w;
	height = h;

}

void RealtimeParticlesShowElement::update()
{
	RealtimeAudioShowElement::update();

	float currentTime = ofGetElapsedTimef();
	float timeStep = currentTime - prevTime;
	prevTime = currentTime;

	if (timeStep < 1.0f) // ignore timesteps that are too large
	{
		ofVec2f direction = ofVec2f(ParticleGenerator::RandomCentered(5.f), ParticleGenerator::RandomCentered(5.f));
		direction.normalize();
		_particleGenerator->Generate(timeStep, ofVec2f(400, 400), direction, 1.f);
		_particleManager->Update(timeStep);
		_particleAnimator->Update(currentTime, timeStep); // compute new properties (position, color, age, etc) all particles
	}
}

void RealtimeParticlesShowElement::draw()
{
	RealtimeAudioShowElement::draw();

	// draw the glows of the lines
	//glow.draw_dontEnableAlphaBlending(x, y, width, height);

	// draw the lines themselves
	_particleRenderer->Draw(ofVec2f(0, 0), 0.5f);
}

void RealtimeParticlesShowElement::drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName)
{
	graphic.clear();

	ofPolyline line;

	Particle* particles = _particleManager->GetParticlesArray();
	for (int i = 0; i < _particleManager->GetNrOfActiveParticles(); i++)
	{
		line.clear();
		line.addVertex(particles[i].Position.x, particles[i].Position.y);
		line.addVertex(particles[i].Position.x + particles[i].Speed.x * 5, particles[i].Position.y + particles[i].Speed.y * 5);

		graphic.addPolyline(line, ofColor(255.0f, 255.0f, 0.0f, 255.0f), false, true);
	}

	laserManager.beginDraw();
	laserManager.drawLaserGraphic(graphic, 1, renderProfileName);
	laserManager.endDraw();

}

// --------------------------- RealtimeCirclesShowElement ------------------
RealtimeCirclesShowElement::RealtimeCirclesShowElement(string name, string inputdevicename, GraphicSet* graphicsset)
	: RealtimeAudioShowElement(name, inputdevicename, graphicSet)
{

}

void RealtimeCirclesShowElement::setup()
{
	onsetD.setup(OFX_ODS_ODF_RCOMPLEX);
	snd.loadSound("sound/NextStep_CLCEdit_Richard.mp3");
	snd.setLoop(false);
}

void RealtimeCirclesShowElement::setActive(bool a)
{
	ShowElement::setActive(a);

	if (a == false)
		if (playing == true)
		{
			stop();
		}
}

void RealtimeCirclesShowElement::SpaceBarPressed()
{
	if (playing == false)
	{
		start();
	}
	else
	{
		stop();
	}
}

void RealtimeCirclesShowElement::start()
{
	snd.play();
	playing = true;
}

void RealtimeCirclesShowElement::stop()
{
	snd.stop();
	playing = false;
}

void RealtimeCirclesShowElement::update()
{
	pulsing = false;

	// compute FFT
	if (playing)
	{
		if (snd.getIsPlaying())
		{
			float* spectrum = ofSoundGetSpectrum(nBands);

			if (onsetD.isOnsetting(spectrum))
			{
				//musicShader->pulse(0);
				cout << "Onset detected" << endl;

				pulsing = true;
			}
		}
	}
}

void RealtimeCirclesShowElement::draw()
{
}

void RealtimeCirclesShowElement::UpdateGraphic(ofColor color)
{
	ellipseGraphic.clear();

	ofPolyline ellipse;
	int NUM_STEPS = 100;
	float a = 1;
	float b = 1;
	float n = 4;
	float scale = 200;
	//float offsetX = 200;
	//float offsetY = 200;
	for (int i = 0; i < NUM_STEPS; i++)
	{
		float t = i / (float)(NUM_STEPS - 1);
		float tp = t * M_TWO_PI;
		float cost = cos(tp);
		float sint = sin(tp);
		float x = scale * cost;
		float y = scale * sint;

		ellipse.addVertex(x, y, 0);
	}
	ellipse.close();
	//graphic2.connectLineSegments();

	ellipseGraphic.addPolyline(ellipse, color, false, true);
}

void RealtimeCirclesShowElement::drawLaserGraphic(ofxLaser::Manager& laserManager, string renderProfileName)
{
	laserManager.beginDraw();

	for (int i = 0; i < 3; i++)
	{
		ofPushMatrix();

		float scale = 1;
		switch (i)
		{
		case 0:
			break;
		case 1:
			scale = 1.2f;
			break;
		case 2:
			scale = 0.8f;
			break;
		}

		// polylines get projected inside the drawLaserGraphic function, so we need a new set each time!
		ofColor c = ofColor(255.0f, 255.0f, 0.0f, 255.0f);
		if (snd.getIsPlaying())
			c = ofColor::red;

		UpdateGraphic(c);

		ofTranslate(400, 400);

		float s = scale;
		if (pulsing)
			s = scale * 1.2;

		ofScale(s, s);

		float angle = fmod(ofGetElapsedTimef() * 30, 180) - 90;
		ofRotateYDeg(angle);

		laserManager.drawLaserGraphic(ellipseGraphic, 1, renderProfileName);

		ofPopMatrix();

	}
	laserManager.endDraw();
}
