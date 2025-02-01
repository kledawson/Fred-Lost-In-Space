#pragma once

#include "ofMain.h"
#include "Shape.h"
#include "Sprite.h"

//
//  Manages all Sprites in a system.  You can create multiple systems
//
class SpriteList {
public:
	void add(Sprite);
	void remove(int);
	void update();
	void draw();
	vector<Sprite> sprites;
};


//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
//
class Emitter : public Shape {
public:
	Emitter();
	void init();
	void draw();
	void start();
	void stop();
	void setLifespan(float);
	void setVelocity(const glm::vec3 v);
	void setChildImage(ofImage);
	void setImage(ofImage);
	void setRate(float);
	void setScale(float);
	void setRotation(float);
	void update();

	bool checkIfCollided(TriangleShape& triangle, Sprite* sprite);

	// for single selection of emitter with mouse
	bool selected = false;

	// virtuals - can overloaded
	virtual void moveSprite(Sprite*);
	virtual void spawnSprite();
	void fireParticleRay(const ofVec3f& direction);
	virtual bool inside(glm::vec3 p) {
		glm::vec3 s = glm::inverse(getTransform()) * glm::vec4(p, 1);
		return (s.x > -width / 2 && s.x < width / 2 && s.y > -height / 2 && s.y < height / 2);
	}

	SpriteList* sys;
	float rate;
	glm::vec3 velocity;
	float lifespan;
	bool started;
	float lastSpawned;
	ofImage childImage;
	ofImage image;
	bool drawable;
	bool haveChildImage;
	bool haveImage;
	float width, height;
	bool showImage;
	int nAgents;
	ofVec3f playerPos;
	float speedMultiplier;
	float thrustMultiplier;
	float maxRotationStep;
	vector<glm::vec3> vertices;

	float scale;
	float rotationSpeed;

	float getRate() const { return rate; }
	float getLifespan() const { return lifespan; }
	glm::vec3 getVelocity() const { return velocity; }
	float getScale() const { return scale; }
	float getRotationSpeed() const { return rotationSpeed; }

};

