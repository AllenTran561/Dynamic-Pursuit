#include "Sprite.h"

//
	// inside - check if point is inside player (can be an image or TriangleShape if no image)
	//
bool Sprite::insidePoint(const glm::vec3 p) {

	// if there is no sprite image attached, then just use triangle case.
	//
	if (!bShowImage) {
		return insideTriangle(p);
	}

	// if sprite image attached, then first check if point is inside bounds of image
	// in object space.  If point is inside bounds, then make sure the point is in
	// opaque part of image.
	//
	glm::vec3 s = glm::inverse(getTransform()) * glm::vec4(p, 1);
	int w = spriteImage.getWidth();
	int h = spriteImage.getHeight();
	if (s.x > -w / 2 && s.x < w / 2 && s.y > -h / 2 && s.y < h / 2) {
		int x = s.x + w / 2;
		int y = s.y + h / 2;
		ofColor color = spriteImage.getColor(x, y);
		return (color.a != 0);   // check if color is opaque (not the transparent background)
	}
	else return false;
}

bool Sprite::insideTriangle(const glm::vec3 p) {

	// transform the screen space point p to the triangle's local 
	// oordinate system  (object space);  this will take into account any
	// rotation, translation or scale on the object.
	//
	glm::vec4 p2 = glm::inverse(getTransform()) * glm::vec4(p, 1);

	glm::vec3 v1 = glm::normalize(verts[0] - p2);
	glm::vec3 v2 = glm::normalize(verts[1] - p2);
	glm::vec3 v3 = glm::normalize(verts[2] - p2);
	float a1 = glm::orientedAngle(v1, v2, glm::vec3(0, 0, 1));
	float a2 = glm::orientedAngle(v2, v3, glm::vec3(0, 0, 1));
	float a3 = glm::orientedAngle(v3, v1, glm::vec3(0, 0, 1));
	if (a1 < 0 && a2 < 0 && a3 < 0) return true;
	else return false;
}

void Sprite::integrate() {


	// interval for this step
	//
	float dt = 1.0 / ofGetFrameRate();

	// update position based on velocity
	//
	pos += (velocity * dt);

	// update acceleration with accumulated paritcles forces
	// remember :  (f = ma) OR (a = 1/m * f)
	//
	ofVec3f accel = acceleration;    // start with any acceleration already on the particle
	accel += (forces * (1.0 / mass));
	velocity += accel * dt;

	// add a little damping for good measure
	//
	velocity *= damping;

	// clear forces on particle (they get re-added each step)
	//

	rot += (angularVelocity * dt);
	float a = angularAcceleration;
	a += (angularForce * 1.0 / mass);
	angularVelocity += a * dt;
	angularVelocity *= damping;

	forces = glm::vec3(0, 0, 0);
	angularForce = 0;
}

void Sprite::addForces(glm::vec3 f) {
	forces = f;
}

void Sprite::addAngularForces(float f) {
	angularForce = f;
}

//Returns heading of the sprite
glm::vec3 Sprite::heading() {
	glm::mat4 rot2 = glm::rotate(glm::mat4(1.0), glm::radians(rot), glm::vec3(0, 0, 1));
	return glm::normalize(rot2 * glm::vec4(0, -1, 0, 1));
}

void Sprite::setVelocity(glm::vec3 v) {
	velocity = v;
}
//Sets move speed
void Sprite::setMoveSpeed(int moveSpeed) {
	this->moveSpeed = moveSpeed;
}

//sets rotation speed
void Sprite::setRotationSpeed(float rotationSpeed) {
	this->rotationSpeed = rotationSpeed;
}

//sets scale
void Sprite::setScale(float scale) {
	this->scale = glm::vec3(scale,scale,scale);
}

//sets height
void Sprite::setHeight(float height) {
	this->height = height;
}

//sets width
void Sprite::setWidth(float width) {
	this->width = width;
}

void Sprite::decreaseEnergy(int d) {
	nEnergy -= d;
}
void Sprite::increaseEnergy(int d) {
	nEnergy += d;
}