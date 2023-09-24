#pragma once

#include <NonEuclideanEngine/misc.hpp>
#include <NonEuclideanEngine/shader.hpp>

#include <cmath>

namespace Knee {
	// classes //
	
	// a game object that does not move and is not affected by physics, but can still interact with other game objects.
	class StaticGameObject : public virtual GeneralObject {
		public:
			StaticGameObject();
			virtual ~StaticGameObject();

			GeneralObject* asGeneralObject();
	};
	
	// like a static game object, but now you can see it!
	class RenderableStaticGameObject : public virtual StaticGameObject, public RenderableObject {
		public:
			RenderableStaticGameObject(Knee::VertexData*, Knee::Texture2D*);
			virtual ~RenderableStaticGameObject();

			Knee::RenderableObject* asRenderableObject();
			Knee::StaticGameObject* asStaticGameObject();
	};
	
	// a game object that can physically interact with a game and be affected by physics.
	// essentially a StaticGameObject, but no longer static (crazy)
	class GameObject : public virtual StaticGameObject {
		// physics
		double m_mass = 0.0;
		glm::vec3 m_velocity = glm::vec3(0);
		
		public:
			GameObject();
			GameObject(double);
			virtual ~GameObject();
			
			Knee::StaticGameObject* asStaticGameObject();

			double getMass();
			void setMass(double);
		
			glm::vec3 getVelocity();
			
			// set the velocity to some value, ignoring mass
			void setVelocity(glm::vec3);
			
			// change velocity by some value, ignoring mass
			void changeVelocity(glm::vec3);
			
			// apply an impulse force to this object
			void impulse(glm::vec3);
			
			// change the object's position based on the provided change in time
			virtual void update(double);
	};
	
	// a game object that can be rendered in the scene.
	class RenderableGameObject : public GameObject, public RenderableStaticGameObject {
		// might be some extra data in here at some point?
		
		public:
			RenderableGameObject(Knee::VertexData*, Knee::Texture2D*);

			GameObject* asGameObject();	
	};
	
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
		constexpr static float LAST_RECURSE_BRIGHTNESS = 0.3f;

		// this is the actual brightness that each portal should be rendered with in order for the last portal to have a brightness of LAST_RECURSE_BRIGHTNESS
		constexpr static float RECURSE_PORTAL_BRIGHTNESS = (float)pow(VisualPortal::LAST_RECURSE_BRIGHTNESS, 1.0 / (double)(VisualPortal::RECURSIVE_WORLD_RENDER_COUNT+1));

		// the paired portal used to determine what the camera should see when viewing this portal.  a paired portal does not have to pair with this portal in order to work
		// a portal can also pair with itself, which is effectively the same as not existing at all (won't be rendered).  this can be useful for portals that you want to use as an output for another portal but you don't want to pair back (one way hallway sort of effect)
		Knee::VisualPortal* m_pair;

		// framebuffer used for primary rendering
		Knee::Framebuffer2D* m_mainFramebuffer;
		
		// framebuffer used to flip between when rendering ourselves
		Knee::Framebuffer2D* m_auxFramebuffer;

		public:
			VisualPortal(Knee::VertexData* vertexData, uint32_t screenWidth, uint32_t screenHeight);
			~VisualPortal();

			Knee::RenderableStaticGameObject* asRenderableStaticGameObject();

			// pair another portal with this portal.  can pass self in order to create a blank portal
			void pairVisualPortal(Knee::VisualPortal* portal);

			void loadPortalTexture(std::vector<RenderableObject*>* renderableObjects, Knee::RenderableObjectShaderProgram* renderableObjectShaderProgramWithDepth);

			void draw();

			bool isOwnPair();

			void setBrightness(float brightness);
	};
}