cmake_minimum_required (VERSION 3.1.0)

set(PROJECT_NAME COLLADA2GLTF)

project(${PROJECT_NAME})
if(MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-std=c++11 -lstdc++fs)
endif()

# cmake -Dtest=ON to build with tests
option(test "Build all tests." OFF)

# GLTF
include_directories(GLTF/include)
add_subdirectory(GLTF)

# Disable for other modules
set(TEST_ENABLED ${test})
set(test OFF)

# RapidJSON
include_directories(GLTF/dependencies/rapidjson/include)

# COLLADASaxFrameworkLoader/BaseUtils/Framework, GeneratedSaxParser
include_directories(dependencies/OpenCOLLADA/OpenCOLLADA/COLLADASaxFrameworkLoader/include)
include_directories(dependencies/OpenCOLLADA/OpenCOLLADA/COLLADABaseUtils/include)
include_directories(dependencies/OpenCOLLADA/OpenCOLLADA/COLLADAFramework/include)
include_directories(dependencies/OpenCOLLADA/OpenCOLLADA/GeneratedSaxParser/include)
add_subdirectory(dependencies/OpenCOLLADA/modules/COLLADASaxFrameworkLoader)

# COLLADA2GLTF
include_directories(include)
file(GLOB LIB_HEADERS "include/*.h")
set(LIB_SOURCES src/COLLADA2GLTFWriter.cpp src/COLLADA2GLTFExtrasHandler.cpp)
add_library(${PROJECT_NAME} ${LIB_HEADERS} ${LIB_SOURCES})
if(MSVC)
   target_link_libraries(${PROJECT_NAME} GLTF COLLADASaxFrameworkLoader)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
   target_link_libraries(${PROJECT_NAME} GLTF COLLADASaxFrameworkLoader stdc++fs)
endif()

# ahoy
include_directories(dependencies/ahoy/include)
add_subdirectory(dependencies/ahoy)

add_executable(${PROJECT_NAME}-bin src/main.cpp)
if(MSVC)
   target_link_libraries(${PROJECT_NAME}-bin ${PROJECT_NAME} ahoy)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
   target_link_libraries(${PROJECT_NAME}-bin ${PROJECT_NAME} ahoy stdc++fs)
endif()

if(TEST_ENABLED)
  enable_testing()

  # gtest
  set(GTEST_LINKED 1)
  include_directories(GLTF/dependencies/googletest/googletest/include)
  add_subdirectory(GLTF/dependencies/googletest/googletest)

  # Unit Tests
  include_directories(test/include)
  file(GLOB TEST_HEADERS "test/include/*.h")
  file(GLOB TEST_SOURCES "test/src/*.cpp")

  add_executable(${PROJECT_NAME}-test ${TEST_HEADERS} ${TEST_SOURCES})
  target_link_libraries(${PROJECT_NAME}-test  ${PROJECT_NAME} GLTF gtest)

  add_test(COLLADA2GLTFWriterTest ${PROJECT_NAME}-test)
endif()
