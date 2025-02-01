#include "ofApp.h"
#include "ofxGui.h"
#include "../Particle.h"
#include "TransformObject.h"


void ofApp::launchParticle() {

	// Create initialize a new particle with values from sliders
	//
	Particle particle;
	particle.color = ofColor::dimGray;

	particle.position = imgShape.pos + glm::vec3(imgShape.getWidth() / 2, imgShape.getHeight() / 2, 1);

	float angleRad = ofDegToRad(imgShape.rotation - 90);
	glm::vec3 facingDirection = glm::vec3(cos(angleRad), sin(angleRad), 0);
	particle.velocity = facingDirection * 600;


	particle.acceleration.set(0, 0, 0);
	particle.damping = 0.99;
	particle.radius = 5;
	particle.lifespan = 2;
	particle.birthtime = ofGetElapsedTimef();


	particles.push_back(particle);

	//cout << "Particle Position: " << particle.position.x << ", "
	//	<< particle.position.y << ", " << particle.position.z << endl;
	//cout << "Particle Velocity: " << particle.velocity.x << ", "
	//	<< particle.velocity.y << ", " << particle.velocity.z << endl;
	//cout << "Particle Acceleration: " << particle.acceleration.x << ", "
	//	<< particle.acceleration.y << ", " << particle.acceleration.z << endl;


	// save your particle here  - you can use an array
	// but make sure to clear() it first as we are using it for 
	// just one particle.


}

void ParticleShape::integrate(float thrustValue) {

	// init current framerate (or you can use ofGetLastFrameTime())
	//
	float framerate = ofGetFrameRate();
	float dt = 1.0 / framerate;

	float angleRad = ofDegToRad(rotation);
	glm::vec3 thrustForce = glm::vec3(cos(angleRad), sin(angleRad), 0 * thrustValue);

	force += thrustForce;
	acceleration = force / mass;

	pos += velocity * dt;
	velocity += acceleration * dt;
	velocity *= damping;

	rotation += angularVelocity * dt;
	angularVelocity += angularAcceleration * dt;

	angularVelocity *= damping;
	angularAcceleration *= damping;

	force = glm::vec3(0, 0, 0);
	acceleration = glm::vec3(0, 0, 0);
}

bool TriangleShape::inside(glm::vec3 p0) {
	glm::vec3 p = glm::inverse(getTransform()) * glm::vec4(p0, 1.0);
	glm::vec3 v1 = glm::normalize(verts[0] - p);
	glm::vec3 v2 = glm::normalize(verts[1] - p);
	glm::vec3 v3 = glm::normalize(verts[2] - p);
	float a1 = glm::orientedAngle(v1, v2, glm::vec3(0, 0, 1));
	float a2 = glm::orientedAngle(v2, v3, glm::vec3(0, 0, 1));
	float a3 = glm::orientedAngle(v3, v1, glm::vec3(0, 0, 1));
	if (a1 < 0 && a2 < 0 && a3 < 0) return true;
	else return false;
}



TriangleShape::TriangleShape() {
	verts.push_back(glm::vec3(50, 50, 0));
	verts.push_back(glm::vec3(0, -100, 0));
	verts.push_back(glm::vec3(-50, 50, 0));
	scale = glm::vec3(1, 1, 1);
	pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0);
	//pos += 50;
}

glm::vec3 TriangleShape::getHeadingVector() {
	glm::vec3 center = (verts[0] + verts[1] + verts[2]) / 3.0f;
	glm::vec3 topVertex = verts[1];
	glm::vec3 headingDirection = glm::normalize(topVertex - center);

	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1));
	glm::vec4 headingVector4 = rotationMatrix * glm::vec4(headingDirection, 0.0);
	glm::vec3 headingVector = glm::vec3(headingVector4);

	return headingVector;
}

void TriangleShape::move(float distance) {
	glm::vec3 headingVector = getHeadingVector();
	glm::vec3 newPos = pos + headingVector * distance;

	// Check if the new position is within the screen bounds
	if (newPos.x >= 0 && newPos.x <= ofGetWindowWidth() && newPos.y >= 0 && newPos.y <= ofGetWindowHeight()) {
		pos = newPos;
	}
}


void TriangleShape::draw(bool showHeadingVector) {
	ofPushMatrix();
	ofMultMatrix(getTransform());

	ofFill();
	ofSetColor(color);
	ofDrawTriangle(verts[0], verts[1], verts[2]);

	if (showHeadingVector) {
		ofSetColor(ofColor::red);
		glm::vec3 center = (verts[0] + verts[1] + verts[2]) / 3.0f;
		glm::vec3 topVertex = verts[1];
		glm::vec3 headingDirection = glm::normalize(topVertex - center);
		glm::vec3 headingLength = center + glm::vec3(0, -128, 0);
		float height = glm::distance(center, headingLength);
		glm::vec3 headingVector = headingDirection * height * 1.2f;
		ofDrawLine(center, center + headingVector);
	}

	ofPopMatrix();
}

