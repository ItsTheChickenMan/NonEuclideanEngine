add_library(NonEuclideanEngine
	application.cpp
	game.cpp
	shader.cpp
	misc.cpp
	fileio.cpp
	glad/glad.c
)

add_library(SDL2 SHARED IMPORTED)
add_library(SDL2main STATIC IMPORTED)

set_target_properties(SDL2 PROPERTIES
	IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/SDL2/SDL2.dll
	IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/lib/SDL2/libSDL2.dll.a
)

set_target_properties(SDL2main PROPERTIES
	IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/SDL2/libSDL2main.a
)

target_link_libraries(NonEuclideanEngine PUBLIC opengl32)
target_link_libraries(NonEuclideanEngine PUBLIC SDL2)
target_link_libraries(NonEuclideanEngine PUBLIC SDL2main)
target_link_libraries(NonEuclideanEngine PUBLIC mingw32)

target_include_directories(NonEuclideanEngine PUBLIC
							"${PROJECT_SOURCE_DIR}/include"
)

# install rules

# resource files
install(DIRECTORY 
	${CMAKE_SOURCE_DIR}/res/
		DESTINATION ${CMAKE_BINARY_DIR}/bin
)

# SDL2
install(IMPORTED_RUNTIME_ARTIFACTS SDL2
	RUNTIME
		DESTINATION ${CMAKE_BINARY_DIR}/bin
)

# headers
install(DIRECTORY 
	${CMAKE_SOURCE_DIR}/include/
		DESTINATION ${CMAKE_BINARY_DIR}/include
)

# static dll import
install(TARGETS NonEuclideanEngine
	ARCHIVE
		DESTINATION ${CMAKE_BINARY_DIR}/lib
)

# dll
install(TARGETS NonEuclideanEngine
	RUNTIME
		DESTINATION ${CMAKE_BINARY_DIR}/bin
)