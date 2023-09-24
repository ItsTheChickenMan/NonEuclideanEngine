#define SDL_MAIN_HANDLED

#include <iostream>
#include <NonEuclideanEngine/application.hpp>
#include <NonEuclideanEngine/texture.hpp>
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

void loadMap(Knee::Game* game, Knee::VertexData* cubeVertexData, Knee::Texture2D* floorTexture, Knee::Texture2D* wallTexture){
	// add objects
	game->addRenderableStaticGameObject( "floor1", new Knee::RenderableStaticGameObject(cubeVertexData, floorTexture) );
	//game->addRenderableStaticGameObject( "wall11", new Knee::RenderableStaticGameObject(cubeVertexData, wallTexture) );
	//game->addRenderableStaticGameObject( "wall12", new Knee::RenderableStaticGameObject(cubeVertexData, wallTexture) );
	//game->addRenderableStaticGameObject( "wall13", new Knee::RenderableStaticGameObject(cubeVertexData, wallTexture) );
	//game->addRenderableStaticGameObject( "wall14", new Knee::RenderableStaticGameObject(cubeVertexData, wallTexture) );

	// set object properties
	game->getStaticGameObject( "floor1" )->setScale(glm::vec3(10, 2, 10));

	//game->getStaticGameObject( "wall11" )->setPosition(glm::vec3(-6, 5, 0));
	//game->getStaticGameObject( "wall11" )->setScale(glm::vec3(2, 8, 10));

	//game->getStaticGameObject( "wall12" )->setPosition(glm::vec3(6, 5, 0));
	//game->getStaticGameObject( "wall12" )->setScale(glm::vec3(2, 8, 10));

	//game->getStaticGameObject( "wall13" )->setPosition(glm::vec3(0, 5, -6));
	//game->getStaticGameObject( "wall13" )->setScale(glm::vec3(10, 8, 2));

	//game->getStaticGameObject( "wall14" )->setPosition(glm::vec3(0, 5, 6));
	//game->getStaticGameObject( "wall14" )->setScale(glm::vec3(10, 8, 2));

	// add objects
	game->addRenderableStaticGameObject( "floor2", new Knee::RenderableStaticGameObject(cubeVertexData, wallTexture) );
	
	// set object properties
	game->getStaticGameObject( "floor2" )->setPosition(glm::vec3(-20, 0, 0));
	game->getStaticGameObject( "floor2" )->setScale(glm::vec3(10, 2, 10));

	/*game->addRenderableStaticGameObject( "wall21", new Knee::RenderableStaticGameObject(cubeVertexData, floorTexture) );
	game->addRenderableStaticGameObject( "wall22", new Knee::RenderableStaticGameObject(cubeVertexData, floorTexture) );
	//game->addRenderableStaticGameObject( "wall23", new Knee::RenderableStaticGameObject(cubeVertexData, floorTexture) );
	game->addRenderableStaticGameObject( "wall24", new Knee::RenderableStaticGameObject(cubeVertexData, floorTexture) );

	// set object properties
	game->getStaticGameObject( "floor2" )->setPosition(glm::vec3(-20, 0, 0));
	game->getStaticGameObject( "floor2" )->setScale(glm::vec3(10, 2, 10));

	game->getStaticGameObject( "wall21" )->setPosition(glm::vec3(-6 - 20, 5, 0));
	game->getStaticGameObject( "wall21" )->setScale(glm::vec3(2, 8, 10));

	game->getStaticGameObject( "wall22" )->setPosition(glm::vec3(6 - 20, 5, 0));
	game->getStaticGameObject( "wall22" )->setScale(glm::vec3(2, 8, 10));

	//game->getStaticGameObject( "wall23" )->setPosition(glm::vec3(0 - 20, 5, -6));
	//game->getStaticGameObject( "wall23" )->setScale(glm::vec3(10, 8, 2));

	game->getStaticGameObject( "wall24" )->setPosition(glm::vec3(0 - 20, 5, 6));
	game->getStaticGameObject( "wall24" )->setScale(glm::vec3(10, 8, 2));*/
}

