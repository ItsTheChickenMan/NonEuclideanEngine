#include <NonEuclideanEngine/misc.hpp>

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/intersect.hpp>

// -------------------- //
// DeltaTimer //

Knee::DeltaTimer::DeltaTimer(){
	this->resetDelta();
	this->resetTime();
}

void Knee::DeltaTimer::resetDelta(){
	this->m_timeOfLastReset = this->m_deltaTimer.now();
}

void Knee::DeltaTimer::resetTime(){
	this->m_startTime = this->getTime();
}

double Knee::DeltaTimer::getTime(){
	std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> now = this->m_deltaTimer.now();
	
	return now.time_since_epoch().count() - this->m_startTime;
}

double Knee::DeltaTimer::getDelta(){
	std::chrono::duration<double> delta = this->m_deltaTimer.now() - this->m_timeOfLastReset;
	
	return delta.count();
}

double Knee::DeltaTimer::getDeltaAndReset(){
	std::chrono::time_point<std::chrono::high_resolution_clock> now = this->m_deltaTimer.now();
	
	std::chrono::duration<double> delta = now - this->m_timeOfLastReset;
	
	this->m_timeOfLastReset = now;
	
	return delta.count();
}

void Knee::DeltaTimer::pauseThread(double seconds){
	double stopTime = this->getTime() + seconds;
	
	while(this->getTime() < stopTime);
}

// -------------------- //
// GeneralObject //

// constructors //

Knee::GeneralObject::GeneralObject(){}

Knee::GeneralObject::GeneralObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale){
	this->setPosition(position);
	this->setRotation(rotation);
	this->setScale(scale);
}

// transformation order getters/setters //

Knee::GeneralObject Knee::GeneralObject::usingTransformationOrder(std::string transformationOrder){
	// copy general object
	Knee::GeneralObject n = *this;

	// set transformation order to provided
	n.setTransformationOrder(transformationOrder);

	// return new
	return n;
}

void Knee::GeneralObject::setTransformationOrder(std::string transformationOrder){
	// fortunately, translation is the only transformation here affected by different translation orders.
	// rotation and scaling are commutative in the sense that it no matter which comes first, rotation and scale are unaffected by each other.
	// it also happens that rotation and scale are unaffected by translation.  this means that the only new thing we have to calculate when changing the transformation order is the new translation of the transformation.
	// note that this is assuming the transformation order is exactly 3 unique transformations.  not sure if this applies in other scenarios.

	// do nothing if new transformation order is natural order
	if(transformationOrder == "srt" || transformationOrder == "rst") return;

	// check that transformationOrder is exactly 3 transformations
	if(transformationOrder.length() != 3) return;

	// TODO: check that they're all unique

	// calculate new translation by determining which transformation come after translation in the transformation order and applying them in order
	bool applyingTransformations = false;

	for(char& c : transformationOrder){
		// check if we should start applying transformation
		if(!applyingTransformations){
			if(c == Knee::GeneralObject::TRANSLATION_CHAR){
				applyingTransformations = true;
			}

			continue;
		}

		// apply applicable transformation
		switch(c){
			case Knee::GeneralObject::ROTATION_CHAR:
				this->m_position = glm::vec3(this->getRotationMatrix() * glm::vec4(this->m_position, 1));
				break;
			case Knee::GeneralObject::SCALE_CHAR:
				this->m_position = glm::vec3(this->getScaleMatrix() * glm::vec4(this->m_position, 1));
				break;
		}
	}

	// update translation matrix + model matrix
	this->updateTranslationMatrix();
	this->updateModelMatrix();
}

// position + rotation + scale getters/setters //

glm::vec3 Knee::GeneralObject::getPosition() const { return this->m_position; }
glm::vec3 Knee::GeneralObject::getRotation() const { return this->m_rotation; }
glm::vec3 Knee::GeneralObject::getScale() const { return this->m_scale; }

void Knee::GeneralObject::setPosition(glm::vec3 position){
	this->m_position = position;
	
	this->updateTranslationMatrix();
	this->updateModelMatrix();
}

void Knee::GeneralObject::setRotation(glm::vec3 rotation){
	this->m_rotation = rotation;
	
	this->updateRotationMatrix();
	this->updateModelMatrix();
}

void Knee::GeneralObject::setScale(glm::vec3 scale){
	this->m_scale = scale;
	
	this->updateScaleMatrix();
	this->updateModelMatrix();
}

void Knee::GeneralObject::changePosition(glm::vec3 change){
	glm::vec3 old = this->getPosition();
	
	this->setPosition(old + change);
}

