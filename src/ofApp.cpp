#include "ofApp.h"

// spawnSprite - we override this function in the Emitter class to spawn our 
// "custom" Agent sprite.
//
void AgentEmitter::spawnSprite() {
	for (int i = 0; i < nAgents; i++) {
		Agent sprite;
		if (haveChildImage) {
			sprite.setImage(childImage);
		}
		else {
			sprite.bHighlight = true;
			sprite.setHeight(abs(sprite.verts[0].y) + abs(sprite.verts[2].y));
			sprite.setWidth(abs(sprite.verts[0].x) + abs(sprite.verts[1].x));
		}
		sprite.velocity = velocity;
		sprite.lifespan = lifespan;
		switch (emitterType) {
		case enemySpawner:
			sprite.pos = glm::vec3(ofRandom(0, ofGetScreenWidth()), ofRandom(0, ofGetScreenHeight()), 0);
			sprite.rot = ofRandom(0, 360);
			sprite.birthtime = ofGetElapsedTimeMillis();
			sys->add(sprite);
			break;
		case playerFire:
			sprite.pos = pos;
			sprite.rot = rot;
			sprite.birthtime = ofGetElapsedTimeMillis();
			if (sprite.birthtime - lastSpawned > 1000) {
				lastSpawned = sprite.birthtime;
				sys->add(sprite);
			}
			break;
		case explosion:
			sprite.pos = glm::vec3(pos.x + ofRandom(-10,10), pos.y + ofRandom(-10, 10), 0);
			sprite.rot = rot;
			sprite.addForces(glm::vec3(ofRandom(-10000, 10000), ofRandom(-10000, 10000), 0));
			sprite.birthtime = ofGetElapsedTimeMillis();
			sys->add(sprite);
			break;
		}
	}
}

//  moveSprite - we override this function in the Emitter class to implment
//  "following" motion towards the player
//
void AgentEmitter::moveSprite(Sprite* sprite) {

	// Get a pointer to the app so we can access application
	// data such as the player sprite.
	//
	ofApp* theApp = (ofApp*)ofGetAppPtr();

	// rotate sprite to point towards player
	//  - find vector "v" from sprite to player
	//  - set rotation of sprite to align with v
	//

	glm::vec3 v = glm::normalize(theApp->player->pos - sprite->pos);
	glm::vec3 h = sprite->heading();
	float dotp = glm::dot(h, v);
	float eps = .0005;
	float sp = sprite->rotationSpeed;
	glm::vec3 crossp = glm::cross(h, v);
	switch (emitterType) {
	case enemySpawner:
		if (dotp < (1.0 - eps)) {
			if (crossp.z > 0.0) {
				sprite->rot += sp;
			}
			else {
				sprite->rot -= sp;
			}
		}
		sprite->addForces(500 * v);
		sprite->integrate();
		break;
	}

	// Calculate new velocity vector
	// with same speed (magnitude) as the old one but in direction of "v"
	// 	
	// Now move the sprite in the normal way (along velocity vector)
	//
	Emitter::moveSprite(sprite);
}

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetVerticalSync(true);
	totalTime = 0;
	//default difficulty
	dif = normal;
	toggleSprites = true;
	ofResetElapsedTimeCounter();
	setupVisuals();
	setupGui(dif);
	setupObjects();
	ofSetFullscreen(true);
	gameState = ready;
}

//Setups visuals and sounds
void ofApp::setupVisuals() {
	if (enemyImage.load("images/Missile2.png") && toggleSprites == true) {
		enemyLoaded = true;
	}
	else {
		enemyLoaded = false;
		cout << "Can't open image file" << endl;
	}
	if (beamImage.load("images/Beam.png") && toggleSprites == true) {
		beamLoaded = true;
	}
	else {
		beamLoaded = false;
		cout << "Can't open image file" << endl;
	}
	if (background.load("images/Background1.png")) {
		backgroundLoaded = true;
	}
	else {
		cout << "Can't open background image file" << endl;
	}
	beamSound.load("sounds/beam.wav");
	beamSound.setLoop(true);
	beamSound.setVolume(.2);
	engineSound.load("sounds/engine.wav");
	engineSound.setLoop(true);
	engineSound.setVolume(.5);
	explosionSound.load("sounds/explosion.wav");
	explosionSound.setLoop(true);
	explosionSound.setVolume(.2);
}

