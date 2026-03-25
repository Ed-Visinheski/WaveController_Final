target_sources(WaveControllerFinal PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/inc/WaveController.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/WaveController.cpp
)

target_include_directories(WaveControllerFinal PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)
