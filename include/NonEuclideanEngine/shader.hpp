#pragma once

#include <glad/glad.h>
#include <map>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

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
			
			bool setUniformMat4(std::string, glm::mat4);
			
			int32_t compile();
			void destroy();
			
			void use();
			
			void drawArrays(uint32_t);
			void drawVertexData(const Knee::VertexData*);
	};
	
	
	// abstract class defining RenderableObjects and their properties.  Any object that you want to be renderable by a RenderableObjectShaderProgram should inherit from this class and overload the appropriate methods.
	class RenderableObject {
		// vertex data to be used when rendering
		const VertexData* m_vertexData;
	
		// caches for values to be reused if one changes
		glm::vec3 m_position = glm::vec3(0);
		glm::vec3 m_rotation = glm::vec3(0);
		glm::vec3 m_scale = glm::vec3(1);
	
		// model matrix
		glm::mat4 m_modelMatrix = glm::mat4(1);
		
		public:
			RenderableObject(Knee::VertexData*);
			
			glm::vec3 getPosition();
			glm::vec3 getRotation();
			glm::vec3 getScale();
			
			void setPosition(glm::vec3);
			void setRotation(glm::vec3);
			void setScale(glm::vec3);
			
			void changePosition(glm::vec3);
			void changeRotation(glm::vec3);
			void changeScale(glm::vec3);
			
			void updateModelMatrix();
			
			glm::mat4 getModelMatrix();
			const Knee::VertexData* getVertexData() const;
		
			void use();
	};
	
	class Camera {
		// position, camera line of sight
		glm::vec3 m_position;
		glm::vec3 m_forward;
		glm::vec3 m_up = glm::vec3(0, 1, 0); // TODO: will this ever need to be customized?
		
		glm::mat4 m_viewMatrix = glm::mat4(1);
		
		// (m_projectionMatrix * m_viewMatrix)
		glm::mat4 m_vpMatrix = glm::mat4(1);
		
		protected:
			// a copy of the matrices used to transform m_vpMatrix.  they're only used internally as a reference if the other is changed, but generally m_vpMatrix will be used for shaders so that the matrix multiplication of projection * view doesn't have to be done more than once per frame (unless necessary)
			glm::mat4 m_projectionMatrix = glm::mat4(1);
			//glm::mat4 m_viewMatrix;
		
		public:
			Camera();
			
			glm::mat4 getProjectionMatrix();
			glm::mat4 getViewMatrix();
			glm::mat4 getViewProjectionMatrix();
			
			// update view matrix based on the current values of m_position and m_rotation
			void updateViewMatrix();
			
			// update m_vpMatrix based on the current values of the projection and view matrices.
			// this is called automatically whenever setCameraPosition and setCameraRotation are called.
			void updateViewProjectionMatrix();
			
			void setPosition(glm::vec3);
			void setRotation(glm::vec3);
	};
	
	class PerspectiveCamera : public Camera {
		public:
			PerspectiveCamera();
			
			// automatically calls updateViewProjectionMatrix()
			void setPerspectiveProperties(float, float, float, float);
	};
	
	// class for rendering RenderableObjects, rendered with perspective projection from the viewpoint of a camera.
	class RenderableObjectShaderProgram : public ShaderProgram {
		public:
			Knee::PerspectiveCamera m_camera;
		
		public:
			RenderableObjectShaderProgram(float, float, float, float);
		
			void drawRenderableObject(RenderableObject&);
	};
}