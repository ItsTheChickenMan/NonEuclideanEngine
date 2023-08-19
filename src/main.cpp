#include <iostream>
#include <NonEuclideanEngine/application.hpp>

#include <SDL2/SDL.h>

#include <glad/glad.h>
#include <cmath>

int main(int argc, char* argv[]){
	Knee::NonEuclideanApplication app;
	
	app.initialize("NonEuclideanEngine Test", 800, 600);
	
	// main loop
	while(!app.shouldQuit()){
		app.processEvents();
		
		app.updateWindow();
	}
	
	app.quit();
	
	return 0;
}