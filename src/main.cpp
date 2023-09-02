#include <iostream>
#include <NonEuclideanEngine/application.hpp>
#include <NonEuclideanEngine/misc.hpp>

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cmath>
#include <cstdio>

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
	
	uint32_t objects = argc < 2 ? 8 : atoi(argv[1]);
	double radius = argc < 3 ? 4.0 : atof(argv[2]);
	double angularSpacing = 2*3.1415 / objects;
	double pulseMin = argc < 4 ? 1.0 : atof(argv[3]);
	double pulseMax = argc < 5 ? pulseMin : atof(argv[4]);
	double pulseRate = argc < 6 ? 1.0 : atof(argv[5]);
	
	Knee::Game* game = app.getGameInstance();
	
	for(uint32_t i = 0; i < objects; i++){
		std::string id = std::to_string(i);
		
		game->addRenderableGameObject( id, new Knee::RenderableGameObject(&testVertexData) );
		
		game->getGameObject( id )->setPosition(glm::vec3(0, 0, 0));
		game->getGameObject( id )->setRotation(glm::vec3(0, 0, 0));
		game->getGameObject( id )->setMass(5.0);
	}
	
	game->getPlayer()->setPosition(glm::vec3(-12, 0, 0));
	
	app.setMaxFPS(60);
	
	glm::vec3 baseScale = glm::vec3(1);
	
	// main loop
	while(!app.shouldQuit()){
		double time = app.getDeltaTimer()->getTime();
		double delta = app.getDeltaTimer()->getDelta();
		
		for(uint32_t i = 0; i < objects; i++){
			std::string id = std::to_string(i);
			
			Knee::GameObject* obj = game->getGameObject(id);
			
			// set position
			obj->setPosition( glm::vec3(0, cos(time + i*angularSpacing)*radius, sin(time + i*angularSpacing)*radius) );
			obj->setRotation( glm::vec3( sin(time*2 + i*angularSpacing), cos(time*1.5 + i*angularSpacing), sin(time + i*angularSpacing) ) );
			obj->setScale( glm::vec3( sin(time * pulseRate)*((pulseMax - pulseMin)/2.0) + (pulseMax+pulseMin)/2.0, sin(time * pulseRate)*((pulseMax - pulseMin)/2.0) + (pulseMax + pulseMin)/2.0, sin(time * pulseRate)*((pulseMax - pulseMin)/2.0) + (pulseMax + pulseMin)/2.0) * baseScale );
		}
		
		app.update();
		
		std::cout << "\r" << (uint32_t)(1.0 / delta) << "  ";
	}
	
	std::cout << std::endl;
	
	app.quit();
	
	return 0;
}