ImageShape::ImageShape() {
	loadImage("images/duck128.png");
	//cout << img.getWidth() << endl;
	//cout << img.getHeight() << endl;

	verts.push_back(glm::vec3(-img.getWidth() / 2.0, img.getHeight() / 2.0, 1));
	verts.push_back(glm::vec3(img.getWidth() / 2.0, img.getHeight() / 2.0, 1));
	verts.push_back(glm::vec3(img.getWidth() / 2.0, -img.getHeight() / 2.0, 1));
	verts.push_back(glm::vec3(-img.getWidth() / 2.0, -img.getHeight() / 2.0, 1));
	scale = glm::vec3(1, 1, 1);

	glm::vec3 imageSize = glm::vec3(img.getWidth(), img.getHeight(), 1);
	glm::vec3 screenCenter = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 1);
	pos = screenCenter - (imageSize / 2.0f);
}


void ImageShape::loadImage(const std::string& filePath) {
	if (img.load(filePath)) {
		//cout << "Image loaded: " << filePath << endl;
	}
	else {
		cout << "Failed to load image: " << filePath << endl;
	}
}

glm::vec3 ImageShape::getHeadingVector() {

	glm::vec3 center = glm::vec3(img.getWidth() / 2.0, img.getHeight() / 2.0, 1);
	glm::vec3 topPoint = glm::vec3(img.getWidth() / 2, 0, 1); 
	glm::vec3 headingDirection = glm::normalize(topPoint - center);

	// Apply the current rotation to the heading vector
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1)); // bugfix
	glm::vec4 headingVector4 = rotationMatrix * glm::vec4(headingDirection, 0.0);
	glm::vec3 headingVector = glm::vec3(headingVector4);

	return headingVector;
}

void ImageShape::move(float distance) {
	glm::vec3 headingVector = getHeadingVector();
	glm::vec3 newPos = pos + headingVector * distance;

	float halfWidth = img.getWidth() / 2.0f;
	float halfHeight = img.getHeight() / 2.0f;

	// bounds imgShape to screen size
	if (newPos.x + halfWidth >= 0 && newPos.x - halfWidth <= ofGetWindowWidth() - img.getWidth() &&
		newPos.y + halfHeight >= 0 && newPos.y - halfHeight <= ofGetWindowHeight() - img.getHeight()) {
		pos = newPos;
	}
}

bool ImageShape::inside(glm::vec3 p0) {
	glm::vec3 p = glm::inverse(getTransformImage()) * glm::vec4(p0, 1.0);

	float minX = verts[0].x;
	float maxX = verts[2].x;
	float minY = verts[2].y;
	float maxY = verts[0].y;

	// Return true if the point is within the bounds of the rectangle
	return (p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY);
}

void ImageShape::draw(bool showHeadingVector) {
	if (img.isAllocated()) {
		ofPushMatrix();

		ofMultMatrix(getTransformImage());
		ofSetColor(ofColor::white);
		img.draw(0, 0);

		if (showHeadingVector) {
			ofSetColor(ofColor::red);
			glm::vec3 center = glm::vec3(img.getWidth() / 2.0, img.getHeight() / 2.0, 1);
			glm::vec3 topPoint = glm::vec3(img.getWidth() / 2, 0, 1);
			glm::vec3 headingDirection = glm::normalize(topPoint - center);
			glm::vec3 headingLength = center + glm::vec3(0, -128, 0);
			float height = glm::distance(center, headingLength);
			glm::vec3 headingVector = headingDirection * height * 1.2f;
			ofDrawLine(center, center + headingVector);
		}

		ofPopMatrix();
	}
}



// Example on how to use GLM to return a transform based on component matrices
//
glm::mat4 Shape::getTransform() {
	glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(pos));
	glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1));
	glm::mat4 scale = glm::scale(glm::mat4(1.0), this->scale);
	return (trans * rot * scale);
}

glm::mat4 ImageShape::getTransformImage() {
	// Translate to the position of the image
	glm::mat4 trans = glm::translate(glm::mat4(1.0), pos);

	// Translate to the center of the image
	glm::mat4 centerTranslation = glm::translate(glm::mat4(1.0), glm::vec3(img.getWidth() / 2.0, img.getHeight() / 2.0, 0));

	// Rotate around the z-axis
	glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1));

	// Translate back from the center of the image
	glm::mat4 reverseTranslation = glm::translate(glm::mat4(1.0), glm::vec3(-img.getWidth() / 2.0, -img.getHeight() / 2.0, 0));

	// Create a scaling matrix from the scale property (now a mat4)
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(scale)); // Assuming scale is still a glm::vec3

	// Combine the transformations: first translate to center, then rotate, then scale, then translate back
	return trans * centerTranslation * rot * scaleMatrix * reverseTranslation;
}