int main(int argc, char* argv[]){
	uint32_t windowWidth = 1280;
	uint32_t windowHeight = 720;
	
	Knee::GameApplication app("NonEuclideanEngine Test", windowWidth, windowHeight);
	
	app.initialize();
	
	// CREATE VERTEX DATA
	float testRawVertexData[] = {
		// positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};
	
	uint32_t testSize = 36;
	uint32_t testSizeBytes = testSize * 8 * sizeof(float);
	
	Knee::VertexData testVertexData(testRawVertexData, testSize, testSizeBytes, "pnt");
	
	float portalRawVertexData[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f
	};

	uint32_t portalSize = 6;
	uint32_t portalSizeBytes = portalSize * 8 * sizeof(float);

	Knee::VertexData portalVertexData(portalRawVertexData, portalSize, portalSizeBytes, "pnt");

	// create texture
	Knee::Texture2D testTexture("./NonEuclideanEngine/image/shrock.png");
	Knee::Texture2D testTexture2("./NonEuclideanEngine/image/RGBA_comp.png");

	// load textures
	Knee::Texture2D floorTexture("./NonEuclideanEngine/image/greyfloor.png");
	Knee::Texture2D wallTexture("./NonEuclideanEngine/image/wall.png");

	// get game instance
	Knee::Game* game = app.getGameInstance();
	
	// load map objects
	loadMap(game, &testVertexData, &floorTexture, &wallTexture);

	game->addRenderableGameObject( "myObject", new Knee::RenderableGameObject(&testVertexData, &testTexture) );
	game->addRenderableGameObject( "myObject1", new Knee::RenderableGameObject(&testVertexData, &testTexture) );
	game->addRenderableGameObject( "myObject2", new Knee::RenderableGameObject(&testVertexData, &testTexture2));
	game->addRenderableGameObject( "myObject3", new Knee::RenderableGameObject(&testVertexData, &testTexture2));

	game->getGameObject( "myObject" )->setPosition( glm::vec3(0, 1.5, 0) );
	game->getGameObject( "myObject1" )->setPosition( glm::vec3(-3, 1.5, 0) );
	game->getGameObject( "myObject2" )->setPosition( glm::vec3(-20, 1.5, 0) );
	game->getGameObject( "myObject3" )->setPosition( glm::vec3(-3 - 20, 1.5, 0) );

	//game->getPlayer()->setPosition( glm::vec3(0, 1.5, -4.5) );
	game->getPlayer()->setPosition( glm::vec3(0, 1.5, 0) );

	// create visual portals
	Knee::VisualPortal portal1(&portalVertexData, windowWidth, windowHeight);
	Knee::VisualPortal portal2(&portalVertexData, windowWidth, windowHeight);

	// assign properties
	portal1.setPosition(glm::vec3(0, 2, -4));
	portal1.setRotation(glm::vec3(0, glm::radians(0.f), 0));
	portal1.setScale(glm::vec3(8, 2, 2));

	//portal2.setPosition(glm::vec3(-20, 3, -3));
	//portal2.setRotation(glm::vec3(glm::radians(45.f), 0, 0));
	//portal2.setScale(glm::vec3(0.5, 0.5, 0.5));

	portal2.setPosition(glm::vec3(1.5, 2, 4));
	portal2.setRotation(glm::vec3(glm::radians(-20.f), glm::radians(35.f), 0));
	portal2.setScale(glm::vec3(8, 2, 2) * 0.75f);

	// pair visual portals
	portal1.pairVisualPortal(&portal2);
	portal2.pairVisualPortal(&portal1);

	// add portals as game objects
	game->addVisualPortal( "portal1", &portal1 );
	game->addVisualPortal( "portal2", &portal2 );
	
	// misc settings
	app.setMaxFPS(120);
	
	double t = 0;
	
	// main loop
	while(!app.shouldQuit()){
		double time = app.getDeltaTimer()->getTime();
		t += app.getDeltaTimer()->getDelta();
		
		glm::vec3 intendedForward = glm::normalize(game->getPlayer()->getPosition() - game->getGameObject("myObject1")->getPosition());

		// ( ͡° ͜ʖ ͡°)
		//game->getGameObject( "myObject1" )->setRotation( glm::vec3(0, atan2(intendedForward.x, intendedForward.z), 0) );

		//game->getGameObject( "myObject1" )->setPosition( glm::vec3(-3, 0, sin(time*time)*2.0) );
		//game->getGameObject( "myObject" )->setRotation( glm::vec3(t, 0, 0) );
		
		app.update();
		
		// print fps
		std::cout << "\r" << (uint32_t)app.getFPS() << "  ";
	}
	
	std::cout << std::endl;
	
	app.quit();
	
	return 0;
}