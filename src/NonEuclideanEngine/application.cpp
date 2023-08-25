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
	
	// enable vsync by default
	this->setSwapInterval(1);
	
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

// check if the event is an application level event and process it if it is.
// returns 1 if the event was processed in some way, otherwise returns 0.
int32_t Knee::Application::processApplicationEvent(SDL_Event event){
	int32_t status = 0;
	
	switch(event.type) {
		case SDL_QUIT:
			this->m_shouldQuit = true;
			status = 1;
			break;
	}
	
	return status;
}

void Knee::Application::processEvents(){
	// event placeholder
	SDL_Event event;
	
	while(SDL_PollEvent(&event)){
		this->processApplicationEvent(event);
	}
}

void Knee::Application::updateWindow(){
	// swap buffers
	SDL_GL_SwapWindow(this->m_window);
}

bool Knee::Application::shouldQuit(){
	return this->m_shouldQuit;
}

int32_t Knee::Application::setSwapInterval(int32_t swapInterval){
	if(SDL_GL_SetSwapInterval(swapInterval) < 0){
		std::cout << Knee::ERROR_PREFACE << "There was an error setting the swap interval to " << swapInterval << std::endl;
		
		return -1;
	}
	
	return 0;
}

// non euclidean application class definitions //

Knee::NonEuclideanApplication::NonEuclideanApplication() : Application() {
}

Knee::NonEuclideanApplication::~NonEuclideanApplication(){
	
}

void Knee::NonEuclideanApplication::processEvents(){
	// event placeholder
	SDL_Event event;
	
	while(SDL_PollEvent(&event)){
		// first process as application level event
		if(this->processApplicationEvent(event) != 0){
			// don't bother processing if it was already handled
			// TODO: might have to remove this if we want to do additional event handling but should be fine for now
			continue;
		}
		
		switch(event.type){
			
		}
	}
}

void Knee::NonEuclideanApplication::update(){
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	
	// update buffer
	this->updateWindow();
}