//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60);
	ofSetVerticalSync(true);

	nEnergy = 10;
	totalEnergyLevels = 10;
	elapsedTime = 0.0f;
	gameStartTime = ofGetElapsedTimef();
	isGameOver = false;

	thrustSound.load("sounds/thrusters-loop.wav");
	thrustSound.setVolume(0.6);
	thrustSound.setLoop(true);

	quackSound.load("sounds/quack-loop.wav");
	quackSound.setVolume(0.4);
	quackSound.setLoop(false);
	
	explosionSound.load("sounds/explosion-loop.wav");
	explosionSound.setVolume(0.4);
	explosionSound.setLoop(false);

	explosion2Sound.load("sounds/explosion2-loop.wav");
	explosion2Sound.setVolume(0.5);
	explosion2Sound.setLoop(false);

	alertSound.load("sounds/alert-loop.wav");
	alertSound.setVolume(0.75);
	alertSound.setLoop(false);

	boopSound.load("sounds/boop-loop.wav");
	boopSound.setVolume(1.00);
	boopSound.setLoop(false);

	title.load("fonts/star-and-moon.ttf", 48);

	desc.load("fonts/star-and-moon.ttf", 24);

	// load image file
	if (defaultImage.load("images/space-invaders-ship-scaled.png")) {
		imageLoaded = true;
	}
	else {
		cout << "Can't open image file" << endl;
		ofExit();
	}

	backgroundImage.load("images/Space.png");

	//ofSetBackgroundColor(ofColor::black);

	// creating 3 emitters
	for (int i = 0; i < 1; i++) {
		Emitter emitter;
		emitter.pos = glm::vec3(ofRandomWidth(), ofRandomHeight(), 0);
		emitter.drawable = true;
		if (imageLoaded) emitter.setChildImage(defaultImage);
		emitter.start();
		emitters.push_back(emitter);
	}

	
	gui.setup("Control Panel");
	Emitters.setup("Emitter Controls");
	Player.setup("Player Controls");
	Agent.setup("Agent Controls");

	Emitters.add(level.setup("Select Level", 1, 0, 3));
	Emitters.add(rate.setup("Rate", 0.5, 0.5, 10));
	Emitters.add(life.setup("Life", 5, 0.1, 10));
	Emitters.add(toggleEmitter.setup("Show Emitter", true));
	Emitters.add(velocity.setup("Emitter Velocity", ofVec3f(100, 100, 0), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));
	Emitters.add(scale.setup("Scale", 1, 0.1, 2.0));
	Emitters.add(rotationSpeed.setup("Rotation Speed (deg/Frame)", 0, 0, 10));
	Emitters.add(toggleImage.setup("Toggle Emitter Image", true));
	gui.add(&Emitters);

	Agent.add(agentCount.setup("Agent Count", 1, 0, 10));
	//Agent.add(agentSpeed.setup("Agent Speed", 10, 0, 25));
	Agent.add(agentThrust.setup("Agent Thrust", 100, 0, 1000));
	Agent.add(agentRotation.setup("Agent Rotation Speed", 1, 0, 10));
	gui.add(&Agent);

	Player.add(showHeadingVector.setup("Show Heading Vector", false));
	Player.add(useImageShape.setup("Use Image Shape", true));
	Player.add(scaleSlider.setup("Scale", 1.0, 0.0, 10.0));
	//Player.add(speedSlider.setup("Speed", 3.0f, 1.0f, 10.0f));
	Player.add(thrust.setup("Thrust", 500, 100, 2000));
	Player.add(rotationSpeedSlider.setup("Rotation Speed", 2.0f, 1.0f, 10.0f));
	gui.add(&Player);

	gui.setPosition(20, 100);

	toggleEmitter.addListener(this, &ofApp::onToggleEmitter);
	toggleImage.addListener(this, &ofApp::onToggleImage);
	agentCount.addListener(this, &ofApp::onAgentCountChange);
	agentSpeed.addListener(this, &ofApp::onAgentSpeedChange);
	agentThrust.addListener(this, &ofApp::onAgentThrustChange);
	agentRotation.addListener(this, &ofApp::onAgentRotationChange);

	showHeadingVector.addListener(this, &ofApp::onToggleHeadingVector);
	useImageShape.addListener(this, &ofApp::onToggleShape);
	scaleSlider.addListener(this, &ofApp::onScaleChange);

	imgShape.loadImage("images/duck128.png");


	turbForce = new TurbulenceForce(ofVec3f(-100, -100, -100), ofVec3f(100, 100, 100));
	gravityForce = new GravityForce(ofVec3f(0, -10, 0));
	radialForce = new ImpulseRadialForce(3000.0);

	explosion.sys->addForce(turbForce);
	explosion.sys->addForce(gravityForce);
	explosion.sys->addForce(radialForce);

	explosion.setVelocity(ofVec3f(30, 30, 0));
	explosion.setOneShot(true);
	explosion.setEmitterType(RadialEmitter);
	explosion.setGroupSize(250);
	explosion.setParticleRadius(1);
	explosion.setLifespan(2);

	//std::cout << "Emitter Position: "
	//	<< explosion.position.x << ", "
	//	<< explosion.position.y << ", "
	//	<< explosion.position.z << std::endl;

	bHide = false;
	showTitle = true;
}

