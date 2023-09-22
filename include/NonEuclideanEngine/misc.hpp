#pragma once

#include <glm/glm.hpp>

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
			glm::vec3 getPosition();
			glm::vec3 getRotation();
			glm::vec3 getScale();

			void setPosition(glm::vec3 position);
			void setRotation(glm::vec3 rotation);
			void setScale(glm::vec3 scale);
			
			void changePosition(glm::vec3 change);
			void changeRotation(glm::vec3 change);
			void changeScale(glm::vec3 change);
			
			// note that calling this will make the cached values for position, rotation, and scale not synced with the model matrix
			// this means that getters for all of these properties become unsafe until the model matrix is updated
			// generally you should use the slower but safer applyGeneralObjectTransformation unless you're positive that you don't need to access any of these values until the model matrix is updated
			void applyMatrixTransformation(glm::mat4 matrixTransformation);

			GeneralObject getInverseGeneralObject();
			
			void addGeneralObject(GeneralObject obj);
			void subtractGeneralObject(GeneralObject obj);

			// this essentially does the same thing as applyMatrixTransformation, but it updates the position, rotation, and scale values appropriately
			// it's also a bit slower since it has to update all of the values, recalculate the applicable transformation matrices, and multiply the 3 matrices together for the resulting transformation.
			// while this is generally preferred, use applyMatrixTransformation if you're positive you don't need to access any of those values and you want to shave off a bit of time
			void applyGeneralObjectTransformation(GeneralObject obj);
			void applyGeneralObjectTransformation(GeneralObject obj, uint32_t n);

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

			// WARNING! this will desync the model matrix from the position, rotation, and scale values.  any call to updateModelMatrix will resync the model matrix.
			void setModelMatrix(glm::mat4 mat);
	};
}