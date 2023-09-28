#pragma once

#include <NonEuclideanEngine/texture.hpp>
#include <NonEuclideanEngine/misc.hpp>

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
			
			void use() const;
	};
	
	class ShaderProgram {
		// PRIVATE MEMBERS //
		static int32_t MAX_TEXTURE_UNITS; // maximum supported texture units (implementation dependent)
		
		// program reference
		GLuint m_program;
		bool m_compiled = false;
		
		// shader management
		std::vector<GLuint> m_shaders;
		
		// uniform management
		std::map<std::string, GLint> m_uniforms;

		// amount of textures bound currently
		uint32_t m_boundTextureCount;

		protected:
			GLchar* getShaderInfoLog(GLuint);
			GLchar* getProgramInfoLog();
			
			void markAttachedShadersForDeletion();
		public:
			ShaderProgram();
			~ShaderProgram();
			
			static void loadMaxTextureUnits();
			uint32_t getMaxTextureUnits();
			
			void bindTexture2D(std::string, Knee::Texture2D*);
			void resetBoundTextures();

			bool isCompiled();
			GLint getUniformLocation(std::string);
			
			// returns 0 upon success and -1 upon error
			int32_t attachShader(GLenum, std::string);
			int32_t loadUniformLocations();
			
			bool setUniformMat4(std::string, glm::mat4);
			
			int32_t compile();
			void destroy();
			
			void use();
			
			void drawArrays(uint32_t);
			void drawVertexData(const Knee::VertexData*);
	};
	
	class Camera : public GeneralObject {
		glm::mat4 m_viewMatrix = glm::mat4(1);
		
		// (m_projectionMatrix * m_viewMatrix)
		glm::mat4 m_vpMatrix = glm::mat4(1);

		protected:
			// a copy of the matrices used to transform m_vpMatrix.  they're only used internally as a reference if the other is changed, but generally m_vpMatrix will be used for shaders so that the matrix multiplication of projection * view doesn't have to be done more than once per frame (unless necessary)
			// projection matrix is public here because subclasses are expected to mess with it a bit, but not so much the view matrix.
			glm::mat4 m_projectionMatrix = glm::mat4(1);
			//glm::mat4 m_viewMatrix;
		
		public:
			Camera();
			
			GeneralObject* asGeneralObject();
			
			glm::mat4 getProjectionMatrix();
			glm::mat4 getViewMatrix();
			glm::mat4 getViewProjectionMatrix();
			
			// update view matrix based on the current values of m_position and m_rotation
			void updateViewMatrix();
			
			// update m_vpMatrix based on the current values of the projection and view matrices.
			// this is called automatically whenever setPosition and setRotation are called.
			void updateViewProjectionMatrix();

			void setPosition(glm::vec3 position);
			void setRotation(glm::vec3 rotation);
	};
	
	class PerspectiveCamera : public Camera {
		float m_near = 0.0;
		float m_far = 0.0;
		float m_fov = 0.0;
		float m_aspectRatio = 0.0;

		public:
			PerspectiveCamera();
			
			float getNear();
			float getFar();
			float getFOV();
			float getAspectRatio();

			// automatically calls updateViewProjectionMatrix()
			void setPerspectiveProperties(float fov, float aspectRatio, float near, float far);
	};
	
	// class for rendering RenderableObjects, rendered with perspective projection from the viewpoint of a camera.
	class RenderableObjectShaderProgram : public ShaderProgram {
		Knee::PerspectiveCamera* m_camera;
		
		public:
			RenderableObjectShaderProgram(float fov, float aspectRatio, float near, float far);

			// link an existing camera to this shader
			RenderableObjectShaderProgram(Knee::PerspectiveCamera* camera);

			Knee::PerspectiveCamera* getCamera();
	};

	// abstract class defining RenderableObjects and their properties.  Any object that you want to be renderable by a RenderableObjectShaderProgram should inherit from this class and overload the appropriate methods.
	class RenderableObject : public virtual GeneralObject {
		// vertex data to be used when rendering
		const VertexData* m_vertexData;

		// shader program to use when rendering
		RenderableObjectShaderProgram* m_shaderProgram;

		protected:
			// texture to be used when rendering
			Knee::Texture2D* m_texture;
		public:
			RenderableObject(VertexData* vertexData, Texture2D* texture, RenderableObjectShaderProgram* program);
			
			RenderableObjectShaderProgram* getShaderProgram();
			void setShaderProgram(RenderableObjectShaderProgram* program);

			const VertexData* getVertexData() const;

			Knee::Texture2D* getTexture();
			void setTexture(Knee::Texture2D* texture);
			bool hasTexture();

			virtual void draw();
	};
}