#include <NonEuclideanEngine/shader.hpp>
#include <NonEuclideanEngine/portal.hpp>
#include <NonEuclideanEngine/player.hpp>

#include <iostream>

// -------------------- //
// VisualPortal //

Knee::VisualPortal::VisualPortal(Knee::VertexData* vertexData, uint32_t screenWidth, uint32_t screenHeight) : 
	RenderableStaticGameObject(
		vertexData,
		NULL
	)
{
	// texture is unassigned until loadPortalTexture is called
	this->m_texture = NULL;

	// create framebuffers
	this->m_mainFramebuffer = new Knee::Framebuffer2D(screenWidth, screenHeight);
	this->m_auxFramebuffer = new Knee::Framebuffer2D(screenWidth, screenHeight);
};

Knee::VisualPortal::~VisualPortal(){
	// free main texture
	delete this->m_mainFramebuffer;

	// free aux texture
	delete this->m_auxFramebuffer;
}

Knee::RenderableStaticGameObject* Knee::VisualPortal::asRenderableStaticGameObject(){
	return static_cast<Knee::RenderableStaticGameObject*>(this);
}

void Knee::VisualPortal::pair(Knee::VisualPortal* portal){
	// assign pair
	this->m_pair = portal;
}

Knee::GeneralObject Knee::VisualPortal::getPairSpaceTransformation(){
	Knee::GeneralObject out = *this->m_pair->asGeneralObject() * this->asGeneralObject()->getInverseGeneralObject();

	return out;
}

// loads the texture for the visual portal so it can be used for rendering
void Knee::VisualPortal::loadPortalTexture(std::vector<RenderableObject*>* renderableObjects, Knee::RenderableObjectShaderProgram* renderableObjectShaderProgramWithDepth){
	// FIXME: sometimes there will be a frame of the scene from a weird angle, could be a lot of things but I'm assuming it stems from portals

	// if we're paired to ourselves, do nothing
	if(this->isOwnPair()) return;
	
	// calculate camera movement transformation
	Knee::GeneralObject pairSpaceTransformation = this->getPairSpaceTransformation();

	// calculate inverse
	Knee::GeneralObject inversePairSpaceTransformation = pairSpaceTransformation.getInverseGeneralObject();

	// initializes to 1 since we always render at least one side of the portal
	Knee::GeneralObject totalTransformation;

	// store transformations in vector
	// this is mainly so that the portals closest to the camera have the lowest floating point error (least amount of transformations from start)
	std::vector<Knee::GeneralObject> transformations;
	transformations.reserve(Knee::VisualPortal::RECURSIVE_WORLD_RENDER_COUNT);

	// calculate total transformation from recursive render requests
	for(uint32_t i = 0; i < Knee::VisualPortal::RECURSIVE_WORLD_RENDER_COUNT+1; i++){
		totalTransformation *= pairSpaceTransformation;

		transformations.push_back(totalTransformation);
	}

	// get reference to camera
	// this should be shared across all shader programs, so getting our own is okay
	Knee::PerspectiveCamera* camera = this->getShaderProgram()->getCamera();

	Knee::GeneralObject cameraTransformation = *camera->asGeneralObject();

	// apply transformation to camera
	// this moves it to the model it needs for the last portal in the line, and is then repeatedly moved back until it's at its original position
	//camera->applyTransformation(totalTransformation);
	//camera->updateViewProjectionMatrix();

	// set brightness to precalculated brightness required
	this->setBrightness(Knee::VisualPortal::RECURSE_PORTAL_BRIGHTNESS);

	// active texture info
	// we need this because we flip between the main and aux texture repeatedly
	uint32_t activeTexture = 0;

	Knee::Framebuffer2D* framebuffers[] = {this->m_mainFramebuffer, this->m_auxFramebuffer};

	// we run this for requested recurses + 1 times to make sure we render at least once
	for(int32_t i = Knee::VisualPortal::RECURSIVE_WORLD_RENDER_COUNT; i >= 0; i--){
		// get inactive texture
		uint32_t inactiveTexture = (activeTexture+1) % 2;

		// bind framebuffer to inactive texture
		framebuffers[inactiveTexture]->bind();

		// clear color + depth buffers		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// use active texture
		this->setTexture(framebuffers[activeTexture]->getTexture2D());

		// move camera
		camera->applyTransformation(transformations.at(i));
		camera->updateViewProjectionMatrix();

		// render objects
		for(uint32_t j = 0; j < renderableObjects->size(); j++){
			// get object
			Knee::RenderableObject* obj = renderableObjects->at(j);

			// don't render our pair
			if(obj == this->m_pair->asRenderableObject()) continue;

			// don't render ourselves on first pass only (active texture won't be populated)
			if(i == Knee::VisualPortal::RECURSIVE_WORLD_RENDER_COUNT && obj == this->asRenderableObject()) continue; 

			// draw object
			obj->draw();
		}

		// move camera back
		camera->copyValues(cameraTransformation);

		// flip active texture
		activeTexture = inactiveTexture;
	}

	camera->updateViewProjectionMatrix();

	// reset brightness
	this->setBrightness(1.0);

	// reset to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// set our texture to whichever was rendered to last
	// activeTexture because inactiveTexture is rendered to before being flipped to the activeTexture at the end of the for loop
	this->setTexture(framebuffers[activeTexture]->getTexture2D());
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

// -------------------- //
// Portal //

Knee::Portal::Portal(Knee::VertexData* vertexData, uint32_t screenWidth, uint32_t screenHeight) : VisualPortal(vertexData, screenWidth, screenHeight) {}

Knee::VisualPortal* Knee::Portal::asVisualPortal(){
	return static_cast<Knee::VisualPortal*>(this);
}

// pair another portal to this portal
void Knee::Portal::pair(Knee::Portal* portal){
	// pair visual portals
	Knee::VisualPortal::pair(portal->asVisualPortal());

	this->m_pair = portal;
}

// check if player needs to be moved
bool Knee::Portal::checkPlayer(Knee::Player* player, double delta){
	// quit if paired to self
	if(this->isOwnPair()) return false;

	// add slight amount to velocity to account for near
	glm::vec3 velocityAdjustment = glm::length2(player->getVelocity()) < 0.01f ? glm::vec3(0) : glm::normalize(player->getVelocity())*(player->getCamera()->getNear()/(float)delta);
	glm::vec3 adjustedVelocity = player->getVelocity() + velocityAdjustment;
	
	//std::cout << "player: " << glm::to_string(player->getVelocity() * (float)delta) << std::endl;
	//std::cout << "adjusted: " << glm::to_string(adjustedVelocity * (float)delta) << std::endl;

	// check if player is passing through this portal
	bool passingThrough = Knee::MathUtils::isLineSegmentIntersectingPlane(player->getPosition(), player->getPosition() + adjustedVelocity * (float)delta,
		this->getPosition(),
		this->getRotationMatrix(),
		glm::vec3(this->getScale().x, this->getScale().y, 0)
	);

	// if player isn't passing through, do nothing
	if(!passingThrough) return false;

	// move player to paired portal
	Knee::GeneralObject pairSpaceTransformation = this->getPairSpaceTransformation();
	
	player->applyTransformation(pairSpaceTransformation);

	adjustedVelocity += velocityAdjustment;
	
	adjustedVelocity = glm::vec3(pairSpaceTransformation.getRotationMatrix() * glm::vec4(adjustedVelocity, 1));

	// adjust velocity
	player->setVelocity(adjustedVelocity);

	// return true (player was moved)
	return true;
}