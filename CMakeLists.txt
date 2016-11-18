cmake_minimum_required (VERSION 3.1.0)

#####################################
# cmake -Dtest=ON to build with tests
#####################################
option(test "Build all tests." OFF)

#####################################
# Main Libraries and  Executables
#####################################
set(PROJECT_NAME COLLADA2GLTF)
project(${PROJECT_NAME})
add_compile_options(-std=c++11)

include_directories(include)
include_directories(GLTF/include)
include_directories(dependencies/OpenCOLLADA/COLLADAFramework/include)
include_directories(dependencies/OpenCOLLADA/COLLADABaseUtils/include)

add_subdirectory(dependencies/OpenCOLLADA)
add_subdirectory(GLTF)

file(GLOB SOURCES "src/*.cpp")
add_library("${PROJECT_NAME}_lib" ${SOURCES})
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib GLTF OpenCOLLADAFramework_static)

#####################################
# Testing
#####################################
if (test)
  add_subdirectory(dependencies/googletest)
  enable_testing()

  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

  ####################################
  # Unit Tests
  ####################################
  add_executable(runUnitTests test/src/test.cpp)
  target_link_libraries(runUnitTests gtest gtest_main)
  target_link_libraries(runUnitTests "${PROJECT_NAME}_lib")

  add_test(GLTFAccessorTest runUnitTests)
  add_test(COLLADA2GLTFWriterTest runUnitTests)
endif()
