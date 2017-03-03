cmake_minimum_required(VERSION 3.1.0)

set(PROJECT_NAME expat)
set(BASE_PATH ../../../../OpenCOLLADA)
set(PROJECT_PATH ${BASE_PATH}/Externals/expat)

project(${PROJECT_NAME})
add_definitions(-DHAVE_MEMMOVE)

# GeneratedSaxParser
include_directories(${PROJECT_PATH}/lib)
file(GLOB HEADERS "${PROJECT_PATH}/lib/*.h")
file(GLOB SOURCES "${PROJECT_PATH}/lib/*.c")

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
