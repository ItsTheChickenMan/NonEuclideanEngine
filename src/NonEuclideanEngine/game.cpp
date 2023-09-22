#include <NonEuclideanEngine/game.hpp>
#include <NonEuclideanEngine/shader.hpp>

#include <iostream>

#include <glm/gtx/norm.hpp>

// StaticGameObject //

Knee::StaticGameObject::StaticGameObject(){
}

Knee::StaticGameObject::~StaticGameObject(){
}

Knee::GeneralObject* Knee::StaticGameObject::asGeneralObject(){
	return static_cast<Knee::GeneralObject*>(this);
}

// RenderableStaticGameObject //

Knee::RenderableStaticGameObject::RenderableStaticGameObject(Knee::VertexData* vertexData, Knee::Texture2D* texture) : RenderableObject(vertexData, texture, NULL) {
}

Knee::RenderableStaticGameObject::~RenderableStaticGameObject(){
}

Knee::RenderableObject* Knee::RenderableStaticGameObject::asRenderableObject(){
	return static_cast<RenderableObject*>(this);
}

Knee::StaticGameObject* Knee::RenderableStaticGameObject::asStaticGameObject(){
	return static_cast<StaticGameObject*>(this);
}

// -------------------- //
// GameObject //

Knee::GameObject::GameObject() : m_mass(1.0) {}
Knee::GameObject::GameObject(double mass) : m_mass(mass) {}

Knee::GameObject::~GameObject(){}

Knee::StaticGameObject* Knee::GameObject::asStaticGameObject(){
	return static_cast<Knee::StaticGameObject*>(this);
}

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

Knee::RenderableGameObject::RenderableGameObject(Knee::VertexData* vertexData, Knee::Texture2D* texture) : GameObject(), RenderableStaticGameObject(vertexData, texture){}

Knee::GameObject* Knee::RenderableGameObject::asGameObject(){
	return static_cast<Knee::GameObject*>(this);
}

// -------------------- //
// VisualPortal //

Knee::VisualPortal::VisualPortal(Knee::VertexData* vertexData, uint32_t screenWidth, uint32_t screenHeight) : 
	RenderableStaticGameObject(
		vertexData,
		NULL
	)
{
	// create texture for m_texture
	this->m_texture = new Knee::Texture2D(screenWidth, screenHeight, GL_UNSIGNED_BYTE);

	// framebuffer
	glGenFramebuffers(1, &this->m_mainFramebuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, this->m_mainFramebuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->m_texture->getGLTexture(), 0);

	// renderbuffer
	glGenRenderbuffers(1, &this->m_mainRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->m_mainRenderbuffer); 
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);  

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->m_mainRenderbuffer);

	// create secondary texture for self rendering
	this->m_secondaryTexture = new Knee::Texture2D(screenWidth, screenHeight, GL_UNSIGNED_BYTE);

	// aux framebuffer
	glGenFramebuffers(1, &this->m_auxFramebuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, this->m_auxFramebuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->m_secondaryTexture->getGLTexture(), 0);

	// aux renderbuffer
	glGenRenderbuffers(1, &this->m_auxRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->m_auxRenderbuffer); 
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);  

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->m_auxRenderbuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->m_auxTexture = new Knee::Texture2D(screenWidth, screenHeight, GL_UNSIGNED_BYTE);
};

Knee::VisualPortal::~VisualPortal(){
	// free texture
	delete this->m_texture;

	// free secondary texture
	delete this->m_secondaryTexture;

	delete this->m_auxTexture;
}

Knee::RenderableStaticGameObject* Knee::VisualPortal::asRenderableStaticGameObject(){
	return static_cast<Knee::RenderableStaticGameObject*>(this);
}

void Knee::VisualPortal::pairVisualPortal(Knee::VisualPortal* portal){
	// assign pair
	this->m_pair = portal;
}

#include <glm/gtx/string_cast.hpp>

