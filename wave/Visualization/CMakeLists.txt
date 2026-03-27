target_sources(WaveControllerFinal PRIVATE
	${CMAKE_SOURCE_DIR}/external/kissfft/kiss_fft.c
	${CMAKE_CURRENT_SOURCE_DIR}/inc/KissFFTAnalyzer.h
	${CMAKE_CURRENT_SOURCE_DIR}/inc/SpectrumAnalyzer.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/KissFFTAnalyzer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SpectrumAnalyzer.cpp
)

target_include_directories(WaveControllerFinal PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/inc
	${CMAKE_SOURCE_DIR}/external/kissfft
)
