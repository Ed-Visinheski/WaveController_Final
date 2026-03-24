target_sources(WaveControllerFinal PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/inc/AudioGenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Buffer.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/AudioGenerator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Buffer.cpp    
)

target_include_directories(WaveControllerFinal PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)
