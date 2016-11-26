cmake_minimum_required(VERSION 3.1.0)

set(PROJECT_NAME GLTF)

project(${PROJECT_NAME})
add_compile_options(-std=c++11)

# cmake -DGLTF_TEST=ON to build with tests
option(GLTF_TEST "Build all tests." OFF)

# RapidJSON
include_directories(dependencies/rapidjson/include)

# GLTF
include_directories(include)
file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.cpp")

add_library(GLTF ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME})

if (GLTF_TEST)
  enable_testing()

  # gtest
  include_directories(dependencies/googletest/googletest/include)
  add_subdirectory(dependencies/googletest/googletest)

  # Unit Tests
  include_directories(test/include)
  file(GLOB TEST_HEADERS "test/include/*.h")
  file(GLOB TEST_SOURCES "test/src/*.cpp")

  add_executable(runUnitTests ${TEST_HEADERS} ${TEST_SOURCES})
  target_link_libraries(runUnitTests ${PROJECT_NAME} gtest)

  add_test(GLTFAccessorTest runUnitTests)
  add_test(GLTFObjectTest runUnitTests)
endif()
