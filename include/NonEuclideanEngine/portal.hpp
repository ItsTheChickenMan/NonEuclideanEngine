#pragma once

#include <NonEuclideanEngine/misc.hpp>
#include <NonEuclideanEngine/gameobjects.hpp>
#include <NonEuclideanEngine/player.hpp>

namespace Knee {
	// a "visual portal" is a surface "paired" to another visual portal.  the portal renders what would be seen through it if light travelled through the pair of portals, or in other words, it "looks" into the paired portal
	// this effect is only visual, and does not interact with the player
	class VisualPortal : public RenderableStaticGameObject {
		// how many times to re-render the world when looking at our own portal
		// basically, how many portals deep we want an infinite hallway of our own portal to be
		// higher the number --> greater the performance dip
		// TODO: add checks to only recursively render if we're positive that we're looking at our own portal
		// TODO: add checks to limit the amount of objects we have to re-render
		const static uint32_t RECURSIVE_WORLD_RENDER_COUNT = 16;

		// the brightness that the last recurse's portal should have
		// setting this less than 1 will make the recursively rendered portals progressively darker the further they are from the actual portal
		// currently disabling this because the effect looks pretty lame and makes the portal teleportation very obvious
		constexpr static float LAST_RECURSE_BRIGHTNESS = 1.0f;

		// this is the actual brightness that each portal should be rendered with in order for the last portal to have a brightness of LAST_RECURSE_BRIGHTNESS
		constexpr static float RECURSE_PORTAL_BRIGHTNESS = (float)pow(VisualPortal::LAST_RECURSE_BRIGHTNESS, 1.0 / (double)(VisualPortal::RECURSIVE_WORLD_RENDER_COUNT+1));

		// the paired portal used to determine what the camera should see when viewing this portal.  a paired portal does not have to pair with this portal in order to work
		// a portal can also pair with itself, which is effectively the same as not existing at all (won't be rendered).  this can be useful for portals that you want to use as an output for another portal but you don't want to pair back (one way hallway sort of effect)
		Knee::VisualPortal* m_pair = NULL;

		// TODO: it doesn't seem like there's any need to have two framebuffers per portal, might make more sense to have two global framebuffers
			// however in the future this might make more sense when we have to render other portals through portals, so keeping this for now
		// framebuffer used for primary rendering
		Knee::Framebuffer2D* m_mainFramebuffer;
		
		// framebuffer used to flip between when rendering ourselves
		Knee::Framebuffer2D* m_auxFramebuffer;

		public:
			VisualPortal(Knee::VertexData* vertexData, uint32_t screenWidth, uint32_t screenHeight);
			~VisualPortal();

			Knee::RenderableStaticGameObject* asRenderableStaticGameObject();

			// pair another portal with this portal.  can pass self in order to create a blank portal
			void pair(Knee::VisualPortal* portal);

			// get the transformation required to move any object from their current position relative to this portal to the same position relative to the paired portal
			Knee::GeneralObject getPairSpaceTransformation();

			void getVertices(glm::vec3& topLeft, glm::vec3& topRight, glm::vec3& bottomLeft, glm::vec3& bottomRight);
			bool isVisible(Knee::PerspectiveCamera* camera);

			void loadPortalTexture(std::vector<RenderableObject*>* renderableObjects, Knee::RenderableObjectShaderProgram* renderableObjectShaderProgramWithDepth);

			void draw();

			bool hasPair();
			bool isOwnPair();

			void setBrightness(float brightness);
	};

	class Portal : public VisualPortal {
		// paired portal
		Portal* m_pair = NULL;

		public:
			Portal(Knee::VertexData* vertexData, uint32_t screenWidth, uint32_t screenHeight);

			// return self as a visual portal
			VisualPortal* asVisualPortal();

			// pair another portal to this one
			void pair(Portal* portal);

			// check if we need to move the player through this portal
			bool checkPlayer(Knee::Player* player, double delta);
	};
}