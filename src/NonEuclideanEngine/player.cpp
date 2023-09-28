#include <NonEuclideanEngine/player.hpp>

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

// -------------------- //
// PlayerInputHandler //

const uint32_t Knee::PlayerInputHandler::KEYBIND_FORWARD = 0;
const uint32_t Knee::PlayerInputHandler::KEYBIND_BACKWARD = 1;
const uint32_t Knee::PlayerInputHandler::KEYBIND_STRAFELEFT = 2;
const uint32_t Knee::PlayerInputHandler::KEYBIND_STRAFERIGHT = 3;
const uint32_t Knee::PlayerInputHandler::KEYBIND_UP = 4;
const uint32_t Knee::PlayerInputHandler::KEYBIND_DOWN = 5;
const uint32_t Knee::PlayerInputHandler::KEYBIND_SPRINT = 6;
const uint32_t Knee::PlayerInputHandler::KEYBIND_UNLOCK_CURSOR = 7;

Knee::PlayerInputHandler::PlayerInputHandler() {}

// KEYBOARD //

bool Knee::PlayerInputHandler::keybindExists(uint32_t name){
	// check if name is a bind
	return this->m_keybinds.count(name) > 0;
}

// throws exception if name isn't a bind
SDL_Scancode Knee::PlayerInputHandler::getKeybind(uint32_t name){
	return this->m_keybinds.at(name);
}

// silently fails if name isn't a bind
void Knee::PlayerInputHandler::keybind(uint32_t name, SDL_Scancode scancode){
	// check if name is a bind
	if( !this->keybindExists(name) ) return;
	
	// set bind
	this->m_keybinds[name] = scancode;
}

bool Knee::PlayerInputHandler::getKeyState(SDL_Scancode scancode){
	return this->m_keystates[scancode];
}

bool Knee::PlayerInputHandler::getKeybindState(uint32_t name){
	// check that bind exists
	if( !this->keybindExists(name) ) return false;
	
	// return state
	return this->getKeyState( this->getKeybind(name) );
}

// MOUSE BUTTON //

bool Knee::PlayerInputHandler::getMouseButtonState(uint8_t button){
	return this->m_mouseButtonStates[button];
}

// MOUSE MOTION //

glm::vec2 Knee::PlayerInputHandler::getMouseMotion(){
	return this->m_mouseMotion;
}

void Knee::PlayerInputHandler::resetMouseMotion(){
	this->m_mouseMotion = glm::vec2(0);
}

// EVENT HANDLING //

void Knee::PlayerInputHandler::reset(){
	this->resetMouseMotion();
}

// process a keyboard event, updating appropriate state
void Knee::PlayerInputHandler::processKeyboardEvent(SDL_KeyboardEvent& event){
	// fetch scancode
	SDL_Scancode scancode = event.keysym.scancode;
	
	// change key state depending on press/release
	this->m_keystates[scancode] = event.state == SDL_PRESSED;
}

// process a mouse button event, updating appropriate states
void Knee::PlayerInputHandler::processMouseButtonEvent(SDL_MouseButtonEvent& event){
	// fetch state
	bool state = event.state == SDL_PRESSED;
	
	// fetch button that was pressed
	uint8_t button = event.button;
	
	// update state
	this->m_mouseButtonStates[button] = state;
}

// process a mouse motion event
void Knee::PlayerInputHandler::processMouseMotionEvent(SDL_MouseMotionEvent& event){
	// get mouse motion
	int32_t xrel = event.xrel;
	int32_t yrel = event.yrel;
	
	this->m_mouseMotion += glm::vec2((float)xrel, (float)yrel);
}

// process an event
void Knee::PlayerInputHandler::processEvent(SDL_Event& event){
	switch(event.type){
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			this->processKeyboardEvent(event.key);
			break;
		case SDL_MOUSEBUTTONUP:		
		case SDL_MOUSEBUTTONDOWN:
			this->processMouseButtonEvent(event.button);
			break;
		case SDL_MOUSEMOTION:
			this->processMouseMotionEvent(event.motion);
			break;
	}
}

// -------------------- //
// Player //

const double Knee::Player::X_ROTATION_LIMIT = glm::radians(89.9f);
const std::map<uint32_t, glm::vec3> Knee::Player::KEYBIND_MOTION_VECTORS = {
	{ Knee::PlayerInputHandler::KEYBIND_FORWARD, glm::vec3(0, 0, 1) }, 
	{ Knee::PlayerInputHandler::KEYBIND_BACKWARD, glm::vec3(0, 0, -1) },
	{ Knee::PlayerInputHandler::KEYBIND_STRAFELEFT, glm::vec3(1, 0, 0) },
	{ Knee::PlayerInputHandler::KEYBIND_STRAFERIGHT, glm::vec3(-1, 0, 0) },
	{ Knee::PlayerInputHandler::KEYBIND_UP, glm::vec3(0, 1, 0) },
	{ Knee::PlayerInputHandler::KEYBIND_DOWN, glm::vec3(0, -1, 0) }
};

Knee::Player::Player() : GameObject() {}

Knee::PlayerInputHandler* Knee::Player::getInputHandler(){
	return &this->m_inputHandler;
}

Knee::PerspectiveCamera* Knee::Player::getCamera(){
	return this->m_camera;
}

