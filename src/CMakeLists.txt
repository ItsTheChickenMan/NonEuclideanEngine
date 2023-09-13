add_subdirectory(NonEuclideanEngine)

add_executable(EngineTest main.cpp)

target_link_libraries(EngineTest PUBLIC NonEuclideanEngine)
target_link_libraries(EngineTest PUBLIC )

target_include_directories(EngineTest PUBLIC
							"${PROJECT_SOURCE_DIR}/include"
							)

install(TARGETS EngineTest
	RUNTIME
		DESTINATION ${CMAKE_BINARY_DIR}/bin
)