//Creates enemy, explosion, and beam emitter along with player
//--------------------------------------------------------------
void ofApp::setupObjects() {
	//Create enemy emitter and start it
	enemyEmitter = new AgentEmitter();  // C++ polymorphism
	enemyEmitter->emitterType = enemySpawner;
	enemyEmitter->pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0);
	enemyEmitter->drawable = true;
	if (enemyLoaded && toggleSprites) {
		enemyEmitter->setChildImage(enemyImage);
	}
	enemyEmitter->start();

	//create a player sprite to chase
	//
	player = new Sprite();
	player->bHighlight = true;
	player->pos = glm::vec3(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2, 0);
	if (player->bShowImage == false) {
		player->setHeight(abs(player->verts[0].y) + abs(player->verts[2].y));
		player->setWidth(abs(player->verts[0].x) + abs(player->verts[1].x));
	}

	//Create beam emitter and start it
	beamEmitter = new AgentEmitter();
	beamEmitter->emitterType = playerFire;
	beamEmitter->pos = player->pos;
	beamEmitter->rot = player->rot;
	beamEmitter->drawable = true;
	if (beamLoaded && toggleSprites) {
		beamEmitter->setChildImage(beamImage);
	}
	beamEmitter->start();

	//Create explosion emitter and start it
	explosionEmitter = new AgentEmitter();
	explosionEmitter->emitterType = explosion;
	explosionEmitter->pos = glm::vec3(500, 500, 0);
	explosionEmitter->drawable = true;
	explosionEmitter->start();
}

//--------------------------------------------------------------
//Setups gui
void ofApp::setupGui(difficulty dif) {
	float d = float(dif) / 10;
	//easy = .8 multiplier
	//normal = 1 multiplier
	//hard = 1.2 multiplier
	gui.setup();
	gui.add(rateOfSpawn.setup("rate", d * 1, 1, 10));
	gui.add(enemyLife.setup("life", d * 5, .1, 15));
	gui.add(velocity.setup("velocity", d * glm::vec3(150, 150, 0), ofVec3f(0, 0, 0), ofVec3f(1000, 1000, 0)));
	gui.add(nAgents.setup("nAgents", 1, 1, 3));
	gui.add(scale.setup("Scale", .8, .1, 1.0));
	gui.add(rotationSpeed.setup("Rotation Speed (deg/Frame)", 3, 1, 5));

	gui.add(nEnergy.setup("nEnergy", 5, 0, 10));
	gui.add(playerMoveSpeed.setup("moveSpeed", 1500, 100, 5000));
	gui.add(playerRotationSpeed.setup("playerRotationSpeed", 500, 0, 3000));
	gui.add(playerScale.setup("Player Scale", 1, 0, 5));

	gui.add(beamLife.setup("Beam Life", 2, .1, 10));
	gui.add(beamSpeed.setup("Beam Speed", 1500, 100, 5000));

	bHide = true;
}

//Updates the program by frame
//--------------------------------------------------------------
void ofApp::update() {
	if (!gameState == playable) {
		return;
	}
	updateKeyPressed();
	updatePlayer();
	updateBeamEmitter();
	updateEnemyEmitter();
	updateExplosionEmitter();

}

//--------------------------------------------------------------
//Updates Keys
void ofApp::updateKeyPressed() {  
	if (keymap[OF_KEY_UP]) {
		player->addForces(player->moveSpeed * player->heading());
		player->bEngine = true;
	}
	if (keymap[OF_KEY_DOWN]) {
		player->addForces(-player->moveSpeed * player->heading());
		player->bEngine = true;

	}
	if (keymap[OF_KEY_LEFT]) {
		player->addAngularForces(-playerRotationSpeed);
		player->bEngine = true;
	}
	if (keymap[OF_KEY_RIGHT]) {
		player->addAngularForces(playerRotationSpeed);
		player->bEngine = true;
	}
}

