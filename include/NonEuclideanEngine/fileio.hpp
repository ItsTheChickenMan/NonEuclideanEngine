#pragma once

#include <cstdint>
#include <SDL2/SDL.h>

namespace Knee {
	int32_t readFileToCharBuffer(const char* file, char** buffer);
}