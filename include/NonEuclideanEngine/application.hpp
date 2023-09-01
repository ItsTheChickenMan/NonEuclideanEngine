// application management

#pragma once

// includes

// NOTE: this has to be included first here so as to not cause conflict with SDL
#include <glad/glad.h>

#include <cstdint>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <NonEuclideanEngine/game.hpp>
#include <NonEuclideanEngine/shader.hpp>

namespace Knee {
	// pretty much just a shell class to get the window and events running properly, and for that reason has no game instance or shaders.
	class Application {
		// MEMBERS //
		
		// window + opengl context
		SDL_Window* m_window;
		SDL_GLContext m_glContext;
		
		// if the application should quit
		// up to the programmer to actually quit in response to this
		bool m_shouldQuit = false;
		
		// METHODS //
		public:
			Application();
			~Application();
			
			void initialize(std::string, uint32_t, uint32_t);
			void quit();
			
			int32_t processApplicationEvent(SDL_Event);
			virtual void processEvents();
			void updateWindow();
			
			bool shouldQuit();
			
			int32_t setSwapInterval(int32_t);
			
	};
	
	class NonEuclideanApplication : public Application {
		// MEMBERS //
		
		// game instance
		Knee::Game m_game;
		
		// general shaders
		ShaderProgram m_worldShader;
		
		// ne specific shaders
		ShaderProgram m_portalShader;
		
		public:
			// METHODS //
			NonEuclideanApplication();
			~NonEuclideanApplication();
			
			void processEvents();
			void update();
	};
}