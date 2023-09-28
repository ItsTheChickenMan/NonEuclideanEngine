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
}