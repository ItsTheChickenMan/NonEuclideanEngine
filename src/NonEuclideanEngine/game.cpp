#include <NonEuclideanEngine/game.hpp>
#include <NonEuclideanEngine/shader.hpp>

#include <iostream>

#include <glm/gtx/norm.hpp>

// StaticGameObject //

Knee::StaticGameObject::StaticGameObject(){
}

Knee::StaticGameObject::~StaticGameObject(){
}

// RenderableStaticGameObject //

Knee::RenderableStaticGameObject::RenderableStaticGameObject(Knee::VertexData* vertexData) : RenderableObject(vertexData) {
}

Knee::RenderableStaticGameObject::~RenderableStaticGameObject(){
}

// -------------------- //
// GameObject //

Knee::GameObject::GameObject() : m_mass(1.0) {}
Knee::GameObject::GameObject(double mass) : m_mass(mass) {}

Knee::GameObject::~GameObject(){}

double Knee::GameObject::getMass(){
	return this->m_mass;
}

void Knee::GameObject::setMass(double mass){
	this->m_mass = mass;
}

glm::vec3 Knee::GameObject::getVelocity(){
	return this->m_velocity;
}

void Knee::GameObject::setVelocity(glm::vec3 newVelocity){
	this->m_velocity = newVelocity;
}

void Knee::GameObject::changeVelocity(glm::vec3 change){
	this->m_velocity += change;
}

void Knee::GameObject::impulse(glm::vec3 f){
	this->m_velocity += f / (float)this->m_mass;
}

void Knee::GameObject::update(double delta){
	// determine change in position
	glm::vec3 changeInPosition = this->m_velocity * (float)delta;
	
	// change position
	this->changePosition(changeInPosition);
}

// -------------------- //
// RenderableGameObject //

Knee::RenderableGameObject::RenderableGameObject(Knee::VertexData* vertexData) : GameObject(), RenderableStaticGameObject(vertexData){}

Knee::RenderableObject* Knee::RenderableGameObject::asRenderableObject(){
	return static_cast<RenderableObject*>(this);
}

// -------------------- //
// Portal //

Knee::Portal::Portal(Knee::VertexData* vertexData) : RenderableGameObject(vertexData) {}

bool Knee::Portal::isVisible(Knee::PerspectiveCamera* camera){
	
	
	return false;
}

// -------------------- //
// PlayerInputHandler //

const uint32_t Knee::PlayerInputHandler::KEYBIND_FORWARD = 0;
const uint32_t Knee::PlayerInputHandler::KEYBIND_BACKWARD = 1;
const uint32_t Knee::PlayerInputHandler::KEYBIND_STRAFELEFT = 2;
const uint32_t Knee::PlayerInputHandler::KEYBIND_STRAFERIGHT = 3;
const uint32_t Knee::PlayerInputHandler::KEYBIND_UP = 4;
const uint32_t Knee::PlayerInputHandler::KEYBIND_DOWN = 5;
const uint32_t Knee::PlayerInputHandler::KEYBIND_SPRINT = 6;

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
	
	this->m_mouseMotion = glm::vec2((float)xrel, (float)yrel);
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

Knee::Player::Player(double mass) : GameObject(mass) {} 

Knee::PlayerInputHandler* Knee::Player::getInputHandler(){
	return &this->m_inputHandler;
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
		
		this->changeRotation( glm::vec3(mouseMotion.y, -mouseMotion.x, 0) );
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
	
	// apply delta
	motionVector *= (float)delta;
	
	this->changePosition(motionVector);
}

void Knee::Player::update(double delta){
	// fetch input handler
	Knee::PlayerInputHandler* inputHandler = this->getInputHandler();
	
	// MOUSE MOVEMENT //
	
	// handle mouse movement appropriately
	this->handleMouseMovement();

	// prevent player from looking behind themselves
	this->lockXRotation();
	
	// KEYBOARD MOVEMENT //
	
	// handle keypresses
	this->handleKeyboard(delta);
	
	// call base update
	GameObject::update(delta);
}

// -------------------- //
// Game //

const std::string Knee::Game::SHADER_ROOT = "./NonEuclideanEngine/shaders";

const std::string Knee::Game::RENDERABLE_GAMEOBJECT_SHADER_ROOT = Knee::Game::SHADER_ROOT + "/renderablegameobject";
const std::string Knee::Game::RENDERABLE_GAMEOBJECT_VERTEX_SHADER_PATH = Knee::Game::RENDERABLE_GAMEOBJECT_SHADER_ROOT + "/renderablegameobjectvertex.glsl";
const std::string Knee::Game::RENDERABLE_GAMEOBJECT_FRAGMENT_SHADER_PATH = Knee::Game::RENDERABLE_GAMEOBJECT_SHADER_ROOT + "/renderablegameobjectfragment.glsl";