//--------------------------------------------------------------
//Update player values
void ofApp::updatePlayer() {
	player->integrate();
	player->setRotationSpeed(playerRotationSpeed);
	player->setMoveSpeed(playerMoveSpeed);
	player->setScale(playerScale);
	player->update();
	checkBorder(*player);
		//player->velocity = glm::vec3(-2 * player->velocity.x, player->velocity.y, 0);
	if (player->nEnergy == 0) {
		gameState = gameOver;
		totalTime = ofGetElapsedTimeMillis() / 1000;
	}
	if (player->bEngine && !engineSound.isPlaying()) {
		engineSound.play();
	}
	else if (!player->bEngine && engineSound.isPlaying()) {
		engineSound.stop();
	}
}
//--------------------------------------------------------------
//Updates beamEmitter
void ofApp::updateBeamEmitter() {
	beamEmitter->pos = player->pos;
	beamEmitter->rot = player->rot;
	beamEmitter->rate = 1;
	beamEmitter->setLifespan(beamLife * 1000);
	beamEmitter->setVelocity(player->heading() * int (beamSpeed));
	beamEmitter->setNAgents(1);
	beamEmitter->update();
	if (beamEmitter->bBeam && !beamSound.isPlaying()) {
		beamTime = ofGetElapsedTimeMillis();
		beamSound.play();
	}
	else if (!beamEmitter->bBeam && beamSound.isPlaying()) {
		if (ofGetElapsedTimeMillis() - beamTime > 1000) {
			beamSound.stop();
		}
	}
	for (int i = 0; i < beamEmitter->sys->sprites.size(); i++) {
		// Get values from sliders and update sprites dynamically
		//
		Sprite &s = beamEmitter->sys->sprites[i];
		float sc = scale;
		float rs = rotationSpeed;
		s.scale = glm::vec3(sc, sc, sc);
		s.setRotationSpeed(rs);
		checkBorder(s);
		//Check Collision for each enemy and beam, remove if collided
		for (int j = 0; j < enemyEmitter->sys->sprites.size(); j++) {
			if (checkCollision(s, enemyEmitter->sys->sprites[j])) {
				explosionEmitter->pos = enemyEmitter->sys->sprites[j].pos;
				explosionEmitter->spawnSprite();
				enemyEmitter->sys->remove(j);
				explosionEmitter->bExplosion = true;
				//player->increaseEnergy(1);
			}
		}
	}
}

//--------------------------------------------------------------
//Update enemyEmitter values
void ofApp::updateEnemyEmitter() {
	enemyEmitter->setRate(rateOfSpawn);
	enemyEmitter->setLifespan(enemyLife * 1000);    // convert to milliseconds 
	enemyEmitter->setVelocity(ofVec3f(velocity->x, velocity->y, velocity->z));
	enemyEmitter->setNAgents(nAgents);
	enemyEmitter->update();
	for (int i = 0; i < enemyEmitter->sys->sprites.size(); i++) {
		// Get values from sliders and update sprites dynamically
		//
		Sprite& s = enemyEmitter->sys->sprites[i];
		float sc = scale;
		float rs = rotationSpeed;
		s.scale = glm::vec3(sc, sc, sc);
		s.setRotationSpeed(rs);
		if (checkCollision(s, *player)) {
			explosionEmitter->pos = enemyEmitter->sys->sprites[i].pos;
			explosionEmitter->spawnSprite();
			explosionEmitter->bExplosion = true;
			player->decreaseEnergy(1);
			enemyEmitter->sys->remove(i);
		}
	}
}
//--------------------------------------------------------------
//Updates explosionEmitter values
void ofApp::updateExplosionEmitter() {
	explosionEmitter->pos = player->pos;
	explosionEmitter->rot = player->rot;
	explosionEmitter->rate = 10;
	explosionEmitter->setLifespan(1000);
	explosionEmitter->setVelocity(glm::vec3(ofRandom(-5, 5), ofRandom(-5, 5), 0));
	explosionEmitter->setNAgents(10);
	explosionEmitter->update();
	if (explosionEmitter->bExplosion && !explosionSound.isPlaying()) {
			explosionTime = ofGetElapsedTimeMillis();
			explosionSound.play();
	}
	else if (!explosionEmitter->bExplosion && explosionSound.isPlaying()) {
			explosionSound.stop();
	}
	else if (explosionEmitter->bExplosion && explosionSound.isPlaying()) {
		if (ofGetElapsedTimeMillis() - explosionTime > 1000)
			explosionEmitter->bExplosion = false;
	}
	for (int i = 0; i < explosionEmitter->sys->sprites.size(); i++) {
		// Get values from sliders and update sprites dynamically
		//
		Sprite& s = explosionEmitter->sys->sprites[i];
		float sc = .3;
		float rs = rotationSpeed;
		s.scale = glm::vec3(sc, sc, sc);
		s.setRotationSpeed(rs);
		s.integrate();
		//Check Collision for each enemy and beam, remove if collided
	}
}

