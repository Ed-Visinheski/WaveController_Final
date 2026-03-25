target_sources(WaveControllerFinal PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/HarmonicControlPanel.h
	${CMAKE_CURRENT_SOURCE_DIR}/inc/SynthVisualizationWindow.h
	${CMAKE_CURRENT_SOURCE_DIR}/inc/WaveForm.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/SynthVisualizationWindow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/WaveForm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HarmonicControlPanel.cpp
)

target_include_directories(WaveControllerFinal PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)
