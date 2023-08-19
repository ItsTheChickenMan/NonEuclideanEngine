#include <NonEuclideanEngine/shader.hpp>
#include <NonEuclideanEngine/fileio.hpp>
#include <NonEuclideanEngine/misc.hpp>

#include <glad/glad.h>
#include <iostream>

// default max texture units (none)
int32_t Knee::ShaderProgram::m_maxTextureUnits = 0;

// constructor
Knee::ShaderProgram::ShaderProgram(){
	
}

// destructor
Knee::ShaderProgram::~ShaderProgram(){
	this->destroy();
}

bool Knee::ShaderProgram::isCompiled(){
	return this->m_compiled;
}

// make sure this is only called once graphics have been properly initialized
void Knee::ShaderProgram::loadMaxTextureUnits(){
	// get max supported texture units
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &Knee::ShaderProgram::m_maxTextureUnits);
}

// create a shader of the provided type from the provided source path and store it until compilation
// returns 0 upon success and -1 upon error
int32_t Knee::ShaderProgram::attachShader(GLenum shaderType, std::string file){
	// read shader source
	char* contents;
	
	if(Knee::readFileToCharBuffer(file.c_str(), &contents) != 0){
		std::cout << Knee::ERROR_PREFACE << "error reading shader contents: " << SDL_GetError() << std::endl;
		
		return -1;
	}
	
	// create shader object
	GLuint shader = glCreateShader(shaderType);
	
	// load shader source
	glShaderSource(shader, 1, &contents, NULL);
	
	// compile
	glCompileShader(shader);
	
	// free contents
	delete[] contents;
	
	// get compile status
	GLint shaderCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
	
	// check compile status
	if(!shaderCompiled){
		// get info log
		GLchar* infoLog = this->getShaderInfoLog(shader);
		
		if(!infoLog){
			std::cout << Knee::ERROR_PREFACE << "There was a shader compilation error, but the info log is empty" << std::endl;
		} else {
			std::cout << Knee::ERROR_PREFACE << infoLog << std::endl;
			
			delete[] infoLog;
		}
		
		return -1;
	}
	
	// add shader to current attached shaders
	this->m_shaders.push_back(shader);
	
	return 0;
}

// gets whatever info is currently in the shader info log and returns it as a string on the heap
// remember to delete when done using
GLchar* Knee::ShaderProgram::getShaderInfoLog(GLuint shader){
	// get info log length
	GLint infoLogLength = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
	
	if(infoLogLength == 0) return NULL;
	
	// get info log
	GLchar* infoLog = new GLchar[infoLogLength];

	glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
	
	return infoLog;
}

GLchar* Knee::ShaderProgram::getProgramInfoLog(){
	// get info log length
	GLint infoLogLength = 0;
	glGetProgramiv(this->m_program, GL_INFO_LOG_LENGTH, &infoLogLength);
	
	if(infoLogLength == 0) return NULL;
	
	// get info log
	GLchar* infoLog = new GLchar[infoLogLength];

	glGetProgramInfoLog(this->m_program, infoLogLength, NULL, infoLog);
	
	return infoLog;
}

int32_t Knee::ShaderProgram::compile(){
	// create program
	this->m_program = glCreateProgram();
	
	// attach shaders
	for(uint32_t i = 0; i < this->m_shaders.size(); i++){
		glAttachShader(this->m_program, this->m_shaders[i]);
	}
	
	// link program
	glLinkProgram(this->m_program);
	
	// check for link errors
	GLint linkStatus = 0;
	glGetProgramiv(this->m_program, GL_LINK_STATUS, &linkStatus);
	
	if(!linkStatus){
		// get info log
		GLchar* infoLog = this->getProgramInfoLog();

		if(!infoLog){
			std::cout << Knee::ERROR_PREFACE << "There was a shader program linker error, but the info log is empty." << std::endl;
		} else {
			std::cout << Knee::ERROR_PREFACE << infoLog;
			
			delete[] infoLog;
		}
		
		return -1;
	}
	
	this->m_compiled = true;
	
	// load uniforms
	this->loadUniformLocations();
	
	return 0;
}

// schedules all attached shaders for deletion when program is deleted
// note that shaders aren't actually deleted until the program is deleted
void Knee::ShaderProgram::markAttachedShadersForDeletion(){
	for(uint32_t i = 0; i < this->m_shaders.size(); i++){
		glDeleteShader(this->m_shaders[i]);
	}
}

// delete shaders and program
void Knee::ShaderProgram::destroy(){
	this->markAttachedShadersForDeletion();
	
	glDeleteProgram(this->m_program);
}

int32_t Knee::ShaderProgram::loadUniformLocations(){
	if(!this->isCompiled()) return -1;
	
	// get number of active uniforms
	GLint numUniforms = 0;
	
	glGetProgramiv(this->m_program, GL_ACTIVE_UNIFORMS, &numUniforms);
	
	// max amount of characters that a uniform name can have
	int32_t nameBufferSize = 256;
	
	// loop through each uniform and get its name and location
	for(GLint i = 0; i < numUniforms; i++){
		// name buffer
		char nameBuffer[nameBufferSize] = {'\0'};
		
		glGetActiveUniformName(this->m_program, (GLuint)i, nameBufferSize, NULL, nameBuffer);
		
		// convert name to std::string
		std::string name = std::string(nameBuffer);
		
		// get location
		GLint location = glGetUniformLocation(this->m_program, name.c_str());
		
		if(location == -1) continue;
		
		// save to uniform manager
		this->m_uniforms[name] = location;
	}
	
	return 0;
}

void Knee::ShaderProgram::use(){
	glUseProgram(this->m_program);
}

GLint Knee::ShaderProgram::getUniformLocation(std::string name){
	return this->m_uniforms[name];
}

