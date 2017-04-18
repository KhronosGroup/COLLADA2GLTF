cmake_minimum_required(VERSION 3.1.0)

set(PROJECT_NAME GLTF)

project(${PROJECT_NAME})
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-std=c++11 -lstdc++fs)
endif()

# cmake -Dtest=ON to build with tests
option(test "Build all tests." OFF)

# RapidJSON
include_directories(dependencies/rapidjson/include)

# gltf
include_directories(include)
file(GLOB HEADERS "include/*.h")
file(GLOB SOURCES "src/*.cpp")

add_library(GLTF ${HEADERS} ${SOURCES})
if(MSVC)
   target_link_libraries(${PROJECT_NAME})
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
   target_link_libraries(${PROJECT_NAME} stdc++fs)
endif()

if (test)
  enable_testing()

  # gtest
  if(!GTEST_LINKED)
    include_directories(dependencies/googletest/googletest/include)
    add_subdirectory(dependencies/googletest/googletest)
  endif()

  # Unit Tests
  include_directories(test/include)
  file(GLOB TEST_HEADERS "test/include/*.h")
  file(GLOB TEST_SOURCES "test/src/*.cpp")

  add_executable(${PROJECT_NAME}-test ${TEST_HEADERS} ${TEST_SOURCES})
  target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME} gtest)

  add_test(GLTFAccessorTest ${PROJECT_NAME}-test)
  add_test(GLTFObjectTest ${PROJECT_NAME}-test)
endif()