void ofApp::restartGame() {
	nEnergy = totalEnergyLevels; 
	elapsedTime = 0.0f; 
	isGameOver = false; 
	isGameRunning = false; 
	emitters.clear(); 

	for (int i = 0; i < 1; i++) {
		Emitter emitter;
		emitter.pos = glm::vec3(ofRandomWidth(), ofRandomHeight(), 0);
		emitter.drawable = true;
		if (imageLoaded) emitter.setChildImage(defaultImage);
		emitter.start();
		emitters.push_back(emitter);
	}

	rate = 0.5;
	life = 5.0;
	velocity = ofVec3f(0, -200, 0);
	scale = 0.1;
	rotationSpeed = 0;
	agentCount = 1;
	agentSpeed = 5;
	agentRotation = 1;
	speedSlider = 2.0f;
	rotationSpeedSlider - 2.0f;

	gui.clear();

	gui.setup();
	Emitters.setup("Emitter Controls");
	Player.setup("Player Controls");
	Agent.setup("Agent Controls");

	Emitters.add(level.setup("Select Level", 1, 0, 3));
	Emitters.add(rate.setup("Rate", 0.5, 0.5, 10));
	Emitters.add(life.setup("Life", 5, 0.1, 10));
	Emitters.add(toggleEmitter.setup("Show Emitter", true));
	Emitters.add(velocity.setup("Velocity", ofVec3f(100, 100, 0), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));
	Emitters.add(scale.setup("Scale", 1, 0.1, 2.0));
	Emitters.add(rotationSpeed.setup("Rotation Speed (deg/Frame)", 0, 0, 10));
	Emitters.add(toggleImage.setup("Toggle Emitter Image", true));
	gui.add(&Emitters);

	Agent.add(agentCount.setup("Agent Count", 1, 0, 10));
	//Agent.add(agentSpeed.setup("Agent Speed", 10, 0, 25));
	Agent.add(agentThrust.setup("Agent Thrust", 100, 0, 1000));
	Agent.add(agentRotation.setup("Agent Rotation Speed", 1, 0, 10));
	gui.add(&Agent);

	Player.add(showHeadingVector.setup("Show Heading Vector", true));
	Player.add(useImageShape.setup("Use Image Shape", true));
	Player.add(scaleSlider.setup("Scale", 1.0, 0.0, 10.0));
	//Player.add(speedSlider.setup("Speed", 3.0f, 1.0f, 10.0f));
	Player.add(thrust.setup("Thrust", 500, 100, 2000));
	Player.add(rotationSpeedSlider.setup("Rotation Speed", 2.0f, 1.0f, 10.0f));
	gui.add(&Player);

	gui.setPosition(20, 100);

	tri.pos = glm::vec3(ofGetWindowWidth() / 2 - 64, ofGetWindowHeight() / 2 - 64, 0);
	imgShape.pos = glm::vec3(ofGetWindowWidth() / 2 - 64, ofGetWindowHeight() / 2 - 64, 0);

	tri.scale = glm::vec3(1.0, 1.0, 1.0);
	imgShape.scale = glm::vec3(1.0, 1.0, 1.0);
	tri.rotation = 0.0f;
	imgShape.rotation = 0.0f;

	bHide = false;
	showTitle = true;
}

void ofApp::onToggleEmitter(bool& draw)
{
	for (auto& emitter : emitters) {
		emitter.drawable = draw;
	}
}

void ofApp::onToggleHeadingVector(bool& show) {
	if (show) {
		cout << "Heading vector is now ON." << endl;
	}
	else {
		cout << "Heading vector is now OFF." << endl;
	}
}

