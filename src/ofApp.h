#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Emitter.h"
#include "Shape.h"
#include "Sprite.h"



class Agent : public Sprite {
public:
	Agent() {
		Sprite::Sprite();
//		cout << "in Agent Constuctor" << endl;
	}
};

class AgentEmitter : public Emitter {
public:
	void spawnSprite();
	void moveSprite(Sprite*);
};

enum gameState {
	ready,
	playable,
	gameOver
};
enum difficulty {
	easy = 8,
	normal = 10,
	hard = 12
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void setupObjects();
		void setupGui(enum difficulty);
		void setupVisuals();

		void updateKeyPressed();
		void updatePlayer();
		void updateEnemyEmitter();
		void updateBeamEmitter();
		void updateExplosionEmitter();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		bool checkCollision(Sprite s1, Sprite s2);
		void checkBorder(Sprite &s);

		Emitter *enemyEmitter = NULL;
		Emitter *beamEmitter = NULL;
		Sprite *player = NULL;
		Emitter *explosionEmitter = NULL;
		bool fire;

		int totalTime;

		ofImage enemyImage;
		ofImage beamImage;
		ofImage background;

		ofSoundPlayer beamSound;
		ofSoundPlayer engineSound;
		ofSoundPlayer explosionSound;

		float beamTime = 0;
		float beamInterval = 1000;
		float explosionTime = 0;

		bool enemyLoaded;
		bool beamLoaded;
		bool backgroundLoaded;

		ofVec3f mouse_last;
		bool toggleSprites;
		bool bDrag = false;
		gameState gameState;
		difficulty dif;
		glm::vec3 lastMousePos;
		// Some basic UI
		//
		bool bHide;

		//Enemy sliders
		ofxFloatSlider rateOfSpawn;
		ofxFloatSlider enemyLife;
		ofxVec3Slider velocity;
		ofxIntSlider nAgents;
		ofxLabel screenSize;
		ofxFloatSlider scale;
		//player sliders
		ofxFloatSlider playerScale;
		ofxFloatSlider rotationSpeed;
		ofxFloatSlider playerRotationSpeed;
		ofxIntSlider playerMoveSpeed;
		ofxIntSlider nEnergy;

		ofxFloatSlider beamLife;
		ofxFloatSlider beamSpeed;
		ofxPanel gui;
		ofTrueTypeFont readyScreen;

		ofxIntSlider levelDifficulty;
		map<int, bool> keymap;
};
