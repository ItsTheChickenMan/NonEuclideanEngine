#include <NonEuclideanEngine/shader.hpp>
#include <NonEuclideanEngine/fileio.hpp>
#include <NonEuclideanEngine/misc.hpp>

#include <glad/glad.h>
#include <iostream>
#include <string>
#include <math.h>

// default max texture units (none)
int32_t Knee::ShaderProgram::MAX_TEXTURE_UNITS = 0;

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

// -------------------- //
// VertexData //

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
	
	uint32_t stride = positionSize + texCoordSize + normalSize;

	// vertex position pointer
	if(hasPositions){
		uint32_t offset = 0;
		
		if(positionIndex == 0){
			offset = 0;
		} else if(positionIndex == 2){
			offset = texCoordSize + normalSize;;
		} else {
			if(hasTexCoords && texCoordIndex == 0){
				offset = texCoordSize;
			} else {
				offset = normalSize;
			}
		}

		glVertexAttribPointer(Knee::VertexData::VA_POSITION_INDEX, positionSize, GL_FLOAT, GL_FALSE, stride * sizeof(float), (GLvoid*)(offset * sizeof(float)));
	
		glEnableVertexAttribArray(Knee::VertexData::VA_POSITION_INDEX);
	}
	
	// vertex tex coords pointer
	if(hasTexCoords){
		uint32_t offset = 0;
		
		if(texCoordIndex == 0){
			offset = 0;
		} else if(texCoordIndex == 2){
			offset = positionSize + normalSize;
		} else {
			if(hasPositions && positionIndex == 0){
				offset = positionSize;
			} else {
				offset = normalSize;
			}
		}
		
		glVertexAttribPointer(Knee::VertexData::VA_TEXCOORD_INDEX, texCoordSize, GL_FLOAT, GL_FALSE, stride * sizeof(float), (GLvoid*)(offset * sizeof(float)));

		glEnableVertexAttribArray(Knee::VertexData::VA_TEXCOORD_INDEX);
	}

	// vertex normals pointer
	if(hasNormals){
		uint32_t offset = 0;
		
		if(normalIndex == 0){
			offset = 0;
		} else if(normalIndex == 2){
			offset = positionSize + texCoordSize;
		} else {
			if(hasPositions && positionIndex == 0){
				offset = positionSize;
			} else {
				offset = texCoordSize;
			}
		}
		
		glEnableVertexAttribArray(Knee::VertexData::VA_NORMAL_INDEX);
		
		glVertexAttribPointer(Knee::VertexData::VA_NORMAL_INDEX, normalSize, GL_FLOAT, GL_FALSE, stride * sizeof(float), (GLvoid*)(offset * sizeof(float)));
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

// -------------------- //
// ShaderProgram //

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

uint32_t Knee::ShaderProgram::getMaxTextureUnits(){
	return Knee::ShaderProgram::MAX_TEXTURE_UNITS;
}

// make sure this is only called once graphics have been properly initialized
void Knee::ShaderProgram::loadMaxTextureUnits(){
	// get max supported texture units
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &Knee::ShaderProgram::MAX_TEXTURE_UNITS);
}

// bind a 2D texture to a uniform in this shader program
void Knee::ShaderProgram::bindTexture2D(std::string uniformName, Knee::Texture2D* texture){
	// check if we would be exceeding the maximum texture units supported
	if(this->m_boundTextureCount >= this->getMaxTextureUnits()){
		std::cout << Knee::WARNING_PREFACE << "Attempted to bind more textures than maximum supported (" << this->getMaxTextureUnits() << ")" << std::endl;
		return;
	}

	// set active texture
	glActiveTexture(GL_TEXTURE0 + this->m_boundTextureCount);

	// bind the texture name to the working 2D texture
	glBindTexture(GL_TEXTURE_2D, texture->getGLTexture());

	// set uniform
	glUniform1i(this->getUniformLocation(uniformName), this->m_boundTextureCount);

	// increment number of bound textures
	this->m_boundTextureCount++;
}

