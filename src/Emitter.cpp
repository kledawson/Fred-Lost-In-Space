#include "ofApp.h"
//----------------------------------------------------------------------------------
//
// This example code demonstrates the use of an "Emitter" class to emit Sprites
// and set them in motion. The concept of an "Emitter" is taken from particle
// systems (which we will cover next week).
//
// The Sprite class has also been upgraded to include lifespan, velocity and age
// members.   The emitter can control rate of emission and the current velocity
// of the particles. In this example, there is no acceleration or physics, the
// sprites just move simple frame-based animation.
//
// The code shows a way to attach images to the sprites and optional the
// emitter (which is a point source) can also have an image.  If there are
// no images attached, a placeholder rectangle is drawn.
// Emitters  can be placed anywhere in the window. In this example, you can drag
// it around with the mouse.
//
// OF has an add-in called ofxGUI which is a very simple UI that is useful for
// creating sliders, buttons and fields. It is not recommended for commercial 
// game development, but it is useful for testing.  The "h" key will hide the GUI
// 
// If you want to run this example, you need to use the ofxGUI add-in in your
// setup.
//
//
//  Kevin M. Smith - CS 134 SJSU




//  Add a Sprite to the Sprite System
//
void SpriteList::add(Sprite s) {
	sprites.push_back(s);
}

// Remove a sprite from the sprite system. Note that this function is not currently
// used. The typical case is that sprites automatically get removed when the reach
// their lifespan.
//
void SpriteList::remove(int i) {
	sprites.erase(sprites.begin() + i);
}


//  Update the SpriteSystem by checking which sprites have exceeded their
//  lifespan (and deleting).  Also the sprite is moved to it's next
//  location based on velocity and direction.
//
void SpriteList::update() {

	if (sprites.size() == 0) return;
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sprites.end()) {
		if (s->lifespan != -1 && s->age() > s->lifespan) {
			//			cout << "deleting sprite: " << s->name << endl;
			tmp = sprites.erase(s);
			s = tmp;
		}
		else s++;
	}

	//  Move sprite
	//
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].pos += sprites[i].velocity / ofGetFrameRate();
	}
}

//  Render all the sprites
//
void SpriteList::draw() {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].draw();
	}
}



Emitter::Emitter() {
	sys = new SpriteList();
	init();
}


void Emitter::init() {
	lifespan = 5000;    // default milliseconds
	started = false;

	lastSpawned = 0;
	rate = 0.5;    // sprites/sec
	haveChildImage = false;
	haveImage = false;
	velocity = ofVec3f(100, 100, 0);
	drawable = false;
	width = 50;
	height = 50;
	scale = 1;
	rotationSpeed = 0;
	showImage = true;
	nAgents = 1;
	speedMultiplier = 10.0f;
	maxRotationStep = 1.0f;
	thrustMultiplier = 50.0f;
}



//  Draw the Emitter if it is drawable. In many cases you would want a hidden emitter
//
//
void Emitter::draw() {
	if (!drawable) {
		// set color to white if selected
		if (selected) {
			ofSetColor(255); 
		}
		else {
			ofSetColor(0, 0, 200, 255);
		}

		if (haveImage) {
			image.draw(-image.getWidth() / 2.0 + pos.x, -image.getHeight() / 2.0 + pos.y);
		}
		else {
			ofDrawRectangle(-width / 2 + pos.x, -height / 2 + pos.y, width, height);
		}

	}

	sys->draw();
}


//  Update the Emitter. If it has been started, spawn new sprites with
//  initial velocity, lifespan, birthtime.
//
void Emitter::update() {
	if (!started) return;

	float time = ofGetElapsedTimeMillis();

	if ((time - lastSpawned) > (1000.0 / rate)) {

		// call virtual to spawn a new sprite
		//
		spawnSprite();
		lastSpawned = time;
	}



	// update sprite list
	//
	if (sys->sprites.size() == 0) return;
	vector<Sprite>::iterator s = sys->sprites.begin();
	vector<Sprite>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sys->sprites.end()) {
		if (s->lifespan != -1 && s->age() > s->lifespan) {
			//			cout << "deleting sprite: " << s->name << endl;
			tmp = sys->sprites.erase(s);
			s = tmp;
		}
		else s++;
	}

	
	for (int i = 0; i < sys->sprites.size(); i++) {
		moveSprite(&sys->sprites[i]);
	}
	
}

