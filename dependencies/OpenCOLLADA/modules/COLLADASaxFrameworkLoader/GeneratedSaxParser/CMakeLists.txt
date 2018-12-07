cmake_minimum_required(VERSION 3.1.0)

set(PROJECT_NAME GeneratedSaxParser)
set(BASE_PATH ../../../OpenCOLLADA)
set(PROJECT_PATH ${BASE_PATH}/GeneratedSaxParser)

project(${PROJECT_NAME})

# GeneratedSaxParser
include_directories(${PROJECT_PATH}/include)
file(GLOB HEADERS "${PROJECT_PATH}/include/*.h")
file(GLOB SOURCES "${PROJECT_PATH}/src/*.cpp")

# expat
include_directories(${BASE_PATH}/Externals/expat/lib)
add_subdirectory(expat)

# LibXML
if(NOT LIBXML2_FOUND)
  include_directories(${BASE_PATH}/Externals/LibXML/include)
  add_subdirectory(LibXML)
else()
  include_directories(${LIBXML2_INCLUDE_DIR})
endif()

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} expat LibXML)
