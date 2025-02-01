
#include "Particle.h"

Particle::Particle() {

	// initialize particle with some reasonable values first;
	//
	velocity.set(0, 0, 0);
	acceleration.set(0, 0, 0);
	position.set(0, 0, 0);
	lifespan = 5;
	birthtime = 0;
	radius = .5;
	damping = .99;
	mass = 1;
	color = ofColor::lightGoldenRodYellow;
}

float Particle::age() {
	return (ofGetElapsedTimeMillis() - birthtime) / 1000.0;
}

void Particle::draw() {

	ofSetColor(color);
	ofDrawSphere(position, radius);
}

// write your own integrator here.. (hint: it's only 3 lines of code)
//
void Particle::integrate() {
	float dt = 1.0 / ofGetFrameRate();

	// (1) Update position using velocity and time interval
	position += velocity * dt;

	// (2) Update velocity using acceleration
	velocity += acceleration * dt;

	// (3) Apply damping to simulate drag
	velocity *= damping;
}