// loads the texture for the visual portal so it can be used for rendering
void Knee::VisualPortal::loadPortalTexture(std::vector<RenderableObject*>* renderableObjects, Knee::RenderableObjectShaderProgram* renderableObjectShaderProgramWithDepth){
	// if we're paired to ourselves, do nothing
	if(this->m_pair == this) return;
	
	// difference transformation
	Knee::GeneralObject portalSpaceTransformation = this->asGeneralObject()->getInverseGeneralObject();
	portalSpaceTransformation.applyGeneralObjectTransformation(*this->m_pair->asGeneralObject());

	Knee::GeneralObject inversePortalSpaceTransformation = portalSpaceTransformation.getInverseGeneralObject();

	// total transformation
	Knee::GeneralObject totalTransformation;

	// store camera	
	Knee::PerspectiveCamera* camera = this->getShaderProgram()->getCamera();

	// set u_additionalModel to identity
	this->getShaderProgram()->setUniformMat4("u_additionalModel", glm::mat4(1));
	
	// copy current texture into secondary texture, since secondary texture is what we will use for rendering while our main texture will be rendered to by the framebuffer
	// we also save the current texture so we don't lose the reference
	Knee::Texture2D* mainFramebufferTexture = this->m_texture;

	// set our texture to the secondary texture
	this->setTexture(this->m_secondaryTexture);

	// to render the portal properly when it's looking at itself, we render the world a few more times (ideally a small number) and then for the rest copy the existing render
	// recursively render the world to render our own portal
	// TODO: this should ONLY happen when we're confident we're looking at this portal through it, otherwise this is a total waste of time
	for(uint32_t i = 0; i < Knee::VisualPortal::RECURSIVE_WORLD_RENDER_COUNT+1; i++){
		// if this isn't the first pass
		if(i > 0){
			// bind auxiliary framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, this->m_auxFramebuffer);
		} else {
			// bind main framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, this->m_mainFramebuffer);
		}
		
		// clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// temporarily move camera
		camera->applyGeneralObjectTransformation(portalSpaceTransformation);
		camera->updateViewProjectionMatrix();

		// add to total transformation
		totalTransformation.applyGeneralObjectTransformation(portalSpaceTransformation);

		// render each object from camera's new perspective
		for(uint32_t i = 0; i < renderableObjects->size(); i++){
			RenderableObject* obj = renderableObjects->at(i);

			// ignore if the portal is our pair
			if(obj == this->m_pair->asRenderableObject() || obj == this->asRenderableObject()) continue;
			
			// render
			obj->draw();
		}

		// if this isn't the first pass
		/*if(i > 0){
			// bind main framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, this->m_mainFramebuffer);

			// move camera up to place portal in the right place
			// TODO: dear lord there has to be a faster way to do this
			camera->applyGeneralObjectTransformation(inversePortalSpaceTransformation);
			camera->updateViewProjectionMatrix();

			// disable depth writing so that portals don't override portals in the back
			glDepthMask(GL_FALSE);

			// draw just portal
			this->draw();

			// reenable depth writing
			glDepthMask(GL_TRUE);

			// move camera back
			camera->applyGeneralObjectTransformation(portalSpaceTransformation);
		}*/
	}

	Knee::GeneralObject inverseTotalTransformation = totalTransformation.getInverseGeneralObject();

	camera->applyGeneralObjectTransformation(inverseTotalTransformation);
	camera->updateViewProjectionMatrix();

	// for additional, lower cost detail, we can take the texture that was rendered to the last portal in the line and recursively re-render it at further values
	if(Knee::VisualPortal::RECURSIVE_PORTAL_RENDER_COUNT > 0){
		// bind to aux framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, this->m_auxFramebuffer);

		// translate the rendered portal to the furthest portal of the recursive rendering
		Knee::GeneralObject t = inverseTotalTransformation;

		t.applyGeneralObjectTransformation(portalSpaceTransformation);

		Knee::GeneralObject it = t.getInverseGeneralObject();

		this->applyGeneralObjectTransformation(t);

		// apply additional transformation to where new portal is placed
		//this->getShaderProgram()->setUniformMat4("u_additionalModel", t.getModelMatrix());
		this->getShaderProgram()->setUniformMat4("u_additionalModel", inversePortalSpaceTransformation.getModelMatrix());

		// 1. render portal to aux framebuffer
		// 2. copy framebuffer to aux texture, use aux texture as portal texture
		// 3. repeat for n steps
		// 4. render final portal to main framebuffer using secondary texture

		this->setTexture(this->m_auxTexture);

		// temporarily allow equal values to pass
		// this allows us to repeatedly write over a previous polygon below
		glDepthFunc(GL_LEQUAL);

		// also disable writing just for convenience
		glDepthMask(GL_FALSE);

		glBindTexture(GL_TEXTURE_2D, this->m_auxTexture->getGLTexture());

		for(uint32_t i = 0; i < Knee::VisualPortal::RECURSIVE_PORTAL_RENDER_COUNT; i++){
			// this will copy pixels from the currently bound framebuffer into the secondary texture
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, this->m_auxTexture->getWidth(), this->m_auxTexture->getHeight(), 0);

			this->draw();
		}

		// switch depth function back to less
		glDepthFunc(GL_LESS);

		// reenable depth writing
		glDepthMask(GL_TRUE);

		this->setTexture(this->m_secondaryTexture);

		this->applyGeneralObjectTransformation(it);
		
		this->getShaderProgram()->setUniformMat4("u_additionalModel", glm::mat4(1));

		// bind main framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, this->m_mainFramebuffer);

		camera->applyGeneralObjectTransformation(totalTransformation);
		camera->applyGeneralObjectTransformation(inversePortalSpaceTransformation);
		camera->updateViewProjectionMatrix();

		// disable depth writing so that portals don't override portals in the back
		glDepthMask(GL_FALSE);

		// draw just portal
		this->draw();

		// reenable depth writing
		glDepthMask(GL_TRUE);

		// move camera back
		camera->applyGeneralObjectTransformation(portalSpaceTransformation);
		camera->applyGeneralObjectTransformation(inverseTotalTransformation);
		camera->updateViewProjectionMatrix();
	}

	// set texture back to default
	this->setTexture(mainFramebufferTexture);

	// go back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Knee::VisualPortal::draw(){
	// save model matrix
	glm::mat4 model = this->getModelMatrix();

	// set model matrix to identity
	this->setModelMatrix(glm::mat4(1));

	// manually assign model
	this->getShaderProgram()->setUniformMat4("u_model", model);

	// draw
	RenderableStaticGameObject::draw();

	// set model matrix back to original
	this->setModelMatrix(model);
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

