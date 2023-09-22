#pragma once

#include <SDL2/SDL_image.h>
#include <glad/glad.h>
#include <string>

namespace Knee {
	// class for creating GL textures from files using SDL_Surface
	// SHOULD NOT be created until an application instance exists, initializing opengl
	// note that this should only be created once and passed as a pointer or reference to ensure that there's no texture instances with a disposed SDL_Surface (which gets thrown away on destruction)
	class Texture2D {
		// gl texture name holder
		GLuint m_glTexture;
		
		// width + height in pixels
		uint32_t m_width;
		uint32_t m_height;

		protected:
			GLint SDLPixelFormatToInternalGLFormat(const SDL_PixelFormat*);
			GLint SDLPixelFormatToGLFormat(const SDL_PixelFormat*);
			void createGLTexture(GLenum internalFormat, uint32_t width, uint32_t height, GLenum format, GLenum type, const GLvoid* data);
			void createGLTexture(SDL_Surface*);

		public:
			// constructor from file - loads image data from file, and then creates gl texture
			Texture2D(std::string);

			// constructor for blank texture - just creates an empty gl texture given the desired values
			Texture2D(uint32_t, uint32_t, GLenum);

			~Texture2D();

			uint32_t getWidth();
			uint32_t getHeight();
						
			GLint getGLTexture();
	};
}