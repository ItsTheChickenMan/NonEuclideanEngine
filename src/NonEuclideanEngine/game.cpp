#include <NonEuclideanEngine/gameobjects.hpp>
#include <NonEuclideanEngine/player.hpp>
#include <NonEuclideanEngine/game.hpp>
#include <NonEuclideanEngine/shader.hpp>

#include <iostream>

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