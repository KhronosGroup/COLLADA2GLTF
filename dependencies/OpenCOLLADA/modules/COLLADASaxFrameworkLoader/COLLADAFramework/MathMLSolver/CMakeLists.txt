cmake_minimum_required(VERSION 3.1.0)

set(PROJECT_NAME MathMLSolver)
set(BASE_PATH ../../../../OpenCOLLADA)
set(PROJECT_PATH ${BASE_PATH}/Externals/MathMLSolver)

project(${PROJECT_NAME})

# MathMLSolver
include_directories(${PROJECT_PATH}/include)
file(GLOB HEADERS "${PROJECT_PATH}/include/*.h")
file(GLOB SOURCES "${PROJECT_PATH}/src/*.cpp")

# MathMLSolver AST
include_directories(${PROJECT_PATH}/include/AST)
file(GLOB AST_HEADERS "${PROJECT_PATH}/include/AST/*.h")
file(GLOB AST_SOURCES "${PROJECT_PATH}/src/AST/*.cpp")

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${AST_HEADERS} ${AST_SOURCES})
