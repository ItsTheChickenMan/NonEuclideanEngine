#include <NonEuclideanEngine/misc.hpp>

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