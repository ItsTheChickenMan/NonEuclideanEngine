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

namespace Knee {
	// pretty much just a shell class to get the window and events running properly, and for that reason has no game instance or shaders.
	class Application {
		// MEMBERS //
		
		protected:
		
			// window features
			std::string m_windowTitle;
			uint32_t m_windowWidth;
			uint32_t m_windowHeight;
			
			// window + opengl context
			SDL_Window* m_window;
			SDL_GLContext m_glContext;
			
			// if the application should quit
			// up to the programmer to actually quit in response to this
			bool m_shouldQuit = false;
		
		// METHODS //
		public:
			Application(std::string, uint32_t, uint32_t);
			~Application();
			
			virtual void initialize();
			void quit();
			
			int32_t processApplicationEvent(SDL_Event);
			virtual void processEvents();
			void updateWindow();
			
			bool shouldQuit();
			
			int32_t setSwapInterval(int32_t);
			
	};
	
	class GameApplication : public Application {
		// MEMBERS //
		
		// game instance
		Knee::Game m_game;
		
		// delta timer
		Knee::DeltaTimer m_deltaTimer;
		
		// expected delta based on max fps (defaults to 60)
		double m_expectedDelta = 1.0 / 60.0;
		
		public:
			// METHODS //
			GameApplication(std::string, uint32_t, uint32_t);
			~GameApplication();
			
			Knee::DeltaTimer* getDeltaTimer();
			Knee::Game* getGameInstance();
			
			void initialize();
			void setMaxFPS(uint32_t);
			void throttleFPS(double);
			
			void processEvents();
			void update();
	};
}