const std::string Knee::Game::VISUAL_PORTAL_SHADER_ROOT = Knee::Game::SHADER_ROOT + "/visualportal";
const std::string Knee::Game::VISUAL_PORTAL_VERTEX_SHADER_PATH = Knee::Game::VISUAL_PORTAL_SHADER_ROOT + "/visualportalvertex.glsl";
const std::string Knee::Game::VISUAL_PORTAL_FRAGMENT_SHADER_PATH = Knee::Game::VISUAL_PORTAL_SHADER_ROOT + "/visualportalfragment.glsl";

const std::string Knee::Game::RENDERABLE_GAMEOBJECT_WITH_DEPTH_SHADER_ROOT = Knee::Game::SHADER_ROOT + "/renderablegameobjectwithdepth";
const std::string Knee::Game::RENDERABLE_GAMEOBJECT_WITH_DEPTH_VERTEX_SHADER_PATH = Knee::Game::RENDERABLE_GAMEOBJECT_VERTEX_SHADER_PATH;
const std::string Knee::Game::RENDERABLE_GAMEOBJECT_WITH_DEPTH_FRAGMENT_SHADER_PATH = Knee::Game::RENDERABLE_GAMEOBJECT_WITH_DEPTH_SHADER_ROOT + "/renderablegameobjectwithdepthfragment.glsl";

// TODO: these should definitely be customizable
Knee::Game::Game(uint32_t windowWidth, uint32_t windowHeight) : 
	m_renderableGameObjectShaderProgram(glm::radians(45.f), (float)windowWidth / (float)windowHeight, 0.1f, 200.f),
	m_visualPortalShaderProgram(m_renderableGameObjectShaderProgram.getCamera()), // link camera
	m_renderableGameObjectWithDepthShaderProgram(m_renderableGameObjectShaderProgram.getCamera())  // link camera
 {}

