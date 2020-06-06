cmake_minimum_required(VERSION 3.17.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(neuron)

include_directories(include)

set(SOURCES
	"src/main.cpp"
	"src/Neuron/Neuron.cpp"
	"src/ThreadPool/ThreadPool.cpp"
	"src/World/World3D.cpp"
)

set(HEADERS
	"include/Neuron.h"
	"include/ThreadPool.h"
	"include/World3D.h"
)

set(DEBUG_FILES
	"include/debug_tool/dNeuron.h"
)

source_group("Header Files" ${HEADERS})
source_group("Source Files" ${SOURCES})
source_group("Debugger Files" ${DEBUG_FILES})

add_executable(neuron ${SOURCES} ${HEADERS} ${DEBUG_FILES})