// TODO: these should definitely be customizable
Knee::Game::Game(uint32_t windowWidth, uint32_t windowHeight) : m_renderableGameObjectShaderProgram(glm::radians(45.f), (float)windowWidth / (float)windowHeight, 0.1f, 200.f) {}

Knee::Game::~Game(){
	for(std::map<std::string, GameObject*>::iterator it = this->m_gameObjects.begin(); it != this->m_gameObjects.end(); ++it ){
		// fetch object
		GameObject* obj = it->second;
		
		// delete
		delete obj;
	}
}

Knee::Player* Knee::Game::getPlayer(){
	return &this->m_player;
}

// needs to be called AFTER application is initialized or gl context won't be present
void Knee::Game::initialize(){
	// attach shaders
	if( this->m_renderableGameObjectShaderProgram.attachShader(GL_VERTEX_SHADER, Knee::Game::RENDERABLE_GAMEOBJECT_VERTEX_SHADER_PATH) < 0 ){
		std::cout << Knee::ERROR_PREFACE << "error attaching vertex shader" << std::endl;
	}
	
	if( this->m_renderableGameObjectShaderProgram.attachShader(GL_FRAGMENT_SHADER, Knee::Game::RENDERABLE_GAMEOBJECT_FRAGMENT_SHADER_PATH) < 0 ){
		std::cout << Knee::ERROR_PREFACE << "error attaching fragment shader" << std::endl;
	}
	
	// compile
	if( this->m_renderableGameObjectShaderProgram.compile() < 0 ){
		std::cout << Knee::ERROR_PREFACE << "error compiling m_renderableGameObjectShaderProgram" << std::endl;
	}
}

Knee::GameObject* Knee::Game::getGameObject(std::string id){
	try {
		Knee::GameObject* obj = this->m_gameObjects.at(id);
	
		return obj;
	} catch( const std::out_of_range e ){
		// this is thrown when there is no element that exists at the id, so return null
		return NULL;
	}
}

void Knee::Game::addGameObject(std::string id, GameObject* obj){
	if(obj == NULL || id.length() < 1) return;
	
	// add to game objects by id
	this->m_gameObjects[id] = obj;
}

void Knee::Game::addRenderableGameObject(std::string id, RenderableGameObject* obj){
	if(obj == NULL || id.length() < 1) return;
	
	// push to renderable objects
	this->m_renderableGameObjects.push_back(obj);
	
	// cast to GameObject
	GameObject* gameObj = static_cast<GameObject*>(obj);
	
	this->addGameObject(id, gameObj);
}

// NOTE: this resets the delta timer
void Knee::Game::updateGameObjects(double delta){
	// iterate through each game object & update
	for(std::map<std::string, GameObject*>::iterator it = this->m_gameObjects.begin(); it != this->m_gameObjects.end(); ++it ){
		// fetch object
		GameObject* obj = it->second;
		
		// update
		obj->update(delta);
	}
}

void Knee::Game::updatePlayer(double delta){
	this->getPlayer()->update(delta);
}

void Knee::Game::renderRenderableGameObjects(){
	// iterate through each renderable game object and update
	for(uint32_t i = 0; i < this->m_renderableGameObjects.size(); i++){
		// fetch object
		RenderableGameObject* obj = this->m_renderableGameObjects.at(i);
		
		// cast down to RenderableObject* and render
		this->m_renderableGameObjectShaderProgram.drawRenderableObject(obj->asRenderableObject());
	}
}

void Knee::Game::renderScene(){
	glEnable(GL_DEPTH_TEST);

	// clear color + depth
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// update camera with latest player position
	this->updateCamera();
	
	// draw renderable objects
	this->renderRenderableGameObjects();
}

Knee::PerspectiveCamera* Knee::Game::getCamera(){
	return this->m_renderableGameObjectShaderProgram.getCamera();
}

void Knee::Game::updateCamera(){
	// set position according to player position
	this->getCamera()->setPosition(this->m_player.getPosition());
	
	// ditto on rotation
	this->getCamera()->setRotation(this->m_player.getRotation());
	
	// camera's vp matrix updates itself, nothing else to do
}

void Knee::Game::processEvent(SDL_Event& event){
	// forward to player's input handler
	this->getPlayer()->getInputHandler()->processEvent(event);
}