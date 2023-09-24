#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <string>
#include <chrono>

namespace Knee {
	const std::string WARNING_PREFACE = "Warning: ";
	const std::string ERROR_PREFACE = "Error: ";
	
	// timer for measuring delta between updates and for general time that the program has been running.
	class DeltaTimer {
		// delta timer
		std::chrono::high_resolution_clock m_deltaTimer;
		
		std::chrono::time_point<std::chrono::high_resolution_clock> m_timeOfLastReset;
		
		// start time
		double m_startTime = 0.0;

		public:
			DeltaTimer();
			
			void resetDelta();
			void resetTime();
			
			double getTime();
			double getDelta();
			double getDeltaAndReset();
			
			// seconds
			void pauseThread(double);
	};

	// any object with a position, rotation, and scale in general 3D space.
	class GeneralObject {
		glm::vec3 m_position = glm::vec3(0);
		glm::vec3 m_rotation = glm::vec3(0);
		glm::vec3 m_scale = glm::vec3(1);
		
		// individual transformation matrices
		glm::mat4 m_translationMatrix = glm::mat4(1);
		glm::mat4 m_rotationMatrix = glm::mat4(1);
		glm::mat4 m_scaleMatrix = glm::mat4(1);

		// model matrix
		glm::mat4 m_modelMatrix = glm::mat4(1);
		
		public:
			/*friend GeneralObject operator*(GeneralObject lhs, const GeneralObject& rhs){
				Knee::GeneralObject out;

				out.applyTransformation(rhs);
				out.applyTransformation(lhs);

				return out;
			}
			
			GeneralObject& operator*=(const GeneralObject& rhs);

			void applyTransformation(GeneralObject t);

			void updatePositionFromModelMatrix();
			*/

			glm::vec3 getPosition() const ;
			glm::vec3 getRotation() const ;
			glm::vec3 getScale() const ;

			void setPosition(glm::vec3 position);
			void setRotation(glm::vec3 rotation);
			void setScale(glm::vec3 scale);
			
			void changePosition(glm::vec3 change);
			void changeRotation(glm::vec3 change);
			void changeScale(glm::vec3 change);
			
			GeneralObject getInverseGeneralObject();
			
			void addGeneralObject(GeneralObject obj);
			void subtractGeneralObject(GeneralObject obj);

			glm::mat4 getTranslationMatrix();
			glm::mat4 getRotationMatrix();
			glm::mat4 getScaleMatrix();
			
			// get the direction that this object is currently pointing in
			glm::vec3 getForwardVector();
			
			void updateTranslationMatrix();
			void updateRotationMatrix();
			void updateScaleMatrix();
			
			void updateModelMatrix();

			glm::mat4 getModelMatrix();

			void applyMatrixTransformation(glm::mat4 model);
	};
}