cmake_minimum_required(VERSION 3.1.0)

set(PROJECT_NAME COLLADAFramework)
set(BASE_PATH ../../../OpenCOLLADA)
set(PROJECT_PATH ${BASE_PATH}/COLLADAFramework)

project(${PROJECT_NAME})

# COLLADAFramework
include_directories(${PROJECT_PATH}/include)
file(GLOB HEADERS "${PROJECT_PATH}/include/*.h")
file(GLOB SOURCES "${PROJECT_PATH}/src/*.cpp")

# COLLADABaseUtils
include_directories(${BASE_PATH}/COLLADABaseUtils/include)
add_subdirectory(COLLADABaseUtils)

# MathMLSolver
include_directories(${BASE_PATH}/Externals/MathMLSolver/include)
include_directories(${BASE_PATH}/Externals/MathMLSolver/include/AST)
add_subdirectory(MathMLSolver)

# pcre
include_directories(${BASE_PATH}/Externals/pcre/include)

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} COLLADABaseUtils MathMLSolver)
