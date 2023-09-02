#pragma once

#include <NonEuclideanEngine/shader.hpp>
#include <NonEuclideanEngine/misc.hpp>

#include <glm/glm.hpp>
#include <vector>
#include <map>

namespace Knee {
	// classes //
	
	// a game object that can physically interact with a game.  pretty much just GeneralObjects with extra physics and some extra data
	class GameObject : public virtual GeneralObject {
		// physics
		double m_mass = 0.0;
		glm::vec3 m_velocity = glm::vec3(0);
		
		public:
			GameObject();
			GameObject(double);
			virtual ~GameObject();
			
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
			void update(double);
	};
	
	// a game object that the renderer uses as a camera.  also contains additional data related to player interaction
	class Player : public GameObject {
		// data in here eventually...
		
		public:
			Player();
			Player(double);
	};
	
	// a game object that can be rendered in the scene.
	class RenderableGameObject : public GameObject, public RenderableObject {
		// might be some extra data in here at some point?
		
		public:
			RenderableGameObject(Knee::VertexData*);
		
			Knee::RenderableObject* asRenderableObject();
	};
	
	class Game {
		// shader paths //
		static const std::string SHADER_ROOT;
		static const std::string RENDERABLE_GAMEOBJECT_SHADER_ROOT;
		static const std::string RENDERABLE_GAMEOBJECT_VERTEX_SHADER_PATH;
		static const std::string RENDERABLE_GAMEOBJECT_FRAGMENT_SHADER_PATH;
	
		// the player
		Knee::Player m_player;
		
		// map of all game objects, mapped by id
		std::map<std::string, GameObject*> m_gameObjects;
		
		// list of all renderable game objects
		// every object in this list should also be in m_gameObjects, therefore, this list should only be used for rendering and all other logic should use the m_gameObjects list
		std::vector<RenderableGameObject*> m_renderableGameObjects;
		
		// shaders
		Knee::RenderableObjectShaderProgram m_renderableGameObjectShaderProgram;
		
		public:
			Game(uint32_t, uint32_t);
			~Game();
		
			Knee::Player* getPlayer();
			
			void initialize();
			
			Knee::GameObject* getGameObject(std::string);
			
			void addGameObject(std::string, GameObject*);
			void addRenderableGameObject(std::string, RenderableGameObject*);
			
			void updateGameObjects(double);
			
			void renderRenderableGameObjects();
			void renderScene();
			
			void updateCamera();
	};
}