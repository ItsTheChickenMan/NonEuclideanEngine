#pragma once

#include <glad/glad.h>
#include <map>
#include <vector>
#include <string>

namespace Knee {
	// class containing basic vertex data for a model.  allows for positions, texture coordinates, and normals in any format.  note that for general safety, the copy constructor for the class is disabled to prevent situations where two VertexData objects point to the same vbo and vao, leading to a bad situation if one were to delete one of the copies and not the other.
	// any method that requires VertexData will take it as a const reference
	class VertexData {
		// vertex attribute constants
		static const std::string VA_POSITION_STR;
		static const std::string VA_TEXCOORD_STR;
		static const std::string VA_NORMAL_STR;
		
		static const uint32_t VA_POSITION_SIZE;
		static const uint32_t VA_TEXCOORD_SIZE;
		static const uint32_t VA_NORMAL_SIZE;
		
		static const uint32_t VA_POSITION_INDEX;
		static const uint32_t VA_TEXCOORD_INDEX;
		static const uint32_t VA_NORMAL_INDEX;
		
		
		// vertex buffer object
		GLuint m_vbo;
		
		// vertex array object
		GLuint m_vao;
		
		// number of vertices
		uint32_t m_vertexCount;
		
		public:
			VertexData(float*, uint32_t, GLsizeiptr, std::string);
			~VertexData();
			
			// disable copy constructor and assignment operator
			VertexData(const VertexData&) = delete;
			VertexData& operator=(VertexData const&) = delete;
			
			uint32_t getVertexCount() const ;
			
			void use() const ;
	};
	
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
			
			void drawArrays(uint32_t);
			void drawVertexData(const Knee::VertexData&);
	};
	
	class RenderableObjectShaderProgram : public ShaderProgram {
		
		
		public:
			
	};
	
	// abstract class defining RenderableObjects and their properties.  Any object that you want to be renderable by a RenderableObjectShaderProgram should inherit from this class and overload the appropriate methods.
	class RenderableObject {
		public:
			
	};
}