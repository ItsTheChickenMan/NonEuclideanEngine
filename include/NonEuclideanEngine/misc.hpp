#pragma once

#include <string>
#include <chrono>

namespace Knee {
	const std::string ERROR_PREFACE = "Error: ";
	
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
	};
}