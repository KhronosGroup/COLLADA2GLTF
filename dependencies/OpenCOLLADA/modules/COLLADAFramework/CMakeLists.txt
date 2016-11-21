cmake_minimum_required(VERSION 3.1.0)

set(PROJECT_NAME COLLADAFramework)
set(BASE_PATH ../../OpenCOLLADA)
set(PROJECT_PATH ${BASE_PATH}/COLLADAFramework)

project(${PROJECT_NAME})

# COLLADAFramework
include_directories(${PROJECT_PATH}/include)
file(GLOB HEADERS "${PROJECT_PATH}/include/*.h")
file(GLOB SOURCES "${PROJECT_PATH}/src/*.cpp")

# COLLADABaseUtils
include_directories(${BASE_PATH}/COLLADABaseUtils/include)
add_subdirectory(COLLADABaseUtils)

# MathML
include_directories(${BASE_PATH}/Externals/MathMLSolver/include)
file(GLOB MATHML_HEADERS "${BASE_PATH}/Externals/MathMLSolver/include/*.h")
file(GLOB MATHML_SOURCES "${BASE_PATH}/Externals/MathMLSolver/*.cpp")

# MathML AST
include_directories(${BASE_PATH}/Externals/MathMLSolver/include/AST)
file(GLOB MATHMLAST_HEADERS "${BASE_PATH}/Externals/MathMLSolver/include/AST/*.h")
file(GLOB MATHMLAST_SOURCES "${BASE_PATH}/Externals/MathMLSolver/src/AST/*.cpp")

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${MATHML_HEADERS} ${MATHML_SOURCES} ${MATHMLAST_HEADERS} ${MATHMLAST_SOURCES})
target_link_libraries(${PROJECT_NAME} COLLADABaseUtils)
