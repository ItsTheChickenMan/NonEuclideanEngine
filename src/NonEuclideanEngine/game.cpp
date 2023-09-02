#include <NonEuclideanEngine/game.hpp>
#include <NonEuclideanEngine/shader.hpp>

#include <iostream>

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

Knee::RenderableGameObject::RenderableGameObject(Knee::VertexData* vertexData) : RenderableObject(vertexData), GameObject() {}

Knee::RenderableObject* Knee::RenderableGameObject::asRenderableObject(){
	return static_cast<RenderableObject*>(this);
}

// -------------------- //
// Player //

Knee::Player::Player() : GameObject() {}

Knee::Player::Player(double mass) : GameObject(mass) {} 

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

void Knee::Game::updateCamera(){
	// set position according to player position
	this->m_renderableGameObjectShaderProgram.getCamera()->setPosition(this->m_player.getPosition());
	
	// ditto on rotation
	this->m_renderableGameObjectShaderProgram.getCamera()->setRotation(this->m_player.getRotation());
	
	// camera's vp matrix updates itself, nothing else to do
}