#include <NonEuclideanEngine/shader.hpp>
#include <NonEuclideanEngine/fileio.hpp>
#include <NonEuclideanEngine/misc.hpp>

#include <glad/glad.h>
#include <iostream>
#include <string>

// default max texture units (none)
int32_t Knee::ShaderProgram::m_maxTextureUnits = 0;

// vertex attributes
const std::string Knee::VertexData::VA_POSITION_STR = "p";
const std::string Knee::VertexData::VA_TEXCOORD_STR = "t";
const std::string Knee::VertexData::VA_NORMAL_STR = "n";

const uint32_t Knee::VertexData::VA_POSITION_SIZE = 3;
const uint32_t Knee::VertexData::VA_TEXCOORD_SIZE = 2;
const uint32_t Knee::VertexData::VA_NORMAL_SIZE = 3;

const uint32_t Knee::VertexData::VA_POSITION_INDEX = 0;
const uint32_t Knee::VertexData::VA_TEXCOORD_INDEX = 1;
const uint32_t Knee::VertexData::VA_NORMAL_INDEX = 2;

// dataOrder is used to specify the presence and order of vertex positions, texture coordinates, and normals in vertex data.
// vertex positions are assumed to have 3 components, texture coordinates assumed to have 2, and normals assumed to have 3.
// ex.	"p" = position only, 3-component vertex data
//			"pt" = position then texture coordinates, 5-component vertex data
//			"tp" = texture coordinates then position
//			"pnt" = position, normals, texture coordinates
// the indices of attributes in shaders will always map 0 to position, 1 to texture coordinates, and 2 to normals
Knee::VertexData::VertexData(float* data, uint32_t vertexCount, GLsizeiptr dataSize, std::string attributeOrder) : m_vertexCount(vertexCount) {
	// create vertex buffer object
	glGenBuffers(1, &this->m_vbo);
	
	// bind buffer to copy data
	glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo);
	
	// copy data
	glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
	
	// create vertex array object
	glGenVertexArrays(1, &this->m_vao);
	
	// bind vertex array for modification
	glBindVertexArray(this->m_vao);
	
	// create vertex attribute pointers
	size_t positionIndex = attributeOrder.find(Knee::VertexData::VA_POSITION_STR);
	size_t texCoordIndex = attributeOrder.find(Knee::VertexData::VA_TEXCOORD_STR);
	size_t normalIndex = attributeOrder.find(Knee::VertexData::VA_NORMAL_STR);
	
	bool hasPositions = positionIndex != std::string::npos;
	bool hasTexCoords = texCoordIndex != std::string::npos;
	bool hasNormals = normalIndex != std::string::npos;
	
	uint32_t positionSize = hasPositions ? Knee::VertexData::VA_POSITION_SIZE : 0;
	uint32_t texCoordSize = hasTexCoords ? Knee::VertexData::VA_TEXCOORD_SIZE : 0;
	uint32_t normalSize = hasNormals ? Knee::VertexData::VA_NORMAL_SIZE : 0;
	
	// vertex position pointer
	if(hasPositions){
		uint32_t stride = texCoordSize + normalSize;
		uint32_t offset = 0;
		
		if(positionIndex == 0){
			offset = 0;
		} else if(positionIndex == 2){
			offset = stride;
		} else {
			if(hasTexCoords && texCoordIndex == 0){
				offset = texCoordSize;
			} else {
				offset = normalSize;
			}
		}
		
		glEnableVertexAttribArray(Knee::VertexData::VA_POSITION_INDEX);
		
		glVertexAttribPointer(Knee::VertexData::VA_POSITION_INDEX, Knee::VertexData::VA_POSITION_SIZE, GL_FLOAT, GL_FALSE, stride * sizeof(float), (GLvoid*)(offset * sizeof(float)));
	}
	
	// vertex tex coords pointer
	if(hasTexCoords){
		uint32_t stride = positionSize + normalSize;
		uint32_t offset = 0;
		
		if(texCoordIndex == 0){
			offset = 0;
		} else if(texCoordIndex == 2){
			offset = stride;
		} else {
			if(hasPositions && positionIndex == 0){
				offset = positionSize;
			} else {
				offset = normalSize;
			}
		}
		
		glEnableVertexAttribArray(Knee::VertexData::VA_TEXCOORD_INDEX);
		
		glVertexAttribPointer(Knee::VertexData::VA_TEXCOORD_INDEX, Knee::VertexData::VA_TEXCOORD_SIZE, GL_FLOAT, GL_FALSE, stride * sizeof(float), (GLvoid*)(offset * sizeof(float)));
	}

	// vertex normals pointer
	if(hasNormals){
		uint32_t stride = positionSize + texCoordSize;
		uint32_t offset = 0;
		
		if(normalIndex == 0){
			offset = 0;
		} else if(normalIndex == 2){
			offset = stride;
		} else {
			if(hasPositions && positionIndex == 0){
				offset = positionSize;
			} else {
				offset = texCoordSize;
			}
		}
		
		glEnableVertexAttribArray(Knee::VertexData::VA_NORMAL_INDEX);
		
		glVertexAttribPointer(Knee::VertexData::VA_NORMAL_INDEX, Knee::VertexData::VA_NORMAL_SIZE, GL_FLOAT, GL_FALSE, stride * sizeof(float), (GLvoid*)(offset * sizeof(float)));
	}
	
	// unbind everything
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Knee::VertexData::~VertexData(){
	// delete vbo
	glDeleteBuffers(1, &this->m_vbo);
	
	// delete vao
	glDeleteVertexArrays(1, &this->m_vao);
}

uint32_t Knee::VertexData::getVertexCount() const {
	return this->m_vertexCount;
}

// use this vertex data for vertex attributes for all shader calls following (until another is used instead)
void Knee::VertexData::use() const {
	glBindVertexArray(this->m_vao);
}

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
	char* contents = NULL;
	
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
			std::cout << Knee::ERROR_PREFACE << (shaderType == GL_VERTEX_SHADER ? " GL_VERTEX_SHADER " : " GL_FRAGMENT_SHADER ") << infoLog << std::endl;
			
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

void Knee::ShaderProgram::drawArrays(uint32_t count){
	glDrawArrays(GL_TRIANGLES, 0, count);
}

void Knee::ShaderProgram::drawVertexData(const Knee::VertexData& vertexData){
	// enable this shader
	this->use();

	// enable vertex data
	vertexData.use();
	
	// draw arrays
	this->drawArrays(vertexData.getVertexCount());
}