void ofApp::onToggleShape(bool& useImage) {
	if (useImage) {
		cout << "Using Image Shape." << endl;
	}
	else {
		cout << "Using Triangle Shape." << endl;
	}
}

void ofApp::onScaleChange(float& scale) {
	imgShape.scale = glm::vec3(scale, scale, 1.0);
	tri.scale = glm::vec3(scale, scale, 1.0);
}

void ofApp::onToggleImage(bool& useImage)
{
	for (auto& emitter : emitters) {
		emitter.showImage = useImage;
	}
}

void ofApp::onAgentCountChange(int& count)
{
	for (auto& emitter : emitters) {
		emitter.nAgents = agentCount;
	}
}

void ofApp::onAgentSpeedChange(float& speed)
{
	for (auto& emitter : emitters) {
		emitter.speedMultiplier = agentSpeed;
	}
}

void ofApp::onAgentThrustChange(float& thrust)
{
	for (auto& emitter : emitters) {
		emitter.thrustMultiplier = agentThrust;
	}
}

void ofApp::onAgentRotationChange(float& rotation)
{
	for (auto& emitter : emitters) {
		emitter.maxRotationStep = agentRotation;
	}
}

void ofApp::exit() {
	showHeadingVector.removeListener(this, &ofApp::onToggleHeadingVector);
	useImageShape.removeListener(this, &ofApp::onToggleShape);
}

bool ofApp::checkCollisionWithPlayer(Sprite* sprite) {
	return tri.inside(sprite->pos);
}

void ofApp::checkPebbleCollision() {
	for (int i = 0; i < particles.size(); i++) {
		glm::vec3 p1 = particles[i].position;
//		cout << "bullet pos: " << particles[i].position << endl;

		for (int j = 0; j < emitters[0].sys->sprites.size(); j++) {
//			cout << "sprite pos: " << emitters[0].sys->sprites[j].pos << endl;
			glm::vec3 p2 = emitters[0].sys->sprites[j].pos;

			if (glm::distance(p1, p2) < 60) {
				nEnergy += 1;
				emitters[0].sys->sprites.erase(emitters[0].sys->sprites.begin() + j);
				explosion.setPosition(p2);
				explosion.sys->reset();
				explosion.start();
				explosion2Sound.play();
				alertSound.play();
				break;
			}
			//	cout << "HIT !!!!" << endl;
			//else {
			//	cout << "MISS !!!" << endl;
			//}
		}
	}
}