bool Emitter::checkIfCollided(TriangleShape& triangle, Sprite* sprite) {
	// Check if the position of the sprite lies inside the player's triangle
	return triangle.inside(sprite->pos);
}

// virtual function to move sprite (can be overloaded)
//
//void Emitter::moveSprite(Sprite* sprite) {
//    ofVec3f direction = playerPos - sprite->pos;  // Direction towards the player
//    float length = direction.length();
//
//    if (length > 0) {
//        direction.normalize();  // Normalize the direction vector
//        sprite->velocity = direction * (sprite->velocity.length() * speedMultiplier); 
//
//        // Calculate target angle (in degrees)
//        float targetAngle = ofRadToDeg(atan2(direction.y, direction.x)) + 90;
//
//        // Calculate angle difference
//        float angleDifference = targetAngle - sprite->rotation;
//        angleDifference = fmod(angleDifference + 360, 360);
//
//        if (angleDifference > 180) {
//            angleDifference -= 360;  // Convert to a negative angle
//        }
//
//        if (abs(angleDifference) > maxRotationStep) {
//            // Rotate by the maximum step size, respecting direction
//            sprite->rotation += maxRotationStep * (angleDifference > 0 ? 1 : -1);
//        } else {
//            // Snap to target if within max step
//            sprite->rotation += angleDifference; 
//        }
//
//        sprite->rotation = fmod(sprite->rotation + 360, 360);
//    }
//
//    sprite->pos += sprite->velocity / ofGetFrameRate();
//
//}

void Emitter::moveSprite(Sprite* sprite) {
    ofVec3f direction = playerPos - sprite->pos;  // Direction towards the player
    float distance = direction.length();

	if (distance > 0) {
		direction.normalize();  // Normalize the direction vector
		sprite->velocity += direction * thrustMultiplier; 

		//float maxSpeed = 200.0f; // Set this to your desired max speed
		//if (glm::length(sprite->velocity) > maxSpeed) {
		//	sprite->velocity = glm::normalize(sprite->velocity) * maxSpeed;
		//}

		// Calculate target angle (in degrees)
		float targetAngle = ofRadToDeg(atan2(sprite->velocity.y, sprite->velocity.x)) + 90;

		// Calculate angle difference
		float angleDifference = targetAngle - sprite->rotation;
		angleDifference = fmod(angleDifference + 360, 360);

		if (angleDifference > 180) {
			angleDifference -= 360;  // Convert to a negative angle
		}

		if (abs(angleDifference) > maxRotationStep) {
			// Rotate by the maximum step size, respecting direction
			sprite->rotation += maxRotationStep * (angleDifference > 0 ? 1 : -1);
		}
		else {
			// Snap to target if within max step
			sprite->rotation += angleDifference;
		}

		sprite->rotation = fmod(sprite->rotation + 360, 360);
	}


    // Integrate motion after updating rotation
    sprite->integrate();
}


// virtual function to spawn sprite (can be overloaded)
//
void Emitter::spawnSprite() {
	for (int i = 0; i < nAgents; i++) {
		Sprite sprite;
		if (haveChildImage) {
			sprite.setImage(childImage);


			pos.x = ofRandom(0, ofGetWidth());
			pos.y = ofRandom(0, ofGetHeight());

			sprite.rotation = ofRandom(0, 360);
			sprite.velocity = velocity;
			sprite.lifespan = lifespan;
			sprite.pos = pos;
			sprite.scale = glm::vec3(scale, scale, scale);
			sprite.rotationSpeed = rotationSpeed;
			sprite.birthtime = ofGetElapsedTimeMillis();
			sprite.bShowImage = showImage;
			vector<glm::vec3> vertices = sprite.verts;
			sys->add(sprite);

		}
	}
}


// Start/Stop the emitter.
//
void Emitter::start() {
	started = true;
	lastSpawned = ofGetElapsedTimeMillis();
}

void Emitter::stop() {
	started = false;
}


void Emitter::setLifespan(float life) {
	lifespan = life;
}

void Emitter::setVelocity(const glm::vec3 v) {
	velocity = v;
}

void Emitter::setChildImage(ofImage img) {
	childImage = img;
	haveChildImage = true;
}

void Emitter::setImage(ofImage img) {
	image = img;
}

void Emitter::setRate(float r) {
	rate = r;
}

void Emitter::setScale(float s) {
	scale = s;
}

void Emitter::setRotation(float rs) {
	rotationSpeed = rs;
}