void Knee::GeneralObject::changeRotation(glm::vec3 change){
	glm::vec3 old = this->getRotation();
	
	this->setRotation(old + change);
}

void Knee::GeneralObject::changeScale(glm::vec3 change){
	glm::vec3 old = this->getScale();
	
	this->setScale(old * change);
}

void Knee::GeneralObject::rotateAboutAxis(double angle, glm::vec3 axis){
	// compute matrix
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1), (float)angle, axis);

	// apply
	this->applyRotationMatrix(rotationMatrix);
}

// GeneralObject operations //

void Knee::GeneralObject::copyValues(GeneralObject other){
	this->m_position = other.getPosition();
	this->m_rotation = other.getRotation();
	this->m_scale = other.getScale();

	// update matrices
	this->updateTranslationMatrix();
	this->updateRotationMatrix();
	this->updateScaleMatrix();
	this->updateModelMatrix();
}

Knee::GeneralObject Knee::GeneralObject::getInverseGeneralObject(){
	// create object with inverse values
	Knee::GeneralObject obj(-this->getPosition(), -this->getRotation(), 1.0f / this->getScale());

	// set transformation order to reverse of natural
	obj.setTransformationOrder("trs");

	return obj;
}

void Knee::GeneralObject::addGeneralObject(GeneralObject obj){
	this->changePosition(obj.getPosition());
	this->changeRotation(obj.getRotation());
	this->changeScale(obj.getScale());
}

void Knee::GeneralObject::applyTransformation(GeneralObject t){
	// scale applies as is
	// rotation is determined by multiplying the rotation matrices and extracting the euler angles.  this needs to be done so that rotation transformations apply rotation relative to global axes rather than to this object's local axes
	// translation is determined just by transforming the existing translation by the model matrix of the other object
		// each model matrix is in natural order, equivalent to some form of T * R * S
			// T, R, S are matrices and T = translation matrix, R = rotation matrix, and S = scale matrix representing some 3 component vectors p, r, s where p = position, r = rotation, and s = scale
		// this performs transformation in the order scale, rotate, translate
		// to apply a transformation to another, we're essentially computing new p, r, s such that the transformation matrices T3, R3, S3 satisfy the equation T3 * R3 * S3 = T2 * R2 * S2 * T1 * R1 * S1
		// where the order is scale by first, rotate by first, translate by first, scale by second, rotate by second, translate by second
		// to compute the transformation's natural components (scale, rotation, and position), we need to determine how much the object moves as a result of individual transformations
		// S1 and R1 have no effect on translation because they are the first transformations to occur, when the overall translation is 0 (scaling and rotating 0 does nothing)
		// so we're left with determining the total translation on v where v is a 3 component vector in the equation T2 * R2 * S2 * T1 * v
		// we know that T1 * v = v + p1
		// so: T2 * R2 * S2 * (T1 * v) = T2 * R2 * S2 * (v + p1) = (T2 * R2 * S2)v + (T2 * R2 * S2)p1
		// to determine total translation on v, assume v is 0:
		// = (T2 * R2 * S2 )(0) + (T2 * R2 * S2)p1 = (T2 * R2 * S2)p1
		// and T2 * R2 * S2 is equivalent to the model matrix of the second object
		// so the total translation is M2 * p1

	// we don't use setters here because they automatically invoke updateModelMatrix, which would be redudant here since we know we don't need to update until the last value is changed
	// so we defer the update to the end of the method

	// apply scale
	this->m_scale *= t.getScale();

	// apply rotation
	this->applyRotationMatrix(t.getRotationMatrix());

	// transform translation
	this->m_position = glm::vec3(t.getModelMatrix() * glm::vec4(this->m_position, 1));

	// update transformation matrices
	this->updateTranslationMatrix();
	this->updateRotationMatrix();
	this->updateScaleMatrix();

	// update model matrix
	this->updateModelMatrix();
}

Knee::GeneralObject& Knee::GeneralObject::operator*=(const GeneralObject& rhs){
	return *this = rhs * (*this);
}

// transformation matrix getters/setters //

glm::mat4 Knee::GeneralObject::getTranslationMatrix(){
	return this->m_translationMatrix;
}

glm::mat4 Knee::GeneralObject::getRotationMatrix(){
	return this->m_rotationMatrix;
}

glm::mat4 Knee::GeneralObject::getScaleMatrix(){
	return this->m_scaleMatrix;
}

void Knee::GeneralObject::updateTranslationMatrix(){
	this->m_translationMatrix = glm::translate(glm::mat4(1), this->m_position);
}

