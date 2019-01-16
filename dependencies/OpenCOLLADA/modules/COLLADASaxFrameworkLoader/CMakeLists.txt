cmake_minimum_required(VERSION 3.1.0)

set(PROJECT_NAME COLLADASaxFrameworkLoader)
set(BASE_PATH ../../OpenCOLLADA)
set(PROJECT_PATH ${BASE_PATH}/COLLADASaxFrameworkLoader)

project(${PROJECT_NAME})

# COLLADASaxFrameworkLoader
include_directories(${PROJECT_PATH}/include)
file(GLOB HEADERS "${PROJECT_PATH}/include/*.h")
file(GLOB SOURCES "${PROJECT_PATH}/src/*.cpp")

# COLLADASaxFrameworkLoader Generated14
include_directories(${PROJECT_PATH}/include/generated14)
file(GLOB HEADERS_GENERATED14 "${PROJECT_PATH}/include/generated14/*.h")
file(GLOB SOURCES_GENERATED14 "${PROJECT_PATH}/src/generated14/*.cpp")

# COLLADASaxFrameworkLoader Generated15
include_directories(${PROJECT_PATH}/include/generated15)
file(GLOB HEADERS_GENERATED15 "${PROJECT_PATH}/include/generated15/*.h")
file(GLOB SOURCES_GENERATED15 "${PROJECT_PATH}/src/generated15/*.cpp")

# COLLADAFramework
include_directories(${BASE_PATH}/COLLADAFramework/include)
add_subdirectory(COLLADAFramework)

# COLLADABaseUtils
include_directories(${BASE_PATH}/COLLADABaseUtils/include)

# MathMLSolver
include_directories(${BASE_PATH}/Externals/MathMLSolver/include)

# MathMLSolver AST
include_directories(${BASE_PATH}/Externals/MathMLSolver/include/AST)

# GeneratedSaxParser
# Use LibXML by default
add_definitions(-DGENERATEDSAXPARSER_XMLPARSER_LIBXML)
include_directories(${BASE_PATH}/GeneratedSaxParser/include)
add_subdirectory(GeneratedSaxParser)

# LibXML
if(NOT LIBXML2_FOUND)
  include_directories(${BASE_PATH}/Externals/LibXML/include)
else()
  include_directories(${LIBXML2_INCLUDE_DIR})
endif()

# expat
include_directories(${BASE_PATH}/Externals/expat/lib)

# pcre
if(NOT PCRE_FOUND)
  add_definitions(-DPCRE_STATIC)
  include_directories(${BASE_PATH}/Externals/pcre/include)
else()
  include_directories(${PCRE_INCLUDE_DIR})
endif()

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${HEADERS_GENERATED14} ${SOURCES_GENERATED14} ${HEADERS_GENERATED15} ${SOURCES_GENERATED15})
target_link_libraries(${PROJECT_NAME} COLLADAFramework GeneratedSaxParser)
