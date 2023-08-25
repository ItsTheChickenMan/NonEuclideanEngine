#include <iostream>
#include <NonEuclideanEngine/application.hpp>

#include <SDL2/SDL.h>

#include <glad/glad.h>
#include <cmath>

int main(int argc, char* argv[]){
	Knee::NonEuclideanApplication app;
	
	app.initialize("NonEuclideanEngine Test", 1280, 720);
	
	float testRawVertexData[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};
	
	uint32_t testSize = 9;
	uint32_t testSizeBytes = testSize*sizeof(float);
	
	Knee::VertexData testVertexData(testRawVertexData, testSize, testSizeBytes, "p");
	
	Knee::ShaderProgram testShaderProgram;
	
	testShaderProgram.attachShader(GL_VERTEX_SHADER, "./NonEuclideanEngine/shaders/simplevertex.glsl");
	testShaderProgram.attachShader(GL_FRAGMENT_SHADER, "./NonEuclideanEngine/shaders/simplefragment.glsl");
	
	testShaderProgram.compile();
	
	// main loop
	while(!app.shouldQuit()){
		testShaderProgram.drawVertexData(testVertexData);
	
		app.processEvents();
		
		app.updateWindow();
	}
	
	app.quit();
	
	return 0;
}