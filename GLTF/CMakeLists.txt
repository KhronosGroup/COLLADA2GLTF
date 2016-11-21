cmake_minimum_required(VERSION 3.1.0)

set(PROJECT_NAME GLTF)

project(GLTF)
add_compile_options(-std=c++11)

# GLTF
include_directories(include)
file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.cpp")

add_library(GLTF ${HEADERS} ${SOURCES})