Knee::Game::~Game(){
	/*for(std::map<std::string, GameObject*>::iterator it = this->m_gameObjects.begin(); it != this->m_gameObjects.end(); ++it ){
		// fetch object
		GameObject* obj = it->second;
		
		// delete
		delete obj;
	}*/
}

Knee::Player* Knee::Game::getPlayer(){
	return &this->m_player;
}

// needs to be called AFTER application is initialized or gl context won't be present
void Knee::Game::initialize(){
	// attach renderable object shaders
	if( this->m_renderableGameObjectShaderProgram.attachShader(GL_VERTEX_SHADER, Knee::Game::RENDERABLE_GAMEOBJECT_VERTEX_SHADER_PATH) < 0 ){
		std::cout << Knee::ERROR_PREFACE << "error attaching renderable gameobject vertex shader" << std::endl;
	}
	
	if( this->m_renderableGameObjectShaderProgram.attachShader(GL_FRAGMENT_SHADER, Knee::Game::RENDERABLE_GAMEOBJECT_FRAGMENT_SHADER_PATH) < 0 ){
		std::cout << Knee::ERROR_PREFACE << "error attaching renderable gameobject fragment shader" << std::endl;
	}
	

	// attach portal shaders
	if( this->m_visualPortalShaderProgram.attachShader(GL_VERTEX_SHADER, Knee::Game::VISUAL_PORTAL_VERTEX_SHADER_PATH) < 0 ){
		std::cout << Knee::ERROR_PREFACE << "error attaching visual portal vertex shader" << std::endl;
	}

	if( this->m_visualPortalShaderProgram.attachShader(GL_FRAGMENT_SHADER, Knee::Game::VISUAL_PORTAL_FRAGMENT_SHADER_PATH) < 0 ){
		std::cout << Knee::ERROR_PREFACE << "error attaching visual portal fragment shader" << std::endl;
	}


	// attach renderable object with depth shaders
	if( this->m_renderableGameObjectWithDepthShaderProgram.attachShader(GL_VERTEX_SHADER, Knee::Game::RENDERABLE_GAMEOBJECT_WITH_DEPTH_VERTEX_SHADER_PATH) < 0 ){
		std::cout << Knee::ERROR_PREFACE << "error attaching renderable gameobject with depth vertex shader" << std::endl;
	}

	if( this->m_renderableGameObjectWithDepthShaderProgram.attachShader(GL_FRAGMENT_SHADER, Knee::Game::RENDERABLE_GAMEOBJECT_WITH_DEPTH_FRAGMENT_SHADER_PATH) < 0 ){
		std::cout << Knee::ERROR_PREFACE << "error attaching renderable gameobject with depth fragment shader" << std::endl;
	}

	// compile renderable gameobject shader program
	if( this->m_renderableGameObjectShaderProgram.compile() < 0 ){
		std::cout << Knee::ERROR_PREFACE << "error compiling m_renderableGameObjectShaderProgram" << std::endl;
	}


	// compile visual portal shader program
	if( this->m_visualPortalShaderProgram.compile() < 0){
		std::cout << Knee::ERROR_PREFACE << "error compiling m_visualPortalShaderProgram" << std::endl;
	}


	// compile renderable gameobject with depth shader program
	if( this->m_visualPortalShaderProgram.compile() < 0){
		std::cout << Knee::ERROR_PREFACE << "error compiling m_renderableGameObjectWithDepthShaderProgram" << std::endl;
	}
}

Knee::StaticGameObject* Knee::Game::getStaticGameObject(std::string id){
	try {
		Knee::StaticGameObject* obj = this->m_staticGameObjects.at(id);
	
		return obj;
	} catch( const std::out_of_range e ){
		std::cout << Knee::ERROR_PREFACE << "no StaticGameObject found with id " << id << ", returning NULL" << std::endl;

		// this is thrown when there is no element that exists at the id, so return null
		return NULL;
	}
}

