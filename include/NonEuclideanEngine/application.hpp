// application management

#pragma once

// includes

// NOTE: this has to be included first here so as to not cause conflict with SDL
#include <glad/glad.h>

#include <cstdint>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <NonEuclideanEngine/shader.hpp>

namespace Knee {
	// class definitions
	class Application {
		// MEMBERS //
		
		// window + opengl context
		SDL_Window* m_window;
		SDL_GLContext m_glContext;
		
		// if the application should quit
		// up to the programmer to actually quit in response to this
		bool m_shouldQuit = false;
		
		// METHODS //
		private:
		
		protected:
		
		public:
			Application();
			~Application();
			
			void initialize(std::string, uint32_t, uint32_t);
			void quit();
			
			void processEvents();
			void updateWindow();
			
			bool shouldQuit();
	};
	
	class NonEuclideanApplication : public Application {
		// MEMBERS //
		
		// general shaders
		ShaderProgram m_worldShader;
		
		// ne specific shaders
		ShaderProgram m_portalShader;
		
		public:
			// METHODS //
			NonEuclideanApplication();
			~NonEuclideanApplication();
			
			
	};
}