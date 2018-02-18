cmake_minimum_required(VERSION 3.1.0)

set(PROJECT_NAME pcre)
set(BASE_PATH ../../../../../OpenCOLLADA)
set(PROJECT_PATH ${BASE_PATH}/Externals/pcre)

project(${PROJECT_NAME})

# pcre
include_directories(${PROJECT_PATH}/include)
file(GLOB HEADERS "${PROJECT_PATH}/include/*.h")
file(GLOB SOURCES "${PROJECT_PATH}/src/*.c")

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
