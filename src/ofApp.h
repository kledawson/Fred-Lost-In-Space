#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Emitter.h"
#include "Shape.h"
#include <unordered_map>
#include "ParticleEmitter.h"
#include "TransformObject.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void restartGame();
    void update();
    void draw();
    void exit();
    bool checkCollisionWithPlayer(Sprite* sprite);
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    // Particles
    vector<Particle> particles;
    void launchParticle();
    float lastParticleLaunchTime = 0.0f;
    float cooldownDuration = 0.75f;

    ParticleEmitter explosion;
    TurbulenceForce* turbForce;
    GravityForce* gravityForce;
    ImpulseRadialForce* radialForce;
    void checkPebbleCollision();

    // Emitters
    std::vector<Emitter> emitters;
    Emitter* emitter = nullptr;
    Emitter* selectedEmitter = nullptr;
    glm::vec2 dragOffset;
    bool dragging = false;

    // Image-related variables
    ofImage defaultImage; // alien sprite
    bool imageLoaded = false;

    // Shape objects
    TriangleShape tri;
    ImageShape imgShape; //duck

    //Circle-related variables
    glm::vec3 circlePos = glm::vec3(500, 350, 0);
    float circleRadius = 100;
    bool inDrag = false;
    bool insideCircle(glm::vec3 p) {
        return (glm::length(p - circlePos) < circleRadius);
    }

    ofImage backgroundImage;

    bool isGameRunning = false;
    bool showTitle = true;
    void startGame();

    std::map<int, bool> keyMap;
    int nEnergy;
    int totalEnergyLevels;
    ofVec3f playerPos;
    float elapsedTime;
    float frameRate;
    int selectedLevel = 1;

    float gameStartTime; 
    bool isGameOver;

    // Gui elements
    ofxPanel gui;
    ofxGuiGroup Emitters;
    ofxGuiGroup Player;
    ofxGuiGroup Agent;

    ofxIntSlider level;
    ofxFloatSlider rate;
    ofxFloatSlider life;
    ofxToggle toggleEmitter;
    ofxVec3Slider velocity;
    ofxLabel screenSize;
    ofxFloatSlider scale;
    ofxFloatSlider rotationSpeed;
    ofxToggle toggleImage;
    ofxIntSlider agentCount;
    ofxFloatSlider agentSpeed;
    ofxFloatSlider agentThrust;
    ofxFloatSlider agentRotation;

    ofxToggle showHeadingVector;
    ofxToggle useImageShape;
    ofxFloatSlider scaleSlider;
    ofxFloatSlider speedSlider;
    ofxFloatSlider thrust;
    ofxFloatSlider rotationSpeedSlider;

    // Listeners
    void onToggleEmitter(bool& draw);
    void onToggleHeadingVector(bool& show);
    void onToggleShape(bool& useImage);
    void onScaleChange(float& scale);
    void onToggleImage(bool& useImage);
    void onAgentCountChange(int& count);
    void onAgentSpeedChange(float& speed);
    void onAgentThrustChange(float& thrust);
    void onAgentRotationChange(float& rotation);

    // Mouse position tracking
    glm::vec3 mousePrevPos;
    ofVec3f mouse_last;

    // sound
    ofSoundPlayer thrustSound;
    ofSoundPlayer quackSound;
    ofSoundPlayer explosionSound;
    ofSoundPlayer explosion2Sound;
    ofSoundPlayer alertSound;
    ofSoundPlayer boopSound;

    // Miscellaneous
    bool bHide = false;
    std::map<int, bool> keyStates;
    ofTrueTypeFont title;
    ofTrueTypeFont desc;
};
