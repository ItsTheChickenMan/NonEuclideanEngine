#pragma once

#include <glm/glm.hpp>

#include <string>
#include <chrono>

namespace Knee {
	const std::string ERROR_PREFACE = "Error: ";
	
	// timer for measuring delta between updates and for general time that the program has been running.
	class DeltaTimer {
		// delta timer
		std::chrono::high_resolution_clock m_deltaTimer;
		
		std::chrono::time_point<std::chrono::high_resolution_clock> m_timeOfLastReset;
		
		public:
			DeltaTimer();
			
			void resetDelta();
			double getTime();
			double getDelta();
			double getDeltaAndReset();
			
			// seconds
			void pauseThread(double);
	};
	
	// any object with a position, rotation, and size in general 3D space.
	// simply stores those 3 values as vec3s as well as a 4x4 model matrix which is updated whenever the former 3 values are changed.
	class GeneralObject {
		glm::vec3 m_position = glm::vec3(0);
		glm::vec3 m_rotation = glm::vec3(0);
		glm::vec3 m_scale = glm::vec3(1);
		
		// model matrix
		glm::mat4 m_modelMatrix = glm::mat4(1);
		
		public:
			glm::vec3 getPosition();
			glm::vec3 getRotation();
			glm::vec3 getScale();
			
			virtual void setPosition(glm::vec3);
			virtual void setRotation(glm::vec3);
			virtual void setScale(glm::vec3);
			
			void changePosition(glm::vec3);
			void changeRotation(glm::vec3);
			void changeScale(glm::vec3);
			
			glm::mat4 getTranslationMatrix();
			glm::mat4 getRotationMatrix();
			glm::mat4 getScaleMatrix();
			
			// get the direction that this object is currently pointing in
			glm::vec3 getForwardVector();
			
			void updateModelMatrix();
			
			glm::mat4 getModelMatrix();
	};
}