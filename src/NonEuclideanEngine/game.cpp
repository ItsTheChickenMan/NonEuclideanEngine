#include <NonEuclideanEngine/game.hpp>

#include <glm/ext/matrix_transform.hpp>

Knee::Game::Game(){
	
}

Knee::Game::~Game(){
	
}

Knee::GameObject::GameObject() : position(glm::vec3(0)), rotation(glm::vec3(0)), scale(glm::vec3(0)) {}

Knee::GameObject::GameObject(glm::vec3 p, glm::vec3 r, glm::vec3 s) : position(p), rotation(r), scale(s) {}

Knee::GameObject::~GameObject(){

}

// using the current values for position, rotation, and scale, recalculate the object's model matrix
void Knee::GameObject::calculateModelMatrix(){
	// clear old matrix
	this->modelMatrix = glm::mat4(1);
	
	// scale
	this->modelMatrix = glm::scale(this->modelMatrix, this->scale);
	
	// rotate (ZYX order)
	this->modelMatrix = glm::rotate(this->modelMatrix, this->rotation.z, glm::vec3(0, 0, 1));
	this->modelMatrix = glm::rotate(this->modelMatrix, this->rotation.y, glm::vec3(0, 1, 0));
	this->modelMatrix = glm::rotate(this->modelMatrix, this->rotation.x, glm::vec3(1, 0, 0));
	
	// translate
	this->modelMatrix = glm::translate(this->modelMatrix, this->position);
}