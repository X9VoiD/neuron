cmake_minimum_required(VERSION 3.17.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(neuron)

include_directories(include)

set(SOURCES
	"src/main.cpp"
	"src/Neuron/Neuron.cpp"
	"src/ThreadPool/ThreadPool.cpp"
)

set(HEADERS
	"include/Neuron.h"
	"include/ThreadPool.h"
)

source_group("Header Files" ${HEADERS})
source_group("Source Files" ${SOURCES})

add_executable(neuron ${SOURCES} ${HEADERS})