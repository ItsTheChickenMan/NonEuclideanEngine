#include <NonEuclideanEngine/misc.hpp>

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>

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

Knee::GeneralObject& Knee::GeneralObject::operator*=(const GeneralObject& rhs){
	return *this = rhs * (*this);
}

void Knee::GeneralObject::updateCachedValuesFromModelMatrix(){
	glm::quat rotation;
	glm::vec3 skew;
	glm::vec4 perspective;

	glm::decompose(this->m_modelMatrix, this->m_scale, rotation, this->m_position, skew, perspective);

	this->m_rotation = glm::eulerAngles(rotation);

	// update matrices
	this->updateTranslationMatrix();
	this->updateRotationMatrix();
	this->updateScaleMatrix();
	this->updateModelMatrix();
}

void Knee::GeneralObject::applyTransformation(GeneralObject t){
	// multiply model matrices
	this->applyMatrixTransformation(t.getModelMatrix());

	// update values from model matrix
	this->updateCachedValuesFromModelMatrix();
}

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

Knee::GeneralObject Knee::GeneralObject::getInverseGeneralObject(){
	Knee::GeneralObject obj;

	glm::mat4 inverseModel = glm::inverse(this->getModelMatrix());

	obj.applyMatrixTransformation(inverseModel);

	obj.updateCachedValuesFromModelMatrix();

	return obj;
}

/*Knee::GeneralObject Knee::GeneralObject::getInverseGeneralObject(){
	Knee::GeneralObject obj;

	obj.setPosition(-this->getPosition());
	obj.setRotation(-this->getRotation());
	obj.setScale(1.0f / this->getScale());

	return obj;
}*/

void Knee::GeneralObject::addGeneralObject(GeneralObject obj){
	this->changePosition(obj.getPosition());
	this->changeRotation(obj.getRotation());
	this->changeScale(obj.getScale());
}

void Knee::GeneralObject::subtractGeneralObject(GeneralObject obj){
	this->addGeneralObject(obj.getInverseGeneralObject());
}

glm::mat4 Knee::GeneralObject::getTranslationMatrix(){
	return this->m_translationMatrix;
}

glm::mat4 Knee::GeneralObject::getRotationMatrix(){
	return this->m_rotationMatrix;
}

glm::mat4 Knee::GeneralObject::getScaleMatrix(){
	return this->m_scaleMatrix;
}

glm::vec3 Knee::GeneralObject::getForwardVector(){
	// apply rotation transformation to normalized vector
	glm::vec4 un = this->getRotationMatrix() * glm::vec4(0, 0, 1, 1);
	
	// return just xyz (w component doesn't matter)
	return glm::vec3(un);
}

glm::vec3 Knee::GeneralObject::getUpVector(){
	// apply rotation transformation to normalized vector
	glm::vec4 un = this->getRotationMatrix() * glm::vec4(0, 1, 0, 1);
	
	// return just xyz (w component doesn't matter)
	return glm::vec3(un);
}

void Knee::GeneralObject::updateTranslationMatrix(){
	this->m_translationMatrix = glm::translate(glm::mat4(1), this->m_position);
}

void Knee::GeneralObject::updateRotationMatrix(){
	glm::mat4 out = glm::eulerAngleZYX(this->m_rotation.z, this->m_rotation.y, this->m_rotation.x);
	
	this->m_rotationMatrix = out;
}

void Knee::GeneralObject::updateScaleMatrix(){
	this->m_scaleMatrix = glm::scale(glm::mat4(1), this->m_scale);
}

void Knee::GeneralObject::updateModelMatrix(){
	// update with all the matrices
	this->m_modelMatrix = this->getTranslationMatrix() * this->getRotationMatrix() * this->getScaleMatrix();
}

glm::mat4 Knee::GeneralObject::getModelMatrix(){
	return this->m_modelMatrix;
}

void Knee::GeneralObject::applyMatrixTransformation(glm::mat4 model){
	this->m_modelMatrix = model * this->m_modelMatrix;
}