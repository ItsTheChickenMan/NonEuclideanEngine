#define SDL_MAIN_HANDLED

#include <iostream>
#include <NonEuclideanEngine/application.hpp>
#include <NonEuclideanEngine/misc.hpp>

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cmath>
#include <cstdio>

glm::vec3 infinitySymbol(double t){
	return glm::vec3(0, sin(2*t)*0.5, sin(t));
}

int main(int argc, char* argv[]){
	uint32_t windowWidth = 1280;
	uint32_t windowHeight = 720;
	
	Knee::GameApplication app("NonEuclideanEngine Test", windowWidth, windowHeight);
	
	app.initialize();
	
	// CREATE VERTEX DATA
	float testRawVertexData[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f
	};
	
	uint32_t testSize = 36;
	uint32_t testSizeBytes = testSize * 3 * sizeof(float);
	
	Knee::VertexData testVertexData(testRawVertexData, testSize, testSizeBytes, "p");
	
	// get game instance
	Knee::Game* game = app.getGameInstance();
	
	game->addRenderableGameObject( "myObject", new Knee::RenderableGameObject(&testVertexData) );
	game->addRenderableGameObject( "myObject1", new Knee::RenderableGameObject(&testVertexData) );
	
	game->getGameObject( "myObject1" )->setPosition( glm::vec3(-3, 0, 0) );
	
	game->getPlayer()->setPosition( glm::vec3(0, 0, -6) );
	
	// misc settings
	app.setMaxFPS(60);
	
	double t = 0;
	
	// main loop
	while(!app.shouldQuit()){
		double time = app.getDeltaTimer()->getTime();
		t += app.getDeltaTimer()->getDelta();
		
		//std::cout << glm::to_string(game->getCamera()->getPosition()) << std::endl;
		
		//game->getGameObject( "myObject" )->setPosition( glm::vec3(0, 0, sin(time)*2.0) );
		game->getGameObject( "myObject" )->setRotation( glm::vec3(t, 0, 0) );
		
		app.update();
		
		// print fps
		//std::cout << "\r" << (uint32_t)app.getFPS() << "  ";
	}
	
	std::cout << std::endl;
	
	app.quit();
	
	return 0;
}