#include <NonEuclideanEngine/texture.hpp>

#include <SDL2/SDL_image.h>
#include <iostream>

// Texture //

Knee::Texture2D::Texture2D(std::string filename){
	SDL_Surface* surface = IMG_Load(filename.c_str());

	// create gl texture
	this->createGLTexture(surface);

	// free surface memory
	SDL_FreeSurface(surface);
}

Knee::Texture2D::Texture2D(uint32_t width, uint32_t height, GLenum type) {
	this->createGLTexture(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, NULL);
};

Knee::Texture2D::~Texture2D(){}

uint32_t Knee::Texture2D::getWidth(){
	return this->m_width;
}

uint32_t Knee::Texture2D::getHeight(){
	return this->m_height;
}

GLint Knee::Texture2D::SDLPixelFormatToInternalGLFormat(const SDL_PixelFormat* sdlFormat){
	if (sdlFormat == nullptr) {
		// Handle the case where sdlFormat is nullptr
		// You can return a default format or throw an error as needed
		return -1;
	}

	// get SDL representation of format
	uint32_t format = sdlFormat->format;

	// get bytes per pixel
	uint8_t bytesPerPixel = sdlFormat->BytesPerPixel;

	// Map SDL pixel format attributes to OpenGL format and type
	switch (sdlFormat->format) {
		case SDL_PIXELFORMAT_RGBA32:
			return GL_RGBA;
		case SDL_PIXELFORMAT_RGB24:
			return GL_RGB;
		default:
			// Handle unsupported pixel formats or return a default format
			return -1;
	}
}

GLint Knee::Texture2D::SDLPixelFormatToGLFormat(const SDL_PixelFormat* sdlFormat){
	if (sdlFormat == nullptr) {
		// Handle the case where sdlFormat is nullptr
		// You can return a default format or throw an error as needed
		return -1;
	}

	// Map SDL pixel format attributes to OpenGL format and type
	switch (sdlFormat->format) {
		case SDL_PIXELFORMAT_RGBA32:
			return GL_RGBA;
		case SDL_PIXELFORMAT_RGB24:
			return GL_RGB;
		default:
			// Handle unsupported pixel formats or return a default format
			return -1;
	}
}

void Knee::Texture2D::createGLTexture(GLenum internalFormat, uint32_t width, uint32_t height, GLenum format, GLenum type, const GLvoid* data){
	// copy width + height into member variables
	this->m_width = width;
	this->m_height = height;

	// generate 1 texture
	glGenTextures(1, &this->m_glTexture);

	// bind texture for 2D texture operations
	glBindTexture(GL_TEXTURE_2D, this->m_glTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, 
		internalFormat,
		width,
		height,
		0,
		format,
		type,
		data
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);
}

// create gl texture from current surface
void Knee::Texture2D::createGLTexture(SDL_Surface* surface){
	GLint internalFormat = this->SDLPixelFormatToInternalGLFormat(surface->format);
	GLint format = this->SDLPixelFormatToGLFormat(surface->format);

	// create GL texture
	this->createGLTexture(internalFormat, surface->w, surface->h, format, GL_UNSIGNED_BYTE, surface->pixels);

	// bind texture
	glBindTexture(GL_TEXTURE_2D, this->m_glTexture);

	// set unpack alignment
	// FIXME: actually match this with the given format
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// generate mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLint Knee::Texture2D::getGLTexture(){
	return this->m_glTexture;
}