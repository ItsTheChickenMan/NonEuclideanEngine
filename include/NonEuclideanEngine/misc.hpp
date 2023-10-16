#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

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
		const static char TRANSLATION_CHAR = 't';
		const static char ROTATION_CHAR = 'r';
		const static char SCALE_CHAR = 's';

		// note that these values always represent transformation in the "natural order" of scaling, then rotating, then translating.  this order results in these rules:
		//
		// position is relative to origin
		// rotation is relative to local origin
		// scale is relative to local origin
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
			// constructors //

			GeneralObject();
			GeneralObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

			// transformation order getters/setters //
			
			// returns a new GeneralObject representing a transformation in natural order equivalent to applying this transformation using the given transformation order
			Knee::GeneralObject usingTransformationOrder(std::string transformationOrder);

			// this takes the existing transformation in the natural order, determines the resulting transformation from the provided transformationOrder, and then converts that transformation to natural order.
			// note that this method is non invertible.  if you want to keep the original transformation, use usingTransformationOrder(std::string) instead to get a new GeneralObject with the transformation order applied
			// note that calling with transformationOrder = "srt" or = "rst" does nothing, since both of these are natural orderings (srt is used by GeneralObject but rst has the same effect)
			void setTransformationOrder(std::string transformationOrder);

			// position + rotation + scale getters/setters //

			glm::vec3 getPosition() const ;
			glm::vec3 getRotation() const ;
			glm::vec3 getScale() const ;

			void setPosition(glm::vec3 position);
			void setRotation(glm::vec3 rotation);
			void setScale(glm::vec3 scale);
			
			void changePosition(glm::vec3 change);
			void changeRotation(glm::vec3 change);
			void changeScale(glm::vec3 change);

			void rotateAboutAxis(double angle, glm::vec3 axis);

			// GeneralObject operations //
			void copyValues(GeneralObject other);

			GeneralObject getInverseGeneralObject();
			
			void addGeneralObject(GeneralObject obj);

			virtual void applyTransformation(GeneralObject t);

			friend GeneralObject operator*(GeneralObject lhs, const GeneralObject& rhs){
				Knee::GeneralObject out;

				out.applyTransformation(rhs);
				out.applyTransformation(lhs);

				return out;
			}
			
			GeneralObject& operator*=(const GeneralObject& rhs);

			// transformation matrix getters/setters //

			glm::mat4 getTranslationMatrix();
			glm::mat4 getRotationMatrix();
			glm::mat4 getScaleMatrix();

			void updateTranslationMatrix();
			void updateRotationMatrix();
			void updateScaleMatrix();

			void applyRotationMatrix(glm::mat4 rotationMatrix);
			
			// axes //

			// get local x, y, z axes
			// this is slightly faster for getting all 3 since it only needs to do matrix multiplication twice, then just uses cross product to get third vector
			void getLocalAxes(glm::vec3& x, glm::vec3& y, glm::vec3& z);

			// get the direction that this object is currently pointing in
			glm::vec3 getLocalZAxis();

			// get the up direction of this object
			glm::vec3 getLocalYAxis();

			// get the cross product of the forward and up vectors
			glm::vec3 getLocalXAxis();

			// alias for getLocalZAxis
			glm::vec3 getForwardVector();

			// alias for getLocalYAxis
			glm::vec3 getUpVector();

			// alias for getLocalXAxis
			glm::vec3 getCrossVector();

			// model matrix //

			glm::mat4 getModelMatrix();

			// based on the current values of the transformation matrices			
			void updateModelMatrix();
	};

	class MathUtils {
		public:
			// check if a 3D line segment is intersecting a 3D plane defined by a position, rotation matrix, and size
			static bool isLineSegmentIntersectingPlane(const glm::vec3& start, const glm::vec3& end, 
				const glm::vec3& planeCenter,
				const glm::mat4& planeRotationMatrix,
				const glm::vec3& planeSize
			);

			// check if two values are approximately equal to eachother given a threshold
			static bool approximatelyEqual(double v1, double v2, double threshold);

			static bool pointIsInAABB(
				const glm::vec2& point,
				const glm::vec2& boxCenter,
				const glm::vec2& boxSize
			);

			// check if a 2D line segment is intersecting another 2D line segment
			// https://stackoverflow.com/a/3746601
			static bool lineSegmentsIntersecting(
				const glm::vec2& s1,
				const glm::vec2& e1,
				const glm::vec2& s2,
				const glm::vec2& e2
			);

			// check if a 2D line segment is intersecting or within
			// ALSO checks if AABB contains line segment, so not necessarily intersection
			static bool lineSegmentIntersectingOrWithinAABB(
				const glm::vec2& s1,
				const glm::vec2& e1,
				const glm::vec2& boxCenter,
				const glm::vec2& boxSize
			);
	};
}