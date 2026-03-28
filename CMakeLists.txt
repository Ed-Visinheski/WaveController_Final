cmake_minimum_required(VERSION 3.20)
project(WaveControllerFinal)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Multimedia Network QUIET)
if(Qt6_FOUND)
	message(STATUS "Found Qt6")
	set(QT_CORE Qt6::Core)
	set(QT_GUI Qt6::Gui)
	set(QT_WIDGETS Qt6::Widgets)
	set(QT_MULTIMEDIA Qt6::Multimedia)
	set(QT_NETWORK Qt6::Network)
else()
	find_package(Qt5 COMPONENTS Core Gui Widgets Multimedia Network QUIET)
	if(Qt5_FOUND)
		message(STATUS "Found Qt5")
		set(QT_CORE Qt5::Core)
		set(QT_GUI Qt5::Gui)
		set(QT_WIDGETS Qt5::Widgets)
		set(QT_MULTIMEDIA Qt5::Multimedia)
		set(QT_NETWORK Qt5::Network)
	endif()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(WaveControllerFinal main.cpp)

add_subdirectory(wave)

# Link Qt libraries if available
if(Qt6_FOUND OR Qt5_FOUND)
	target_link_libraries(WaveControllerFinal PRIVATE 
		${QT_CORE} 
		${QT_GUI} 
		${QT_WIDGETS}
		${QT_MULTIMEDIA}
		${QT_NETWORK}
	)
endif() 