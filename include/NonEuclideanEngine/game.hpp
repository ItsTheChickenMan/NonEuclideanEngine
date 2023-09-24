#pragma once

#include <NonEuclideanEngine/misc.hpp>
#include <NonEuclideanEngine/shader.hpp>
#include <NonEuclideanEngine/gameobjects.hpp>
#include <NonEuclideanEngine/player.hpp>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include <vector>
#include <map>
#include <unordered_map>

namespace Knee {
	class Game {
		// shader paths //
		static const std::string SHADER_ROOT;
		static const std::string RENDERABLE_GAMEOBJECT_SHADER_ROOT;
		static const std::string RENDERABLE_GAMEOBJECT_VERTEX_SHADER_PATH;
		static const std::string RENDERABLE_GAMEOBJECT_FRAGMENT_SHADER_PATH;
		static const std::string VISUAL_PORTAL_SHADER_ROOT;
		static const std::string VISUAL_PORTAL_VERTEX_SHADER_PATH;
		static const std::string VISUAL_PORTAL_FRAGMENT_SHADER_PATH;
		static const std::string RENDERABLE_GAMEOBJECT_WITH_DEPTH_SHADER_ROOT;
		static const std::string RENDERABLE_GAMEOBJECT_WITH_DEPTH_VERTEX_SHADER_PATH;
		static const std::string RENDERABLE_GAMEOBJECT_WITH_DEPTH_FRAGMENT_SHADER_PATH;
		
		// the player
		Knee::Player m_player;
		
		// map of all static game objects, mapped by id
		std::map<std::string, StaticGameObject*> m_staticGameObjects;

		// map of all game objects, mapped by id
		std::map<std::string, GameObject*> m_gameObjects;
		
		// list of all renderable objects, added to whenever a type of renderable game object is added
		std::vector<RenderableObject*> m_renderableGameObjects;
		
		// map of all visual portals, mapped by id
		std::vector<VisualPortal*> m_visualPortals;

		// shaders
		Knee::RenderableObjectShaderProgram m_renderableGameObjectShaderProgram;
		Knee::RenderableObjectShaderProgram m_visualPortalShaderProgram;
		Knee::RenderableObjectShaderProgram m_renderableGameObjectWithDepthShaderProgram;

		public:
			Game(uint32_t, uint32_t);
			~Game();
		
			Knee::Player* getPlayer();
			
			void initialize();
			
			Knee::StaticGameObject* getStaticGameObject(std::string);
			Knee::GameObject* getGameObject(std::string);
			
			void addStaticGameObject(std::string id, StaticGameObject* obj);
			void addGameObject(std::string id, GameObject* obj);

			void addRenderableStaticGameObject(std::string id, RenderableStaticGameObject* obj);
			void addRenderableGameObject(std::string id, RenderableGameObject* obj);
			
			void addVisualPortal(std::string id, VisualPortal* portal);

			void updateGameObjects(double);
			void updatePlayer(double);
			
			// renders both static and non-static game objects
			void renderAllRenderableGameObjects();
			void updateVisualPortals();

			void renderScene();
			
			Knee::PerspectiveCamera* getPlayerCamera();
			void updateCamera();
			
			void processEvent(SDL_Event&);
	};
}