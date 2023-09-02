#include <NonEuclideanEngine/misc.hpp>

#include <glm/ext.hpp>

// -------------------- //
// DeltaTimer //

Knee::DeltaTimer::DeltaTimer(){
	this->resetDelta();
}

void Knee::DeltaTimer::resetDelta(){
	this->m_timeOfLastReset = this->m_deltaTimer.now();
}

double Knee::DeltaTimer::getTime(){
	std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> now = this->m_deltaTimer.now();
	
	return now.time_since_epoch().count();
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

glm::vec3 Knee::GeneralObject::getPosition(){ return this->m_position; }
glm::vec3 Knee::GeneralObject::getRotation(){ return this->m_rotation; }
glm::vec3 Knee::GeneralObject::getScale(){ return this->m_scale; }

void Knee::GeneralObject::setPosition(glm::vec3 position){
	this->m_position = position;
	
	this->updateModelMatrix();
}

void Knee::GeneralObject::setRotation(glm::vec3 rotation){
	this->m_rotation = rotation;
	
	this->updateModelMatrix();
}

void Knee::GeneralObject::setScale(glm::vec3 scale){
	this->m_scale = scale;
	
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
	
	this->setScale(old + change);
}

void Knee::GeneralObject::updateModelMatrix(){
	// reset to identity
	this->m_modelMatrix = glm::mat4(1);
	
	// translate
	this->m_modelMatrix = glm::translate(this->m_modelMatrix, this->m_position);
	
	// rotate
	this->m_modelMatrix = glm::rotate(this->m_modelMatrix, this->m_rotation.z, glm::vec3(0, 0, 1));
	this->m_modelMatrix = glm::rotate(this->m_modelMatrix, this->m_rotation.y, glm::vec3(0, 1, 0));
	this->m_modelMatrix = glm::rotate(this->m_modelMatrix, this->m_rotation.x, glm::vec3(1, 0, 0));
	
	
	// scale
	this->m_modelMatrix = glm::scale(this->m_modelMatrix, this->m_scale);
}

glm::mat4 Knee::GeneralObject::getModelMatrix(){
	return this->m_modelMatrix;
}