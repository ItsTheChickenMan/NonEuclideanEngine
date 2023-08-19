#include <NonEuclideanEngine/fileio.hpp>

// reads contents of file into buffer.
// buffer does not need to be initialized
// returns 0 upon success and -1 upon error (call SDL_GetError() for more info)
int32_t Knee::readFileToCharBuffer(const char* file, char** buffer){
	SDL_RWops *io = SDL_RWFromFile(file, "rb");
	
	if(io != NULL){
		// get size
		int64_t size = SDL_RWsize(io);
		
		// create buffer of proper size
		*buffer = new char[size];
		
		// read to buffer
		if(SDL_RWread(io, *buffer, size, 1) == 0){
			return -1;
		} else {
			return 0;
		}
	}
	
	return -1;
}