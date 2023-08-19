// sdl2 application management

// includes //
#include <NonEuclideanEngine/application.hpp>
#include <NonEuclideanEngine/misc.hpp>

#include <iostream>
#include <string>
#include <cassert>

// application class methods definitions //

// constructor
Knee::Application::Application(){
}

// destructor
Knee::Application::~Application(){
	this->quit();
}

// initialize
// sets up subsystems, creates the window
void Knee::Application::initialize(std::string title, uint32_t windowWidth, uint32_t windowHeight){
	// initialize video subsystem
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		std::cout << Knee::ERROR_PREFACE << SDL_GetError();
	}
	
	// set GL attributes necessary for creating window
	// using OpenGL 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	
	// core profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	// double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
	// require 32-bit color buffer (8 per rgba)
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	
	// require a stencil buffer
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	
	// create window
	uint32_t flags = 0;
	
	flags |= SDL_WINDOW_SHOWN;
	flags |= SDL_WINDOW_OPENGL;
	
	this->m_window = SDL_CreateWindow(title.c_str(), 20, 50, windowWidth, windowHeight, flags);
	
	assert(this->m_window != NULL);
	
	// create opengl context
	this->m_glContext = SDL_GL_CreateContext(this->m_window);
	
	assert(this->m_glContext != NULL);
	
	// glad setup
	if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
		std::cout << Knee::ERROR_PREFACE << "Failed to initialize GLAD" << std::endl;
	}
	
	// set viewport size
	glViewport(0, 0, windowWidth, windowHeight);
	
	if(SDL_GL_SetSwapInterval(1) < 0){
		std::cout << Knee::ERROR_PREFACE << "Couldn't enable vsync" << std::endl;
	}
	
	// misc gl settings
	glClearColor(0.3, 0.0, 0.0, 1.0);
	
	Knee::ShaderProgram::loadMaxTextureUnits();
}

// free any occupied memory, delete anything related to SDL and then quit SDL
void Knee::Application::quit(){
	// free window
	SDL_DestroyWindow(this->m_window);
	
	// call quit
	SDL_Quit();
}

void Knee::Application::processEvents(){
	// event placeholder
	SDL_Event event;
	
	while(SDL_PollEvent(&event)){
		switch(event.type) {
			case SDL_QUIT:
				this->m_shouldQuit = true;
				break;
		}
	}
}

void Knee::Application::updateWindow(){
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	
	// swap buffers
	SDL_GL_SwapWindow(this->m_window);
}

bool Knee::Application::shouldQuit(){
	return this->m_shouldQuit;
}

// non euclidean application class definitions //

Knee::NonEuclideanApplication::NonEuclideanApplication() : Application() {
}

Knee::NonEuclideanApplication::~NonEuclideanApplication(){
}