void ofApp::update() {

	if (level != selectedLevel) {
		selectedLevel = level;

		// Apply settings based on the new level
		switch (selectedLevel) {
		case 0:
			break;
		case 1:
			rate = 0.5;
			life = 5;
			agentCount = 1;
			agentSpeed = 10;
			agentThrust = 50;
			agentRotation = 1;
			thrust = 500;
			rotationSpeedSlider = 3.0f;
			cout << "Level 1 selected" << endl;
			break;
		case 2:
			rate = 1.0;
			life = 7.5;
			agentCount = 2;
			agentSpeed = 15;
			agentThrust = 100;
			agentRotation = 2;
			thrust = 750;
			rotationSpeedSlider = 4.0f;
			cout << "Level 2 selected" << endl;
			break;
		case 3:
			rate = 1.5;
			life = 10;
			agentCount = 3;
			agentSpeed = 20;
			agentThrust = 150;
			agentRotation = 3;
			thrust = 1000;
			rotationSpeedSlider = 5.0f;
			cout << "Level 3 selected" << endl;
			break;
		}
	}



	if (isGameRunning && !isGameOver) {
		ofSeedRandom();
		float sliderThrust = thrust;
		float angleRad = ofDegToRad(tri.rotation - 90);
		glm::vec3 facingDirection = glm::vec3(cos(angleRad), sin(angleRad), 0);
		//if (keyStates[OF_KEY_UP]) {
		//	tri.move(speedSlider);
		//	imgShape.move(speedSlider);
		//}
		//if (keyStates[OF_KEY_DOWN]) {
		//	tri.move(-speedSlider);
		//	imgShape.move(-speedSlider);
		//}
		//if (keyStates[OF_KEY_LEFT]) {
		//	tri.rotation -= rotationSpeedSlider;
		//	imgShape.rotation -= rotationSpeedSlider;
		//}
		//if (keyStates[OF_KEY_RIGHT]) {
		//	tri.rotation += rotationSpeedSlider;
		//	imgShape.rotation += rotationSpeedSlider;
		//}
		if (keyStates['a']) {
			tri.angularVelocity -= sliderThrust * 0.01;
			imgShape.angularVelocity -= sliderThrust * 0.01;
		}
		if (keyStates['d']) {
			tri.angularVelocity += sliderThrust * 0.01;
			imgShape.angularVelocity += sliderThrust * 0.01;
		}
		if (keyStates['w']) {
			tri.force += facingDirection * sliderThrust;
			imgShape.force += facingDirection * sliderThrust;
		}
		if (keyStates['s']) {
			tri.force -= facingDirection * sliderThrust;
			imgShape.force -= facingDirection * sliderThrust;
		}
		if (keyStates[' ']) {
			float currentTime = ofGetElapsedTimef();

			if (keyStates[' ']) {
				if (currentTime - lastParticleLaunchTime >= cooldownDuration) {
					launchParticle(); 
					boopSound.play();
					lastParticleLaunchTime = currentTime;
				}
			}
		}

		tri.integrate(thrust);
		imgShape.integrate(thrust);


		// Calculate triangle bounding box based on its vertices
		float triangleMinX = std::min({ tri.verts[0].x, tri.verts[1].x, tri.verts[2].x });
		float triangleMaxX = std::max({ tri.verts[0].x, tri.verts[1].x, tri.verts[2].x });
		float triangleMinY = std::min({ tri.verts[0].y, tri.verts[1].y, tri.verts[2].y });
		float triangleMaxY = std::max({ tri.verts[0].y, tri.verts[1].y, tri.verts[2].y });

		// Clamp triangle's position within screen bounds
		float triangleWidth = triangleMaxX - triangleMinX;
		float triangleHeight = triangleMaxY - triangleMinY;
		float screenWidth = ofGetWindowWidth();
		float screenHeight = ofGetWindowHeight();

		tri.pos.x = std::max(0.0f, std::min(tri.pos.x, screenWidth));
		tri.pos.y = std::max(0.0f, std::min(tri.pos.y, screenHeight));

		// Check bounds for imgShape
		float imgHalfWidth = imgShape.img.getWidth() / 2.0f;
		float imgHalfHeight = imgShape.img.getHeight() / 2.0f;

		// Calculate current center position of the image
		float imgCenterX = imgShape.pos.x + imgHalfWidth;
		float imgCenterY = imgShape.pos.y + imgHalfHeight;

		// Clamp imgShape's center position within screen bounds
		imgCenterX = std::max(0.0f, std::min(imgCenterX, screenWidth));
		imgCenterY = std::max(0.0f, std::min(imgCenterY, screenHeight));

		// Update imgShape's position to be centered
		imgShape.pos.x = imgCenterX - imgHalfWidth;
		imgShape.pos.y = imgCenterY - imgHalfHeight;

		elapsedTime += ofGetLastFrameTime();
		frameRate = ofGetFrameRate();

		for (int i = 0; i < emitters.size(); i++) {
			if (emitters[i].selected) {
				emitters[i].setRate(rate);
				emitters[i].setLifespan(life * 1000);
				emitters[i].setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));

				emitters[i].scale = scale;
				emitters[i].rotationSpeed = rotationSpeed;

			}

			emitters[i].update();
		}

		// collision detection for triangle and agent
		for (int j = 0; j < emitters[0].sys->sprites.size(); j++) {
			if (checkCollisionWithPlayer(&emitters[0].sys->sprites[j])) {
				emitters[0].sys->sprites.erase(emitters[0].sys->sprites.begin() + j); 
				nEnergy -= 1;

				ofVec3f imagePosition = ofVec3f(imgShape.pos.x + imgShape.getWidth() / 2, imgShape.pos.y + imgShape.getHeight() / 2, 1);
				explosion.setPosition(imagePosition);

				explosion.sys->reset();
				explosion.start();
				explosionSound.play();
				quackSound.play();
			}
			else {
				j++;
			}
		}

		checkPebbleCollision();


		glm::vec3 triangleCenter = (tri.verts[0] + tri.verts[1] + tri.verts[2]) / 3.0f;
		playerPos = glm::vec3(triangleCenter.x + tri.pos.x, triangleCenter.y + tri.pos.y, 0);

		if (useImageShape) {
			glm::vec3 imageCenter = glm::vec3(imgShape.pos.x + imgShape.img.getWidth() / 2.0, imgShape.pos.y + imgShape.img.getHeight() / 2.0, 0);
			playerPos = imageCenter; 
		}

		for (auto& emitter : emitters) {
			emitter.playerPos = playerPos;
		}

		for (auto& particle : particles) {
			particle.integrate();
		}

		explosion.update();

		//remove particle when lifespan over
		auto it = particles.begin();
		while (it != particles.end()) {
			if (ofGetElapsedTimef() - it->birthtime > it->lifespan) {
				it = particles.erase(it);
			}
			else {
				++it;
			}
		}

		//if (useImageShape) {
		//	imgShape.integrate(thrust);
		//}
		//else {
		//	tri.integrate(thrust);
		//}

		if (nEnergy <= 0) {
			isGameOver = true;
			showTitle = false;
			isGameRunning = false;
		}

	}
}

