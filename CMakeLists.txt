cmake_minimum_required (VERSION 3.1.0)

set(PROJECT_NAME COLLADA2GLTF)

project(${PROJECT_NAME})
add_compile_options(-std=c++11)

# cmake -Dtest=ON to build with tests
option(test "Build all tests." OFF)

# COLLADA2GLTF_lib
include_directories(include)
file(GLOB LIB_HEADERS "include/*.h")
set(LIB_SOURCES src/COLLADA2GLTFWriter.cpp)
add_library(${PROJECT_NAME}_lib ${LIB_HEADERS} ${LIB_SOURCES})

# GLTF
include_directories(GLTF/include)
add_subdirectory(GLTF)

# COLLADABaseUtils/Framework
include_directories(dependencies/OpenCOLLADA/OpenCOLLADA/COLLADABaseUtils/include)
include_directories(dependencies/OpenCOLLADA/OpenCOLLADA/COLLADAFramework/include)
add_subdirectory(dependencies/OpenCOLLADA/modules/COLLADAFramework)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib GLTF COLLADAFramework)

if (test)
  enable_testing()

  # gtest
  include_directories(dependencies/googletest/googletest/include)
  add_subdirectory(dependencies/googletest/googletest)

  # Unit Tests
  include_directories(test/include)
  file(GLOB TEST_HEADERS "test/include/*.h")
  file(GLOB TEST_SOURCES "test/src/*.cpp")

  add_executable(runUnitTests ${TEST_HEADERS} ${TEST_SOURCES})
  target_link_libraries(runUnitTests  ${PROJECT_NAME}_lib gtest)

  add_test(GLTFAccessorTest runUnitTests)
  add_test(COLLADA2GLTFWriterTest runUnitTests)
endif()
