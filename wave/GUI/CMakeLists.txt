target_sources(WaveControllerFinal PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/inc/WaveForm.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/WaveForm.cpp
)

target_include_directories(WaveControllerFinal PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)
