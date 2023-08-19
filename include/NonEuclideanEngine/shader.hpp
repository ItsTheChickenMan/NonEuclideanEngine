#pragma once

#include <glad/glad.h>
#include <map>
#include <vector>
#include <string>

namespace Knee {
	class ShaderProgram {
		// PRIVATE MEMBERS //
		static int32_t m_maxTextureUnits; // maximum supported texture units (implementation dependent)
		
		// program reference
		GLuint m_program;
		bool m_compiled = false;
		
		// shader management
		std::vector<GLuint> m_shaders;
		
		// uniform management
		std::map<std::string, GLint> m_uniforms;
		
		// texture management
		uint32_t m_textureUnits = 0; // number of currently bound texture units
		
		protected:
			GLchar* getShaderInfoLog(GLuint);
			GLchar* getProgramInfoLog();
			
			
			void markAttachedShadersForDeletion();
		public:
			ShaderProgram();
			~ShaderProgram();
		
			static void loadMaxTextureUnits();
			
			bool isCompiled();
			GLint getUniformLocation(std::string);
			
			int32_t attachShader(GLenum, std::string);
			int32_t loadUniformLocations();
			
			int32_t compile();
			void destroy();
			
			void use();
	};
}