void ofApp::startGame() {
	isGameRunning = true;
	showTitle = false;
	isGameOver = false;
}

//--------------------------------------------------------------
void ofApp::draw() {
	if (!isGameRunning && !isGameOver) {
		ofBackground(0);

		ofSetColor(ofColor::lightYellow);

		// Title
		string titleText = "Fred: Lost In Space";
		float titleWidth = title.stringWidth(titleText);
		float titleX = (ofGetWidth() - titleWidth) / 2;
		title.drawString(titleText, titleX, ofGetHeight() / 2 - 60);

		ofSetColor(ofColor::mediumPurple);

		string dividerText = "-------------------------";
		float dividerWidth = title.stringWidth(dividerText);
		float dividerX = (ofGetWidth() - dividerWidth) / 2;
		title.drawString(dividerText, dividerX, ofGetHeight() / 2);

		ofSetColor(ofColor::cornflowerBlue);

		// Description
		string descText = "Press SPACEBAR to begin";
		float descWidth = desc.stringWidth(descText);
		float descX = (ofGetWidth() - descWidth) / 2;
		desc.drawString(descText, descX, ofGetHeight() / 2 + 60);

		ofSetColor(ofColor::white);

		ofDrawBitmapString("Keybinds: \n WASD = Movement\n SPACEBAR = Throw Pebble\n F = Fullscreen\n H = Settings\n T = Take Damage\n B = Test Explosion\n U = Show Heading Vector\n I = Toggle Agent Sprite\n O = Toggle Player Sprite\n P = Show Agent Emitter", 20, 30);

		if (bHide) {
			gui.draw();
		}
	}

	else if (isGameRunning) {
		ofSetColor(255);
		backgroundImage.draw(0, 0, ofGetWidth(), ofGetHeight());
		ofSetColor(255);

		ofDrawBitmapString("Position: " + ofToString(tri.pos), 300, 30);

		ofDrawBitmapString("Velocity: " + ofToString(tri.velocity), 300, 50);

		ofDrawBitmapString("Current Level: " + ofToString(selectedLevel) + " / " + "3", 20, 30);

		ofDrawBitmapString("Current Energy Level: " + ofToString(nEnergy) + " / " + ofToString(totalEnergyLevels), 20, 50);

		ofDrawBitmapString("Elapsed Time: " + ofToString(elapsedTime, 2) + " seconds", 20, 70);

		ofDrawBitmapString("Frame Rate: " + ofToString(frameRate, 2) + " FPS", 20, 90);

		for (auto& emitter : emitters) {
			emitter.draw();
		}

		for (auto& particle : particles) {
			particle.draw();
		}

		explosion.draw();

		if (useImageShape) {
			imgShape.draw(showHeadingVector);
		}
		else {
			tri.draw(showHeadingVector);
		}

		//DRAW PLAYER CENTER
		//ofSetColor(ofColor::red);
		//ofDrawCircle(playerPos, 5);

		if (bHide) {
			gui.draw();
		}
	}

	if (isGameOver) {
		ofSetColor(255, 0, 0);

		string gameOverText = "Game Over! Total Time: " + ofToString(elapsedTime, 2) + " seconds";
		float gameOverWidth = title.stringWidth(gameOverText);
		float gameOverX = (ofGetWidth() - gameOverWidth) / 2;
		title.drawString(gameOverText, gameOverX, ofGetHeight() / 2 - 60);

		ofSetColor(ofColor::whiteSmoke);

		string dividerText = "------------------------------";
		float dividerWidth = title.stringWidth(dividerText);
		float dividerX = (ofGetWidth() - dividerWidth) / 2;
		title.drawString(dividerText, dividerX, ofGetHeight() / 2);

		ofSetColor(ofColor::midnightBlue);

		string restartText = "Press R to restart";
		float restartWidth = desc.stringWidth(restartText);
		float restartX = (ofGetWidth() - restartWidth) / 2;
		desc.drawString(restartText, restartX, ofGetHeight() / 2 + 50);
	}
}

