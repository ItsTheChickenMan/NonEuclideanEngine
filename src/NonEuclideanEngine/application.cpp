// sdl2 application management

// includes //
#include <NonEuclideanEngine/application.hpp>
#include <NonEuclideanEngine/misc.hpp>

#include <iostream>
#include <string>
#include <cassert>

// application class methods definitions //

// constructor
Knee::Application::Application(std::string title, uint32_t windowWidth, uint32_t windowHeight) : m_windowTitle(title), m_windowWidth(windowWidth), m_windowHeight(windowHeight) {}

// destructor
Knee::Application::~Application(){
	this->quit();
}

// initialize
// sets up subsystems, creates the window
void Knee::Application::initialize(){
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
	
	this->m_window = SDL_CreateWindow(this->m_windowTitle.c_str(), 20, 50, this->m_windowWidth, this->m_windowHeight, flags);
	
	assert(this->m_window != NULL);
	
	// create opengl context
	this->m_glContext = SDL_GL_CreateContext(this->m_window);
	
	assert(this->m_glContext != NULL);
	
	// glad setup
	if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)){
		std::cout << Knee::ERROR_PREFACE << "Failed to initialize GLAD" << std::endl;
	}
	
	// set viewport size
	glViewport(0, 0, this->m_windowWidth, this->m_windowHeight);
	
	// enable vsync by default
	this->setSwapInterval(0);
	
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

Knee::GameApplication::GameApplication(std::string title, uint32_t windowWidth, uint32_t windowHeight) : Knee::Application(title, windowWidth, windowHeight), m_game(windowWidth, windowHeight) {}

Knee::GameApplication::~GameApplication(){
	
}

Knee::Game* Knee::GameApplication::getGameInstance(){
	return &this->m_game;
}

Knee::DeltaTimer* Knee::GameApplication::getDeltaTimer(){
	return &this->m_deltaTimer;
}

void Knee::GameApplication::setMaxFPS(uint32_t fps){
	this->m_expectedDelta = 1.0 / (double)(fps+1);
}

void Knee::GameApplication::initialize(){
	// call original initialize
	Application::initialize();
	
	// initialize game as well
	this->m_game.initialize();
}

void Knee::GameApplication::processEvents(){
	// event placeholder
	SDL_Event event;
	
	while(SDL_PollEvent(&event)){
		// first process as application level event
		if(this->processApplicationEvent(event) != 0){
			// don't bother processing if it was already handled
			// TODO: might have to remove this if we want to do additional event handling but should be fine for now
			continue;
		}
		
		// forward to game to process as game event
		this->getGameInstance()->processEvent(event);
	}
}

double Knee::GameApplication::getFPS(){
	return 1.0 / this->getDeltaTimer()->getDelta();
}

void Knee::GameApplication::throttleFPS(double frameDelta){
	double remainingDelta = this->m_expectedDelta - frameDelta;
	
	if(remainingDelta <= 1.0/10000.0) return;
	
	this->m_deltaTimer.pauseThread(remainingDelta);
}

void Knee::GameApplication::update(){
	double delta = this->m_deltaTimer.getDeltaAndReset();
	double startTime = this->m_deltaTimer.getTime();
	
	// reset input handler
	this->getGameInstance()->getPlayer()->getInputHandler()->reset();
	
	// process events
	this->processEvents();
	
	// get game instance
	Knee::Game* game = this->getGameInstance();
	
	// update player
	game->getPlayer()->update(delta);
	
	// update game objects
	game->updateGameObjects(delta);
	
	// render scene
	game->renderScene();
	
	// update buffer
	this->updateWindow();
	
	// throttle fps
	double endTime = this->m_deltaTimer.getTime();
	
	this->throttleFPS(endTime - startTime);
}