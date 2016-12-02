cmake_minimum_required (VERSION 3.1.0)

set(PROJECT_NAME COLLADA2GLTF)

project(${PROJECT_NAME})
if(MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-std=c++11)
endif()

# cmake -Dtest=ON to build with tests
option(test "Build all tests." OFF)

# COLLADABaseUtils/Framework
include_directories(dependencies/OpenCOLLADA/OpenCOLLADA/COLLADABaseUtils/include)
include_directories(dependencies/OpenCOLLADA/OpenCOLLADA/COLLADAFramework/include)
add_subdirectory(dependencies/OpenCOLLADA/modules/COLLADAFramework)

# GLTF
include_directories(GLTF/include)
add_subdirectory(GLTF)

# COLLADA2GLTF_lib
include_directories(include)
file(GLOB LIB_HEADERS "include/*.h")
set(LIB_SOURCES src/COLLADA2GLTFWriter.cpp)
add_library(${PROJECT_NAME}_lib ${LIB_HEADERS} ${LIB_SOURCES})
target_link_libraries(${PROJECT_NAME}_lib GLTF COLLADAFramework)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)

if(test)
  enable_testing()

  # gtest
  set(GTEST_LINKED 1)
  include_directories(GLTF/dependencies/googletest/googletest/include)
  add_subdirectory(GLTF/dependencies/googletest/googletest)

  # Unit Tests
  include_directories(test/include)
  file(GLOB TEST_HEADERS "test/include/*.h")
  file(GLOB TEST_SOURCES "test/src/*.cpp")

  add_executable(${PROJECT_NAME}_TEST ${TEST_HEADERS} ${TEST_SOURCES})
  target_link_libraries(${PROJECT_NAME}_TEST  ${PROJECT_NAME}_lib GLTF gtest)

  add_test(COLLADA2GLTFWriterTest ${PROJECT_NAME}_TEST)
endif()