//--------------------------------------------------------------
//Checks if sprite collided with another sprite
bool ofApp::checkCollision(Sprite s1, Sprite s2) {
	for (int i = 0; i < 3; i++) {
		glm::vec3 sVert = s1.getTransform() * glm::vec4(s1.verts[i], 1.0f);
		glm::vec3 tVert = s2.getTransform() * glm::vec4(s2.verts[i], 1.0f);
		if (s2.insidePoint(sVert) || s1.insidePoint(tVert)) {
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------
//Checks if temperary sprite pos is outside screen border
//Uses +- height to get the full image size
//Bounces sprite off of the border
void ofApp::checkBorder(Sprite &s) {
	float x = s.pos.x;
	float y = s.pos.y;
	float height = s.height;
	float width = s.width;
	if (x - width / 2 < 0 || x + width / 2 > ofGetScreenWidth()) {
		s.setVelocity(glm::vec3(-2 * s.velocity.x, s.velocity.y, 0));
	}
	else if (y - height / 2 < 0 || y + height / 2 > ofGetScreenHeight()) {
		s.setVelocity(glm::vec3(s.velocity.x, -2 * s.velocity.y, 0));
	}
}

//--------------------------------------------------------------
//Draws App
void ofApp::draw() {
	ofSetColor(ofColor::white);
	if (gameState == playable) {
		if (backgroundLoaded) {
			background.draw(0,0);
		}
		enemyEmitter->draw();
		beamEmitter->draw();
		explosionEmitter->draw();
		ofSetColor(ofColor::aqua);
		ofDrawLine(player->pos, player->pos + player->heading() * glm::vec3(3000, 3000, 0));
		ofSetColor(ofColor::white);
		player->draw();
		ofDrawBitmapString("nEnergy = ", ofGetScreenWidth() - 100, 25);
		ofDrawBitmapString(player->nEnergy, ofGetScreenWidth() - 20, 25);
		ofDrawBitmapString(ofGetFrameRate(), ofGetScreenWidth() - 100, 50);
		ofDrawBitmapString(ofGetElapsedTimeMillis() / 1000, ofGetScreenWidth() - 100, 75);
	}

	else if (gameState == ready) {
		ofDrawBitmapString("To start game press space bar", ofGetScreenWidth() / 2 -100, ofGetScreenHeight() / 2);
		ofDrawBitmapString("1 = easy    2 = normal    3 = hard", ofGetScreenWidth() / 2 - 100, ofGetScreenHeight() / 2 + 25);
		switch (dif) {
		case easy:
			ofDrawBitmapString("Easy Selected", ofGetScreenWidth() / 2 - 100, ofGetScreenHeight() / 2 + 50);
			break;
		case normal:
			ofDrawBitmapString("Normal Selected", ofGetScreenWidth() / 2 - 100, ofGetScreenHeight() / 2 + 50);
			break;
		case hard:
			ofDrawBitmapString("Hard Selected", ofGetScreenWidth() / 2 - 100, ofGetScreenHeight() / 2 + 50);
			break;

		}
		ofDrawBitmapString("Press 'h' to show GUI menu", ofGetScreenWidth() / 2 - 100, ofGetScreenHeight() / 2 + 75);
		ofDrawBitmapString("Press 'q' to toggle sprites", ofGetScreenWidth() / 2 - 100, ofGetScreenHeight() / 2 + 100);
		ofDrawBitmapString("Custom Sprites = ", ofGetScreenWidth() / 2 - 100, ofGetScreenHeight() / 2 + 125);
		if (toggleSprites) {
			ofDrawBitmapString("True", ofGetScreenWidth() / 2 + 50, ofGetScreenHeight() / 2 + 125);
		}
		else {
			ofDrawBitmapString("False", ofGetScreenWidth() / 2 + 50, ofGetScreenHeight() / 2 + 125);
		}
		ofSetBackgroundColor(ofColor::black);
	}
	else if (gameState == gameOver) {
		ofDrawBitmapString("Game Over", ofGetScreenWidth() / 2 - 50, ofGetScreenHeight() / 2);
		ofDrawBitmapString("Total Time Survived = ", ofGetScreenWidth() / 2 - 50, ofGetScreenHeight() / 2 + 25);
		ofDrawBitmapString(totalTime, ofGetScreenWidth() / 2 + 125, ofGetScreenHeight() / 2 + 25);
		ofDrawBitmapString("Press space to return to menu", ofGetScreenWidth() / 2 - 50, ofGetScreenHeight() / 2 + 50);
		ofSetBackgroundColor(ofColor::black);
	}
	if (!bHide) {
		gui.draw();
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if (bDrag) {
		glm::vec3 p = glm::vec3(x, y, 0);
		glm::vec3 delta = p - lastMousePos;
		player->pos += delta;
		lastMousePos = p;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	glm::vec3 pos = glm::vec3(x, y, 0);
	if (player->insidePoint(pos)) {
		bDrag = true;
		lastMousePos = pos;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bDrag = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}
	
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
		//Toggles Fullscreen
	case 'f':
		ofToggleFullscreen();
		break;
		//Hides GUI
	case 'h':
		bHide = !bHide;
		break;
		//Sets difficulty to easy
	case '1':
		if (gameState == ready) {
			dif = easy;
			setupGui(dif);
			setupObjects();
			break;
		}
		//Sets difficulty to normal
	case '2':
		if (gameState == ready) {
			dif = normal;
			setupGui(dif);
			setupObjects();
			break;
		}
		//Sets difficulty to hard
	case '3':
		if (gameState == ready) {
			dif = hard;
			setupGui(dif);
			setupObjects();
			break;
		}
		//Starts/restarts game
	case ' ':
		if (gameState == gameOver) {
			setup();
			break;
		}
		else if (gameState == ready) {
			gameState = playable;
			ofResetElapsedTimeCounter();
			bHide = false;
			break;
		}
		else if (gameState == playable) {
			beamEmitter->spawnSprite();
			beamEmitter->bBeam = true;
			break;
		}
		//Toggles sprites at the beginning
	case 'q':
		if (gameState == ready) {
			toggleSprites = !toggleSprites;
			setupObjects();
			break;
		}
	}

	keymap[key] = true;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case OF_KEY_LEFT:   // turn left
		player->bEngine = false;
		break;
	case OF_KEY_RIGHT:  // turn right
		player->bEngine = false;		
		break;
	case OF_KEY_UP:     // go forward
		player->bEngine = false;		
		break;
	case OF_KEY_DOWN:   // go backward
		player->bEngine = false;
		break;
	case ' ':
		if (gameState == playable) {
			beamEmitter->bBeam = false;
		}
	default:
		break;
	}
	keymap[key] = false;
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