//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
//	cout << "mouse( " << x << "," << y << ")" << endl;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	glm::vec3 pos = glm::vec3(x, y, 0);

	if (inDrag) {
		glm::vec3 delta = pos - mousePrevPos;
		tri.pos += delta;
		imgShape.pos += delta;
		circlePos += delta;
		mousePrevPos = pos;
		cout << "Dragging ";
	}

	if (selectedEmitter != nullptr) {
		selectedEmitter->pos = glm::vec3(x, y, 0) + dragOffset; // dragging emitter with mouse
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	glm::vec3 mousePos = glm::vec3(x, y, 0);
	inDrag = insideCircle(mousePos) || tri.inside(mousePos) || imgShape.inside(mousePos);

	if (inDrag) {
		mousePrevPos = mousePos;
	}

    bool clickedOutside = true;

    // deselect previous emitter
    if (selectedEmitter != nullptr) {
        selectedEmitter->selected = false;
    }
	// manages selected emitter
    for (auto& emitter : emitters) {
        if (emitter.drawable && ofDist(x, y, emitter.pos.x, emitter.pos.y) < emitter.width) {
            emitter.selected = true;
            selectedEmitter = &emitter;
            dragOffset = emitter.pos - glm::vec3(x, y, 0);

			rate = emitter.getRate();
			life = emitter.getLifespan() / 1000.0;
			velocity = emitter.getVelocity();
			scale = emitter.getScale();
			rotationSpeed = emitter.getRotationSpeed();

			// ends loop if clicked outside
            clickedOutside = false; 
            break;
        }
    }
	if (clickedOutside) {
		selectedEmitter = nullptr;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

void ofApp::keyPressed(int key) {
	keyStates[key] = true;
	float sliderThrust = thrust;
	ofVec3f centerScreen(800, 450, 0);
	switch (key) {
		// Movement and rotation controls
	case ' ':
		if (!isGameRunning && !isGameOver) {
			startGame();
		}
		break;
	//case OF_KEY_UP:
	//	tri.move(speedSlider);
	//	imgShape.move(speedSlider);
	//	break;
	//case OF_KEY_DOWN:
	//	tri.move(-speedSlider);
	//	imgShape.move(-speedSlider);
	//	break;
	//case OF_KEY_LEFT:
	//	tri.rotation += -rotationSpeedSlider;
	//	imgShape.rotation += -rotationSpeedSlider;
	//	break;
	//case OF_KEY_RIGHT:
	//	tri.rotation += rotationSpeedSlider;
	//	imgShape.rotation += rotationSpeedSlider;
	//	break;
	//case OF_KEY_LEFT:   // turn left
	//	if (isGameRunning) {
	//		tri.angularVelocity -= sliderThrust * 0.01;
	//		imgShape.angularVelocity -= sliderThrust * 0.01;
	//	}
	//	break;
	//case OF_KEY_RIGHT:  // turn right
	//	if (isGameRunning) {
	//		tri.angularVelocity += sliderThrust * 0.01;
	//		imgShape.angularVelocity += sliderThrust * 0.01;
	//	}
		break;
	case 'a':   // turn left
		if (!thrustSound.isPlaying()) {
			thrustSound.play();
		}
		break;
	case 'd':  // turn right
		if (!thrustSound.isPlaying()) {
			thrustSound.play();
		}
		break;
	case 'w':  // go forward 
		if (!thrustSound.isPlaying()) {
			thrustSound.play();
		}
		break;
	case 's':  // go backward
		if (!thrustSound.isPlaying()) {
			thrustSound.play();
		}
		break;
	case 'B':
	case 'b':
		explosion.setPosition(centerScreen);
		explosion.sys->reset();
		explosion.start();
		explosionSound.play();
		break;
	case 'R':
	case 'r':
		if (isGameOver) {
			restartGame();
		}
		break;
	case 'T':
	case 't':
		nEnergy--;
		quackSound.play();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		bHide = !bHide;
		break;
	case 'U':
	case 'u':
		showHeadingVector = !showHeadingVector;
		break;
	case 'I':
	case 'i':
		for (auto& emitter : emitters) {
			emitter.showImage = !emitter.showImage;
		}
		break;
	case 'O':
	case 'o':
		useImageShape = !useImageShape;
		break;
	case 'P':
	case 'p':
		emitters[0].drawable = !emitters[0].drawable;
		break;
	default:
		break;
	}
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	//switch (key) {
	//case OF_KEY_LEFT:
	//case OF_KEY_RIGHT:
	//case OF_KEY_UP:
	//case OF_KEY_DOWN:
	//	break;
	//case OF_KEY_ALT:
	//	break;
	//case OF_KEY_CONTROL:
	//	break;
	//case OF_KEY_SHIFT:
	//	break;
	//}
	switch (key) {
	case 'a':   // turn left
		thrustSound.stop();
		break;
	case 'd':  // turn right
		thrustSound.stop();
		break;
	case 'w':
		thrustSound.stop(); // go forward
		break;
	case 's':
		thrustSound.stop(); // go backward
		break;
	default:
		break;
	}
	keyStates[key] = false;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