void Knee::GeneralObject::updateRotationMatrix(){
	glm::mat4 out = glm::eulerAngleYXZ(this->m_rotation.y, this->m_rotation.x, this->m_rotation.z);	
	//glm::mat4 out = glm::eulerAngleYXZ(this->m_rotation.y, this->m_rotation.x, this->m_rotation.z);

	this->m_rotationMatrix = out;
}

void Knee::GeneralObject::updateScaleMatrix(){
	this->m_scaleMatrix = glm::scale(glm::mat4(1), this->m_scale);
}

void Knee::GeneralObject::applyRotationMatrix(glm::mat4 rotationMatrix){
	// apply rotation
	glm::mat4 newRotationMatrix = rotationMatrix * this->getRotationMatrix();
	
	// derive euler angles
	glm::extractEulerAngleYXZ(newRotationMatrix, this->m_rotation.y, this->m_rotation.x, this->m_rotation.z);

	// update rotation matrix
	this->updateRotationMatrix();
}

// local axes //

void Knee::GeneralObject::getLocalAxes(glm::vec3& x, glm::vec3& y, glm::vec3& z){
	z = this->getLocalZAxis();
	y = this->getLocalYAxis();
	x = glm::cross(z, y);
}

glm::vec3 Knee::GeneralObject::getLocalZAxis(){
	// apply rotation transformation to normalized vector
	glm::vec4 un = this->getRotationMatrix() * glm::vec4(0, 0, 1, 1);
	
	// return just xyz (w component doesn't matter)
	return glm::vec3(un);
}

glm::vec3 Knee::GeneralObject::getLocalYAxis(){
	// apply rotation transformation to normalized vector
	glm::vec4 un = this->getRotationMatrix() * glm::vec4(0, 1, 0, 1);
	
	// return just xyz (w component doesn't matter)
	return glm::vec3(un);
}

glm::vec3 Knee::GeneralObject::getLocalXAxis(){
	// apply rotation transformation to normalized vector
	glm::vec4 un = this->getRotationMatrix() * glm::vec4(1, 0, 0, 1);
	
	// return just xyz (w component doesn't matter)
	return glm::vec3(un);
}

glm::vec3 Knee::GeneralObject::getForwardVector(){
	return this->getLocalZAxis();
}

glm::vec3 Knee::GeneralObject::getUpVector(){
	return this->getLocalYAxis();
}

glm::vec3 Knee::GeneralObject::getCrossVector(){
	return this->getLocalXAxis();
}

// model matrix //

glm::mat4 Knee::GeneralObject::getModelMatrix(){
	return this->m_modelMatrix;
}

void Knee::GeneralObject::updateModelMatrix(){
	// update through precalculated transformation matrices
	this->m_modelMatrix = this->getTranslationMatrix() * this->getRotationMatrix()  * this->getScaleMatrix();
}

// -------------------- //
// MathUtils //

bool Knee::MathUtils::isLineSegmentIntersectingPlane(const glm::vec3& start, const glm::vec3& end, 
	const glm::vec3& planeCenter,
	const glm::mat4& planeRotationMatrix,
	const glm::vec3& planeSize) {
	
	// glm only has "intersectLineTriangle", so we split the plane into two unique triangles

	// calculate all vertices
	glm::vec3 tl = -planeSize/2.f; // top left
	glm::vec3 tr = tl + glm::vec3(planeSize.x, 0, 0); // top right
	glm::vec3 br = planeSize/2.f; // bottom right
	glm::vec3 bl = br - glm::vec3(planeSize.x, 0, 0); // bottom left

	// calculate rotation matrix
	glm::mat4 rotationMatrix = planeRotationMatrix;

	// vertices
	glm::vec3 vertices[] = {tl, tr, bl, br};

	// apply rotation matrix to each vertex
	for(uint32_t i = 0; i < 4; i++){
		vertices[i] = glm::vec3(rotationMatrix * glm::vec4(vertices[i], 0));
		vertices[i] += planeCenter;
	}

	// calculate triangle intersections
	for(uint32_t i = 0; i < 2; i++){
		glm::vec2 intersectionPoint;
		float distance;

		bool intersection = glm::intersectRayTriangle(start, glm::normalize(end - start), vertices[i], vertices[i+1], vertices[i+2], intersectionPoint, distance);

		// if intersection, return early
		if(intersection){
			// check if intersection lies on segment
			if(distance > 0 && distance*distance < glm::length2(end - start)){
				return true; 
			}
		}
	}

	// no intersection, return false
	return false;
}

bool Knee::MathUtils::approximatelyEqual(double v1, double v2, double threshold){
	return std::abs(v1-v2) <= threshold;
}