#pragma once

#include "ofMain.h"


// Basic Shape class supporting matrix transformations and drawing.
// 
//

class Shape {
public:
    glm::mat4 getTransform();
    // transformations
    //
    glm::vec3 pos = glm::vec3(0, 0, 0);
    float rotation = 0;
    glm::vec3 scale = glm::vec3(1, 1, 1);
    // methods of shape
    //
    virtual bool inside(glm::vec3 p) { return false; }
    virtual ofColor getColor() { return color; }
    // OF methods
    //

	float defaultSize = 20.0;

	virtual void draw() {

		// draw a box by defaultd if not overridden
		//
		ofPushMatrix();
		ofMultMatrix(getTransform());
		ofDrawBox(defaultSize);
		ofPopMatrix();
	}
    ofColor color = ofColor::blue;

};

class ParticleShape : public Shape {
public:
    glm::vec3 velocity = glm::vec3(0, 0, 0);
    glm::vec3 acceleration = glm::vec3(0, 0, 0);
    glm::vec3 force = glm::vec3(0, 0, 0);
    float angularVelocity;
    float angularAcceleration;
    float mass = 1.0;
    float damping = .97;

    void integrate(float thrustValue);
};

// Triangle Shape
class TriangleShape : public ParticleShape {
public:
    TriangleShape();
    bool inside(glm::vec3 p);
    void draw(bool showHeadingVector);
    vector<glm::vec3> verts;

    void move(float distance);
    glm::vec3 getHeadingVector();
};

// Imported Image
class ImageShape : public ParticleShape {
public:
    ImageShape();
    glm::mat4 getTransformImage();
    void loadImage(const std::string& filePath);
    void draw(bool showHeadingVector);
    void move(float distance);
    glm::vec3 getHeadingVector();
    float getWidth() const {
        return img.getWidth();
    }

    // Method to get the height of the image
    float getHeight() const {
        return img.getHeight();
    }
    bool inside(glm::vec3 p0);
    vector<glm::vec3> verts;

    ofImage img; // Image to be displayed
};
