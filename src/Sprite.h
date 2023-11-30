#pragma once

#include "Shape.h"



// Base class for a Sprite. Can be instantiated on it's own (deafult)
// or subclassed to create a custom sprite.  Includes Shape transformations.
// If the sprite has no image set on it, then a simple triangle will be drawn.
//

class Sprite : public Shape {
public:
	Sprite() {
		// default geometry (triangle) if no image attached.
		//
		//bottom left
		verts.push_back(glm::vec3(-20, 30, 0));
		//bottom right
		verts.push_back(glm::vec3(20, 30, 0));
		//top
		verts.push_back(glm::vec3(0, -30, 0));
	}
	
	// some functions for highlighting when selected
	//
	void draw() {
		if (bShowImage) {
			ofPushMatrix();
			ofSetColor(ofColor::white);
			ofMultMatrix(getTransform());
			spriteImage.draw(-spriteImage.getWidth() / 2, -spriteImage.getHeight() / 2.0);
			ofPopMatrix();
		}
		else
		{
			if (bHighlight) ofSetColor(ofColor::white);
			else ofSetColor(ofColor::green);
			ofPushMatrix();
			ofMultMatrix(getTransform());
			ofDrawTriangle(verts[0], verts[1], verts[2]);
			ofPopMatrix();
		}
	}

	float age() {
		return (ofGetElapsedTimeMillis() - birthtime);
	}

	void setImage(ofImage img) {
		spriteImage = img;
		bShowImage = true;
		width = img.getWidth();
		height = img.getHeight();
	}

	virtual bool insidePoint(const glm::vec3 p);
	//virtual bool insideSprite(const Sprite sprite);
	virtual void update() {}
	virtual void setMoveSpeed(int moveSpeed);
	virtual void setRotationSpeed(float rotSpeed);
	virtual void setScale(float scale);
	virtual void setHeight(float height);
	virtual void setWidth(float width);
	virtual void setVelocity(glm::vec3 v);
	bool insideTriangle(const glm::vec3 p);
	void decreaseEnergy(int d);
	void increaseEnergy(int d);

	void setSelected(bool state) { bSelected = state; }
	void setHighlight(bool state) { bHighlight = state; }
	bool isSelected() { return bSelected; }
	bool isHighlight() { return bHighlight; }

	void integrate();
	void addForces(glm::vec3 f);
	void addAngularForces(float f);
	
	glm::vec3 heading();

	bool bHighlight = false;
	bool bSelected = false;
	bool bShowImage = false;

	//Physics Variables
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	glm::vec3 forces = glm::vec3(0, 0, 0);;
	glm::vec3 velocity = glm::vec3(0, 0, 0);
	float angularForce = 0;
	float angularVelocity = 0;
	float angularAcceleration = 0;
	float mass = 1.0;
	float damping = .96;

	float rotationSpeed = 0.0;
	float moveSpeed = 50;
	float birthtime = 0; // elapsed time in ms
	float lifespan = -1;  //  time in ms
	string name =  "UnammedSprite";
	float width;
	float height;
	ofImage spriteImage;
	int nEnergy = 5;

	// default verts for polyline shape if no image on sprite
	//
	vector<glm::vec3> verts;
};

