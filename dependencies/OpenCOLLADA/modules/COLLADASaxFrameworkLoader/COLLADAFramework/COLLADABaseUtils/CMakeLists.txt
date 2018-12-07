cmake_minimum_required(VERSION 3.1.0)

set(PROJECT_NAME COLLADABaseUtils)
set(BASE_PATH ../../../../OpenCOLLADA)
set(PROJECT_PATH ${BASE_PATH}/COLLADABaseUtils)

project(${PROJECT_NAME})

# COLLADABaseUtils
include_directories(${PROJECT_PATH}/include)
file(GLOB HEADERS "${PROJECT_PATH}/include/*.h")
file(GLOB SOURCES "${PROJECT_PATH}/src/*.cpp")

# COLLADABaseUtils Math
include_directories(${PROJECT_PATH}/include/Math)
file(GLOB MATH_HEADERS "${PROJECT_PATH}/include/Math/*.h")
file(GLOB MATH_SOURCES "${PROJECT_PATH}/src/Math/*.cpp")

# UTF
include_directories(${BASE_PATH}/Externals/UTF/include)
file(GLOB UTF_HEADERS "${BASE_PATH}/Externals/UTF/include/*.h")
file(GLOB UTF_SOURCES "${BASE_PATH}/Externals/UTF/src/*.c")

# pcre
if(NOT PCRE_FOUND)
  add_definitions(-DPCRE_STATIC)
  include_directories(${BASE_PATH}/Externals/pcre/include)
  add_subdirectory(pcre)
else()
  include_directories(${PCRE_INCLUDE_DIR})
endif()

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${MATH_HEADERS} ${MATH_SOURCES} ${UTF_HEADERS} ${UTF_SOURCES})
target_link_libraries(${PROJECT_NAME} pcre)
