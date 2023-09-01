#include <iostream>
#include <NonEuclideanEngine/application.hpp>
#include <NonEuclideanEngine/misc.hpp>

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glad/glad.h>
#include <cmath>
#include <cstdio>

int main(int argc, char* argv[]){
	Knee::NonEuclideanApplication app;
	
	uint32_t windowWidth = 1280;
	uint32_t windowHeight = 720;
	
	app.initialize("NonEuclideanEngine Test", windowWidth, windowHeight);
	
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
	
	// CREATE RENDERABLE OBJECT
	Knee::RenderableObject renderableObject(&testVertexData);
	
	// CREATE SHADER PROGRAM
	Knee::RenderableObjectShaderProgram testShaderProgram( glm::radians(45.f), (float)windowWidth / (float)windowHeight, 0.1f, 100.f);
	
	testShaderProgram.attachShader(GL_VERTEX_SHADER, "./NonEuclideanEngine/shaders/simpleprojection/simpleprojectionvertex.glsl");
	testShaderProgram.attachShader(GL_FRAGMENT_SHADER, "./NonEuclideanEngine/shaders/simple/simplefragment.glsl");
	
	testShaderProgram.compile();
	
	testShaderProgram.m_camera.setPosition(glm::vec3(0, 0, 0));
	
	renderableObject.setPosition(glm::vec3(5, 0, 0));
	renderableObject.setRotation(glm::vec3(glm::radians(0.f), glm::radians(0.f), glm::radians(0.f)));
	
	glm::vec3 baseScale = glm::vec3(1, 1, 3);
	
	float pulseLow = argc < 4 ? 1.0 : atof(argv[3]);
	float pulseHigh = argc < 5 ? pulseLow : atof(argv[4]);
	float pulseRate = argc < 6 ? 1.0 : atof(argv[5]);
	
	uint32_t width = argc < 2 ? 1 : atoi(argv[1]);
	uint32_t height = argc < 3 ? 1 : atoi(argv[2]);
	
	double render_width = 7.25;
	double render_height = 4.0;
	
	glm::vec3 scaledScale = baseScale / (float)(width > height ? width : height);

	glEnable(GL_DEPTH_TEST);
	
	Knee::DeltaTimer timer;
	
	// main loop
	while(!app.shouldQuit()){
		double delta = timer.getDeltaAndReset();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		renderableObject.changeRotation(glm::vec3(delta, delta*10, 0));
		
		renderableObject.setScale( (glm::vec3( sin(timer.getTime() * pulseRate), sin(timer.getTime() * pulseRate), sin(timer.getTime() * pulseRate) )*((pulseHigh-pulseLow)/2.f) + (pulseHigh+pulseLow)/2.f )  * scaledScale);
		
		for(uint32_t j = 0; j < height; j++){
			double ystep = render_height / height;
			
			for(uint32_t i = 0; i < width; i++){
				double xstep = render_width / width;
				
				renderableObject.setPosition(glm::vec3(5, (j+0.5)*ystep - render_height/2.0, (i+0.5)*xstep - render_width/2.0));
				testShaderProgram.drawRenderableObject(renderableObject);
			}
		}
	
		app.processEvents();
		
		app.updateWindow();
		
		//std::cout << "\r" << (1.0 / delta) << "fps" << "   ";
	}
	
	std::cout << std::endl;
	
	app.quit();
	
	return 0;
}