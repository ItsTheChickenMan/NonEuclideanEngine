#include <NonEuclideanEngine/shader.hpp>
#include <NonEuclideanEngine/gameobjects.hpp>

#include <iostream>

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
	// NOTE: we keep m_texture NULL until loadPortalTexture is called, which assigns it to one of the aux textures
	this->m_texture = NULL;

	// create auxiliary texture
	this->m_mainTexture = new Knee::Texture2D(screenWidth, screenHeight, GL_UNSIGNED_BYTE);

	// framebuffer
	glGenFramebuffers(1, &this->m_mainFramebuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, this->m_mainFramebuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->m_mainTexture->getGLTexture(), 0);

	// renderbuffer
	glGenRenderbuffers(1, &this->m_mainRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->m_mainRenderbuffer); 
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);  

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->m_mainRenderbuffer);

	// create secondary texture for self rendering
	this->m_auxTexture = new Knee::Texture2D(screenWidth, screenHeight, GL_UNSIGNED_BYTE);

	// aux framebuffer
	glGenFramebuffers(1, &this->m_auxFramebuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, this->m_auxFramebuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->m_auxTexture->getGLTexture(), 0);

	// aux renderbuffer
	glGenRenderbuffers(1, &this->m_auxRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->m_auxRenderbuffer); 
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);  

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->m_auxRenderbuffer);

	// back to default renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// back to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

Knee::VisualPortal::~VisualPortal(){
	// free main texture
	delete this->m_mainTexture;

	// free aux texture
	delete this->m_auxTexture;
}

Knee::RenderableStaticGameObject* Knee::VisualPortal::asRenderableStaticGameObject(){
	return static_cast<Knee::RenderableStaticGameObject*>(this);
}

void Knee::VisualPortal::pairVisualPortal(Knee::VisualPortal* portal){
	// assign pair
	this->m_pair = portal;
}

// loads the texture for the visual portal so it can be used for rendering
void Knee::VisualPortal::loadPortalTexture(std::vector<RenderableObject*>* renderableObjects, Knee::RenderableObjectShaderProgram* renderableObjectShaderProgramWithDepth){
	// if we're paired to ourselves, do nothing
	if(this->isOwnPair()) return;
	
	// calculate camera movement transformation
	Knee::GeneralObject portalSpaceTransformation = this->asGeneralObject()->getInverseGeneralObject();

	portalSpaceTransformation *= *this->m_pair->asGeneralObject();

	// calculate inverse
	Knee::GeneralObject inversePortalSpaceTransformation = portalSpaceTransformation.getInverseGeneralObject();

	// initializes to 1 since we always render at least one side of the portal
	Knee::GeneralObject totalTransformation = portalSpaceTransformation;

	// calculate total transformation from recursive render requests
	for(uint32_t i = 0; i < Knee::VisualPortal::RECURSIVE_WORLD_RENDER_COUNT; i++){
		totalTransformation *= portalSpaceTransformation;
	}

	// get reference to camera
	// this should be shared across all shader programs, so getting our own is okay
	Knee::PerspectiveCamera* camera = this->getShaderProgram()->getCamera();

	// apply transformation to camera
	// this moves it to the model it needs for the last portal in the line, and is then repeatedly moved back until it's at its original position
	camera->applyTransformation(totalTransformation);
	camera->updateViewProjectionMatrix();

	// set brightness to precalculated brightness required
	this->setBrightness(Knee::VisualPortal::RECURSE_PORTAL_BRIGHTNESS);

	// active texture info
	// we need this because we flip between the main and aux texture repeatedly
	uint32_t activeTexture = 0;

	uint32_t framebuffers[] = {this->m_mainFramebuffer, this->m_auxFramebuffer};
	Knee::Texture2D* textures[] = {this->m_mainTexture, this->m_auxTexture};

	// we run this for requested recurses + 1 times to make sure we render at least once
	for(uint32_t i = 0; i < Knee::VisualPortal::RECURSIVE_WORLD_RENDER_COUNT+1; i++){
		// get inactive texture
		uint32_t inactiveTexture = (activeTexture+1) % 2;

		// bind framebuffer to inactive texture
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[inactiveTexture]);

		// clear color + depth buffers		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// use active texture
		this->setTexture(textures[activeTexture]);

		// render objects
		for(uint32_t j = 0; j < renderableObjects->size(); j++){
			// get object
			Knee::RenderableObject* obj = renderableObjects->at(j);

			// don't render our pair
			if(obj == this->m_pair->asRenderableObject()) continue;

			// don't render ourselves on first pass only (active texture won't be populated)
			if(i == 0 && obj == this->asRenderableObject()) continue; 

			// draw object
			obj->draw();
		}

		// move camera back
		camera->applyTransformation(inversePortalSpaceTransformation);
		camera->updateViewProjectionMatrix();

		// flip active texture
		activeTexture = inactiveTexture;
	}
	
	// reset brightness
	this->setBrightness(1.0);

	// reset to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// set our texture to whichever was rendered to last
	// activeTexture because inactiveTexture is rendered to before being flipped to the activeTexture at the end of the for loop
	this->setTexture(textures[activeTexture]);
}

void Knee::VisualPortal::draw(){
	// draw nothing if we're our own pair
	if(this->isOwnPair()) return;

	// draw
	RenderableStaticGameObject::draw();
}

bool Knee::VisualPortal::isOwnPair(){
	return this->m_pair == this;
}

void Knee::VisualPortal::setBrightness(float brightness){
	this->getShaderProgram()->use();

	glUniform1f(this->getShaderProgram()->getUniformLocation("u_brightness"), brightness);
}