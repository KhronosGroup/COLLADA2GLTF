cmake_minimum_required(VERSION 3.1.0)

set(PROJECT_NAME argparse)
set(BASE_PATH ../..)
set(PROJECT_PATH ${BASE_PATH}/argparse)

project(${PROJECT_NAME})

# argparse
include_directories(${PROJECT_PATH})
set(HEADERS "${PROJECT_PATH}/argparse.h")
set(SOURCES "${PROJECT_PATH}/argparse.c")

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
