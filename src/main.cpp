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
	
	game->addRenderableGameObject( "myObject", new Knee::RenderableGameObject(&testVertexData, &testTexture) );
	//game->addRenderableGameObject( "myObject1", new Knee::RenderableGameObject(&testVertexData, &testTexture) );
	game->addRenderableGameObject( "myObject2", new Knee::RenderableGameObject(&testVertexData, &testTexture2));
	game->addRenderableGameObject( "myObject3", new Knee::RenderableGameObject(&testVertexData, &testTexture2));
	game->addRenderableGameObject( "losernado", new Knee::RenderableGameObject(&testVertexData, &floorTexture) );
	
	// load map objects
	loadMap(game, &testVertexData, &floorTexture, &wallTexture);
	
	game->getGameObject( "myObject" )->setPosition( glm::vec3(0, 1.5, 0) );
	//game->getGameObject( "myObject1" )->setPosition( glm::vec3(-3, 1.5, 0) );
	game->getGameObject( "myObject2" )->setPosition( glm::vec3(-17, 2.0, 0) );
	game->getGameObject( "myObject3" )->setPosition( glm::vec3(-17, 1.5, 0) );

	game->getGameObject( "losernado" )->setPosition(glm::vec3(0, 5, 0));
	game->getGameObject( "losernado" )->setScale(glm::vec3(0.1, 0.1, 1));

	//game->getPlayer()->setPosition( glm::vec3(0, 1.5, -4.5) );
	game->getPlayer()->setPosition( glm::vec3(-20, 1.5, 0) );

	// create portals
	Knee::Portal portal1(&portalVertexData, windowWidth, windowHeight);
	Knee::Portal portal2(&portalVertexData, windowWidth, windowHeight);
	Knee::Portal portal3(&portalVertexData, windowWidth, windowHeight);
	Knee::Portal portal4(&portalVertexData, windowWidth, windowHeight);

	// assign properties
	portal1.setPosition(glm::vec3(-20, 2.5, -3));
	portal1.setRotation(glm::vec3(0, glm::radians(0.f), 0));
	portal1.setScale(glm::vec3(3, 3, 1));

	portal2.setPosition(glm::vec3(-20, 2.5, 3));
	portal2.setRotation(glm::vec3(glm::radians(-0.f), glm::radians(0.f), 0));
	portal2.setScale(glm::vec3(3, 3, 1));

	portal3.setPosition(glm::vec3(3, 2.5, 0));
	portal3.setRotation(glm::vec3(glm::radians(-0.f), glm::radians(90.f), 0));
	portal3.setScale(glm::vec3(3, 3, 1));

	portal4.setPosition(glm::vec3(-3, 2.5, 0));
	portal4.setRotation(glm::vec3(glm::radians(-0.f), glm::radians(90.f), 0));
	portal4.setScale(glm::vec3(3, 3, 1));


	// pair visual portals
	portal1.pair(&portal2);
	//portal3.pair(&portal1);
	portal2.pair(&portal1);
	//portal4.pair(&portal2);

	// add portals as game objects
	game->addPortal( "portal1", &portal1 );
	game->addPortal( "portal2", &portal2 );
	//game->addPortal( "portal3", &portal3 );
	//game->addPortal( "portal4", &portal4 );

	// misc settings
	app.setMaxFPS(120);
	
	// main loop
	while(!app.shouldQuit()){
		double time = app.getDeltaTimer()->getTime();
		double delta = app.getDeltaTimer()->getDelta();
		
		//glm::vec3 intendedForward = glm::normalize(game->getPlayer()->getPosition() - game->getGameObject("myObject1")->getPosition());

		// ( ͡° ͜ʖ ͡°)
		//game->getGameObject( "myObject1" )->setRotation( glm::vec3(0, atan2(intendedForward.x, intendedForward.z), 0) );

		game->getGameObject( "losernado" )->setRotation( game->getPlayer()->getRotation() );
		//game->getGameObject( "myObject1" )->setPosition( glm::vec3(-3 + sin( time*5.0 )*0.5, 1.5, 0) );
		game->getGameObject( "myObject2" )->setPosition( glm::vec3( -17, 2.75 + sin(time*7.0)*0.25, 0) );
		//game->getGameObject( "myObject" )->setRotation( glm::vec3(t, 0, 0) );
		
		float mag = glm::radians(45.f);

		if(game->getPlayer()->getInputHandler()->getKeyState(SDL_SCANCODE_LEFT)){
			game->getStaticGameObject( "portal2" )->changeRotation(glm::vec3(0, mag * delta, 0));
		} else if(game->getPlayer()->getInputHandler()->getKeyState(SDL_SCANCODE_RIGHT)){
			game->getStaticGameObject( "portal2" )->changeRotation(glm::vec3(0, -mag * delta, 0));
		}

		float scaleMag = 0.999f;

		/*if(game->getPlayer()->getInputHandler()->getKeyState(SDL_SCANCODE_UP)){
			game->getStaticGameObject( "portal2" )->changeScale(glm::vec3(1) * (1.0f / scaleMag));
		} else if(game->getPlayer()->getInputHandler()->getKeyState(SDL_SCANCODE_DOWN)){
			game->getStaticGameObject( "portal2" )->changeScale(glm::vec3(1) * scaleMag);
		}*/

		//game->getStaticGameObject( "portal2" )->setPosition(glm::vec3(0, 1 + game->getStaticGameObject("portal2")->getScale().y/2.f, 3));

		app.update();
		
		// print fps
		//std::cout << "\r" << (uint32_t)app.getFPS() << "  ";
	}
	
	std::cout << std::endl;
	
	app.quit();
	
	return 0;
}