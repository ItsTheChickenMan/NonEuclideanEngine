#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Knee {
	// any object/thing that exists within the game.
	class GameObject {
		// model values
		
		// XYZ position
		glm::vec3 position;
		
		// XYZ Euler Rotation (applied in ZYX order)
		glm::vec3 rotation;
		
		// XYZ scale
		glm::vec3 scale;
		
		glm::mat4 modelMatrix;
		
		public:
			void calculateModelMatrix();
			
		public:
			GameObject();
			GameObject(glm::vec3 p, glm::vec3 r, glm::vec3 s);
			
			~GameObject();
			
	};
	
	// a game object that is able to be rendered.  in addition to data that a game object contains, this has vertex data, material info, and other attributes/methods to enable rendering through a Knee::ShaderProgram.
	class RenderableGameObject : public GameObject {
		
	};
	
	class Game {
		// MEMBERS //
		std::vector<GameObject*> m_gameObjects;
		std::vector<RenderableGameObject*> m_renderableGameObjects;
		
		public:
			Game();
			~Game();
			
			
	};
}