// reset the number of currently bound textures
void Knee::ShaderProgram::resetBoundTextures(){
	// reset bound texture count
	this->m_boundTextureCount = 0;
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

// sets the value at the uniform location in this shader to the provided mat4.  returns true if the uniform was found, false if otherwise.
bool Knee::ShaderProgram::setUniformMat4(std::string name, glm::mat4 matrix){
	GLint location = this->getUniformLocation(name);
	
	if(location == -1) return false;
	
	this->use();
	
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	
	return true;
}

void Knee::ShaderProgram::use(){
	glUseProgram(this->m_program);
}

GLint Knee::ShaderProgram::getUniformLocation(std::string name){
	GLint out = -1;
	
	try {
		out = this->m_uniforms.at(name);
	} catch( const std::out_of_range& e ){
		out = -1;
	}
	
	return out;
}

void Knee::ShaderProgram::drawArrays(uint32_t count){
	glDrawArrays(GL_TRIANGLES, 0, count);
}

void Knee::ShaderProgram::drawVertexData(const Knee::VertexData* vertexData){
	// enable this shader
	this->use();

	// enable vertex data
	vertexData->use();
	
	// draw arrays
	this->drawArrays(vertexData->getVertexCount());
}

// -------------------- //
// Camera //

Knee::Camera::Camera(){
	this->setPosition(glm::vec3(0));
	this->setRotation(glm::vec3(0));
}

glm::mat4 Knee::Camera::getProjectionMatrix(){
	return this->m_projectionMatrix;
}

glm::mat4 Knee::Camera::getViewMatrix(){
	return this->m_viewMatrix;
}

glm::mat4 Knee::Camera::getViewProjectionMatrix(){
	return this->m_vpMatrix;
}

void Knee::Camera::updateViewMatrix(){
	this->m_viewMatrix = glm::lookAt(this->getPosition(), this->getPosition() + this->getForwardVector(), this->m_up);
}

void Knee::Camera::updateViewProjectionMatrix(){
	this->updateViewMatrix();
	
	this->m_vpMatrix = this->getProjectionMatrix() * this->getViewMatrix();
}

void Knee::Camera::setPosition(glm::vec3 v){
	GeneralObject::setPosition(v);
	
	// update vp
	this->updateViewProjectionMatrix();
}

// in roll, pitch, yaw, order
// roll value (r.z) is ignored
void Knee::Camera::setRotation(glm::vec3 r){
	// call general object setter
	GeneralObject::setRotation(r);
	
	/*this->m_forward = glm::normalize(glm::vec3(
		sin(r.y)*cos(r.x),
		sin(r.x),
		cos(r.y)*cos(r.x)
	));*/
	
	// update vp
	this->updateViewProjectionMatrix();
}

// -------------------- //
// PerspectiveCamera //

Knee::PerspectiveCamera::PerspectiveCamera() : Knee::Camera() {
}

// fov is expected in radians
void Knee::PerspectiveCamera::setPerspectiveProperties(float fov, float aspectRatio, float near, float far){
	this->m_projectionMatrix = glm::perspective(fov, aspectRatio, near, far);
	
	// update vp
	this->updateViewProjectionMatrix();
}

// -------------------- //
// RenderableObjectShaderProgram //

Knee::RenderableObjectShaderProgram::RenderableObjectShaderProgram(float fov, float aspectRatio, float near, float far) : Knee::ShaderProgram() {
	this->m_camera = new PerspectiveCamera();

	this->m_camera->setPerspectiveProperties(fov, aspectRatio, near, far);
}

Knee::RenderableObjectShaderProgram::RenderableObjectShaderProgram(Knee::PerspectiveCamera* camera) : Knee::ShaderProgram(), m_camera(camera) {}

Knee::PerspectiveCamera* Knee::RenderableObjectShaderProgram::getCamera(){
	return this->m_camera;
}

// NOTE: this method will look for certain uniforms (but will silently continue if not found):
// mvp - (projection * view * model) matrix
// transposeInverseModel - matrix for transforming the normals such that they match the model after it is transformed by the model matrix.
/*void Knee::RenderableObjectShaderProgram::drawRenderableObject(RenderableObject* renderableObject){
	// reset bound textures
	// TODO: should probably have some kind of system in place to keep textures from previous binds
	this->resetBoundTextures();

	// calculate mvp matrix
	glm::mat4 mvp = this->m_camera.getViewProjectionMatrix() * renderableObject->getModelMatrix();
	
	// calculate transposeInverseModel matrix
	glm::mat4 tim = glm::transpose(glm::inverse(renderableObject->getModelMatrix()));
	
	// set uniforms
	this->setUniformMat4("u_mvp", mvp);
	this->setUniformMat4("transposeInverseModel", tim);
	
	// bind texture if present
	if(renderableObject->hasTexture()){
		this->bindTexture2D("u_sampler", renderableObject->getTexture());
	}

	// draw vertex data
	this->drawVertexData( renderableObject->getVertexData() );
}*/

// -------------------- //
// RenderableObject //

Knee::RenderableObject::RenderableObject(Knee::VertexData* vertexData, Knee::Texture2D* texture, Knee::RenderableObjectShaderProgram* program) : m_vertexData(vertexData), m_texture(texture), m_shaderProgram(program) {}

Knee::RenderableObjectShaderProgram* Knee::RenderableObject::getShaderProgram(){
	return this->m_shaderProgram;
}

void Knee::RenderableObject::setShaderProgram(RenderableObjectShaderProgram* program){
	this->m_shaderProgram = program;
}

// NOTE: m_vertexData is constant in RenderableObject, so do not attempt to modify
const Knee::VertexData* Knee::RenderableObject::getVertexData() const {
	return this->m_vertexData;
}

Knee::Texture2D* Knee::RenderableObject::getTexture(){
	return this->m_texture;
}

void Knee::RenderableObject::setTexture(Knee::Texture2D* texture){
	this->m_texture = texture;
}

bool Knee::RenderableObject::hasTexture(){
	return this->m_texture != NULL;
}

void Knee::RenderableObject::draw(){
	// reset bound textures in shader program
	// TODO: should probably have some kind of system in place to keep textures from previous binds
	this->m_shaderProgram->resetBoundTextures();

	// calculate mvp matrix
	glm::mat4 mvp = this->m_shaderProgram->getCamera()->getViewProjectionMatrix() * this->getModelMatrix();
	
	// calculate transposeInverseModel matrix
	glm::mat4 tim = glm::transpose(glm::inverse(this->getModelMatrix()));
	
	// set uniforms
	this->m_shaderProgram->setUniformMat4("u_mvp", mvp);
	this->m_shaderProgram->setUniformMat4("transposeInverseModel", tim);
	
	// bind texture if present
	if(this->hasTexture()){
		this->m_shaderProgram->bindTexture2D("u_sampler", this->getTexture());
	}

	// draw vertex data
	this->m_shaderProgram->drawVertexData( this->getVertexData() );
}