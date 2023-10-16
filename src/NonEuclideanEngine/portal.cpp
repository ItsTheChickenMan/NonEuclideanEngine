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

void Knee::VisualPortal::getVertices(glm::vec3& topLeft, glm::vec3& topRight, glm::vec3& bottomLeft, glm::vec3& bottomRight){
	glm::vec3 planeSize = glm::vec3(this->getScale().x, this->getScale().y, 0);

	topLeft = -planeSize/2.f; // top left
	topRight = topLeft + glm::vec3(planeSize.x, 0, 0); // top right
	bottomRight = planeSize/2.f; // bottom right
	bottomLeft = bottomRight - glm::vec3(planeSize.x, 0, 0); // bottom left

	// rotate + translate
	glm::mat4 rotationMatrix = this->getRotationMatrix();

	topLeft = glm::vec3(rotationMatrix * glm::vec4(topLeft, 1));
	topRight = glm::vec3(rotationMatrix * glm::vec4(topRight, 1));
	bottomLeft = glm::vec3(rotationMatrix * glm::vec4(bottomLeft, 1));
	bottomRight = glm::vec3(rotationMatrix * glm::vec4(bottomRight, 1));

	topLeft += this->getPosition();
	topRight += this->getPosition();
	bottomLeft += this->getPosition();
	bottomRight += this->getPosition();
}

bool Knee::VisualPortal::isVisible(Knee::PerspectiveCamera* camera){
	// get portal vertices
	glm::vec3 vertices[4];

	this->getVertices(vertices[0], vertices[1], vertices[3], vertices[2]);

	glm::vec3 ndc[4];

	// transform to NDC and check for visiblity
	for(uint32_t i = 0; i < 4; i++){
		glm::vec4 v = camera->getViewProjectionMatrix() * glm::vec4(vertices[i], 1);

		// perspective division
		v /= v.w;

		bool visible = true;

		// check that v falls inside range of -1 to 1
		for(uint32_t j = 0; j < 4; j++){
			float c = v[j];

			if(c < -1.0 || c > 1.0){
				// vertex is visible, no need to continue
				visible = false;
				break;
			}
		}

		if(visible) return true;

		// store for use in other checks
		ndc[i] = glm::vec3(v);
	}

	glm::vec3 ndcPosition = glm::vec3(0);
	glm::vec3 ndcSize = glm::vec3(2, 2, 0);

	// check that each of the four sections are satisfied for enveloping
	bool envelopingCases[] = {false, false, false, false};

	// check edges
	// this also checks for enveloping, when no edges intersect but the entirety of NDC is within the portal's screen area
	for(uint32_t i = 0; i < 4; i++){
		glm::vec3 start = ndc[i];
		glm::vec3 end = ndc[(i+1)%4];

		if(Knee::MathUtils::lineSegmentIntersectingOrWithinAABB(
			start, end,
			ndcPosition,
			ndcSize
		)){
			return true;
		}

		// check for enveloping
		if(start.z > 0.0){
			if(start.x <= -1.0 && start.y <= -1.0){
				envelopingCases[0] = true;
			} else if(start.x >= 1.0 && start.y <= -1.0){
				envelopingCases[1] = true;
			} else if(start.x <= -1.0 && start.y >= 1.0){
				envelopingCases[2] = true;
			} else if(start.x >= 1.0 && start.y >= 1.0){
				envelopingCases[3] = true;
			}
		}
	}

	// check special enveloping case
	bool enveloping = true;

	for(uint32_t i = 0; (i < 4) && enveloping; i++){
		enveloping &= envelopingCases[i];
	}

	if(enveloping){
		return true;
	}

	// log coordinates
	if(this->getPosition().z > 0){
		std::cout << "(" << std::endl;

		for(uint32_t i = 0; i < 4; i++){
			std::cout << "\t" << ndc[i].x << ", " << ndc[i].y << "," << ndc[i].z << std::endl;
		}

		std::cout << ")\n" << std::endl;
	}

	return false;
}

// loads the texture for the visual portal so it can be used for rendering
void Knee::VisualPortal::loadPortalTexture(std::vector<RenderableObject*>* renderableObjects, Knee::RenderableObjectShaderProgram* renderableObjectShaderProgramWithDepth){
	// FIXME: sometimes there will be a frame of the scene from a weird angle, could be a lot of things but I'm assuming it stems from portals

	// if we have no pair, do nothing
	if(!this->hasPair()) return;

	// if we're paired to ourselves, do nothing
	if(this->isOwnPair()) return;
	
	// then, check that portal is on screen at all
	// easy way to do this is transform all four vertices by mvp and check that at least one is within screen coords
	
	// get reference to camera
	// this should be shared across all shader programs, so getting our own is okay
	Knee::PerspectiveCamera* camera = this->getShaderProgram()->getCamera();

	// check visibility, if not visible from camera then do nothing
	if(!this->isVisible(camera)){
		return;
	}

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
	for(int32_t i = transformations.size()-1; i >= 0; i--){
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

	camera->copyValues(cameraTransformation);
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

bool Knee::VisualPortal::hasPair(){
	return this->m_pair != NULL;
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