Knee::GameObject* Knee::Game::getGameObject(std::string id){
	try {
		Knee::GameObject* obj = this->m_gameObjects.at(id);
	
		return obj;
	} catch( const std::out_of_range e ){
		std::cout << Knee::ERROR_PREFACE << "no GameObject found with id " << id << ", returning NULL" << std::endl;

		// this is thrown when there is no element that exists at the id, so return null
		return NULL;
	}
}

void Knee::Game::addStaticGameObject(std::string id, Knee::StaticGameObject* obj){
	if(obj == NULL || id.length() < 1) return;

	// add to static game objects by id
	this->m_staticGameObjects[id] = obj;
}

void Knee::Game::addGameObject(std::string id, Knee::GameObject* obj){
	if(obj == NULL || id.length() < 1) return;
	
	// add to game objects by id
	this->m_gameObjects[id] = obj;
}

void Knee::Game::addRenderableStaticGameObject(std::string id, Knee::RenderableStaticGameObject* obj){
	if(obj == NULL || id.length() < 1) return;

	// add shader
	obj->setShaderProgram(&this->m_renderableGameObjectShaderProgram);

	// push to renderable objects
	this->m_renderableGameObjects.push_back(obj->asRenderableObject());

	// cast to StaticGameObject
	Knee::StaticGameObject* staticGameObj = obj->asStaticGameObject();

	// add to static game objects by id
	this->addStaticGameObject(id, obj);
}

void Knee::Game::addRenderableGameObject(std::string id, Knee::RenderableGameObject* obj){
	if(obj == NULL || id.length() < 1) return;
	
	// add shader
	obj->setShaderProgram(&this->m_renderableGameObjectShaderProgram);

	// push to renderable objects
	this->m_renderableGameObjects.push_back(obj->asRenderableObject());
	
	// cast to GameObject
	Knee::GameObject* gameObj = obj->asGameObject();
	
	this->addGameObject(id, gameObj);
}

void Knee::Game::addVisualPortal(std::string id, VisualPortal* portal){
	if(portal == NULL || id.length() < 1) return;

	// push to visual portals
	this->m_visualPortals.push_back(portal);

	// cast to renderable static game object
	Knee::RenderableStaticGameObject* staticGameObj = portal->asRenderableStaticGameObject();

	this->addRenderableStaticGameObject(id, staticGameObj);

	// add shader
	// (we put this here to override the shader added by addRenderableStaticGameObject)
	portal->setShaderProgram(&this->m_visualPortalShaderProgram);
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

void Knee::Game::renderAllRenderableGameObjects(){
	// iterate through each renderable game object and update
	for(uint32_t i = 0; i < this->m_renderableGameObjects.size(); i++){
		// fetch object
		RenderableObject* obj = this->m_renderableGameObjects.at(i);
		
		// render
		obj->draw();
	}
}

void Knee::Game::updateVisualPortals(){
	// iterate through all visual portals
	for(uint32_t i = 0; i < this->m_visualPortals.size(); i++){
		// get portal
		VisualPortal* portal = this->m_visualPortals.at(i);

		// load texture
		portal->loadPortalTexture(&this->m_renderableGameObjects, &this->m_renderableGameObjectWithDepthShaderProgram);
	}
}

void Knee::Game::renderScene(){
	glEnable(GL_DEPTH_TEST);

	// clear color + depth
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// update camera with latest player position
	this->updateCamera();
	
	// update portals
	this->updateVisualPortals();

	// draw renderable objects
	this->renderAllRenderableGameObjects();
}

Knee::PerspectiveCamera* Knee::Game::getPlayerCamera(){
	return this->m_renderableGameObjectShaderProgram.getCamera();
}

void Knee::Game::updateCamera(){
	// set position according to player position
	this->getPlayerCamera()->setPosition(this->m_player.getPosition());
	
	// ditto on rotation
	this->getPlayerCamera()->setRotation(this->m_player.getRotation());
	
	// camera's vp matrix updates itself, nothing else to do
}

void Knee::Game::processEvent(SDL_Event& event){
	// forward to player's input handler
	this->getPlayer()->getInputHandler()->processEvent(event);
}