void Knee::Player::setCamera(Knee::PerspectiveCamera* camera){
	this->m_camera = camera;
}

bool Knee::Player::checkMouseLock(){
	// fetch input handler
	Knee::PlayerInputHandler* inputHandler = this->getInputHandler();
	
	// check if mouse is locked
	if(SDL_GetRelativeMouseMode() == SDL_FALSE){
		// if not, lock when user presses mouse button
		if(inputHandler->getMouseButtonState(SDL_BUTTON_LEFT)){
			SDL_SetRelativeMouseMode(SDL_TRUE);
		
			return true;
		}
		
		return false;
	} else {
		// if we are, unlock when user presses the UNLOCK_CURSOR keybind
		if(inputHandler->getKeybindState(Knee::PlayerInputHandler::KEYBIND_UNLOCK_CURSOR)){
			SDL_SetRelativeMouseMode(SDL_FALSE);

			return false;
		}

		return true;
	}
}

void Knee::Player::lockXRotation(){
	// lock player rotation
	float limit = Knee::Player::X_ROTATION_LIMIT;
	
	glm::vec3 newRotation = this->getRotation();
	
	if(this->getRotation().x > limit){
		newRotation.x = limit;
	} else if(this->getRotation().x < -limit){
		newRotation.x = -limit;
	}
	
	this->setRotation(newRotation);
}

void Knee::Player::handleMouseMovement(){
	// fetch input handler
	Knee::PlayerInputHandler* inputHandler = this->getInputHandler();
	
	// check if mouse is locked, rotate if it is
	if(this->checkMouseLock()){
		// rotate
		glm::vec2 mouseMotion = (float)this->m_mouseSensitivity * inputHandler->getMouseMotion() / 500.f;
		
		glm::vec3 change = glm::vec3(mouseMotion.y, -mouseMotion.x, 0);

		//std::cout << "yaw axis: " << glm::to_string(this->m_yawAxis) << std::endl;

		// apply transformation
		this->rotateAboutAxis(change.y, this->m_yawAxis);
		this->rotateAboutAxis(change.x, this->getCrossVector());
	}
}

glm::vec3 Knee::Player::getDesiredMotionVector(){
	// fetch input handler
	Knee::PlayerInputHandler* inputHandler = this->getInputHandler();
	
	// create initial motion vector
	glm::vec3 motionVector = glm::vec3(0);
	
	// get list of all desired motion vectors by bind
	std::map<uint32_t, glm::vec3> motionVectors = Knee::Player::KEYBIND_MOTION_VECTORS;
	
	// loop through each item in motion vectors
	for(std::map<uint32_t, glm::vec3>::iterator it = motionVectors.begin(); it != motionVectors.end(); ++it){
		// get bind name
		uint32_t bindName = it->first;
		
		// get bind state
		bool state = inputHandler->getKeybindState(bindName);
		
		// get desired motion
		glm::vec3 desired = it->second;
	
		// add to motion vector
		motionVector += (float)state * desired;	
	}
	
	// rotate motion vector according to player's current rotation
	motionVector = glm::vec3(this->getRotationMatrix() * glm::vec4(motionVector, 1));
	
	if(glm::length2(motionVector) > 0.0001){
		motionVector = glm::normalize(motionVector);
	}
	
	return motionVector;
}

void Knee::Player::handleKeyboard(double delta){
	// fetch input handler
	Knee::PlayerInputHandler* inputHandler = this->getInputHandler();
	
	// get desired motion vector
	glm::vec3 motionVector = this->getDesiredMotionVector();
	
	// apply speed
	bool sprinting = inputHandler->getKeybindState(Knee::PlayerInputHandler::KEYBIND_SPRINT);
	
	motionVector *= (float)this->m_playerSpeed * (sprinting ? this->m_sprintMultiplier : 1.0f);
	
	if(inputHandler->getKeyState(SDL_SCANCODE_G)){
		std::cout << "current player position: " << glm::to_string(this->getPosition()) << std::endl;
		std::cout << "current player rotation: " << glm::to_string(this->getRotation()) << std::endl;
	}

	// set velocity
	this->changeVelocity(motionVector);
}

void Knee::Player::applyTransformation(GeneralObject t){
	glm::vec3 oldScale = this->getScale();
	
	// call parent
	GeneralObject::applyTransformation(t);

	// update yaw axis
	Knee::GeneralObject obj(glm::vec3(0), glm::vec3(0, this->getRotation().y, this->getRotation().z), glm::vec3(1));

	this->m_yawAxis = glm::vec3(obj.getRotationMatrix() * glm::vec4(0, 1, 0, 1));

	// adjust max speed based on scaling
	this->m_playerSpeed *= this->getScale().x / oldScale.x;
}

void Knee::Player::handleMovement(double delta){
	// fetch input handler
	Knee::PlayerInputHandler* inputHandler = this->getInputHandler();
	
	// MOUSE MOVEMENT //
	
	// handle mouse movement appropriately
	this->handleMouseMovement();

	// prevent player from looking behind themselves
	//this->lockXRotation();
	
	// KEYBOARD MOVEMENT //
	
	// handle keypresses
	this->handleKeyboard(delta);
}

void Knee::Player::update(double delta){
	// call base update
	GameObject::update(delta);

	// reset velocity
	this->setVelocity(glm::vec3(0));
}