target_sources(WaveControllerFinal PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/inc/Constants.h
	${CMAKE_CURRENT_SOURCE_DIR}/inc/Timer.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/Timer.cpp
)

target_include_directories(WaveControllerFinal PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)
