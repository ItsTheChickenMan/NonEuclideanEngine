#include <NonEuclideanEngine/shader.hpp>
#include <NonEuclideanEngine/gameobjects.hpp>

#include <iostream>

// StaticGameObject //

Knee::StaticGameObject::StaticGameObject(){
}

Knee::StaticGameObject::~StaticGameObject(){
}

Knee::GeneralObject* Knee::StaticGameObject::asGeneralObject(){
	return static_cast<Knee::GeneralObject*>(this);
}

// RenderableStaticGameObject //

Knee::RenderableStaticGameObject::RenderableStaticGameObject(Knee::VertexData* vertexData, Knee::Texture2D* texture) : RenderableObject(vertexData, texture, NULL) {
}

Knee::RenderableStaticGameObject::~RenderableStaticGameObject(){
}

Knee::RenderableObject* Knee::RenderableStaticGameObject::asRenderableObject(){
	return static_cast<RenderableObject*>(this);
}

Knee::StaticGameObject* Knee::RenderableStaticGameObject::asStaticGameObject(){
	return static_cast<StaticGameObject*>(this);
}

// -------------------- //
// GameObject //

Knee::GameObject::GameObject() : m_mass(1.0) {}
Knee::GameObject::GameObject(double mass) : m_mass(mass) {}

Knee::GameObject::~GameObject(){}

Knee::StaticGameObject* Knee::GameObject::asStaticGameObject(){
	return static_cast<Knee::StaticGameObject*>(this);
}

double Knee::GameObject::getMass(){
	return this->m_mass;
}

void Knee::GameObject::setMass(double mass){
	this->m_mass = mass;
}

glm::vec3 Knee::GameObject::getVelocity(){
	return this->m_velocity;
}

void Knee::GameObject::setVelocity(glm::vec3 newVelocity){
	this->m_velocity = newVelocity;
}

void Knee::GameObject::changeVelocity(glm::vec3 change){
	this->m_velocity += change;
}

void Knee::GameObject::impulse(glm::vec3 f){
	this->m_velocity += f / (float)this->m_mass;
}

void Knee::GameObject::update(double delta){
	// determine change in position
	glm::vec3 changeInPosition = this->m_velocity * (float)delta;
	
	// change position
	this->changePosition(changeInPosition);
}

// -------------------- //
// RenderableGameObject //

Knee::RenderableGameObject::RenderableGameObject(Knee::VertexData* vertexData, Knee::Texture2D* texture) : GameObject(), RenderableStaticGameObject(vertexData, texture){}

Knee::GameObject* Knee::RenderableGameObject::asGameObject(){
	return static_cast<Knee::GameObject*>(this);
}