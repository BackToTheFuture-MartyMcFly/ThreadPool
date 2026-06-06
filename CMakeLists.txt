cmake_minimum_required(VERSION 3.25)

project(ThreadPool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_SUPPRESS_REGENERATION true)

set(SOURCE_FILES ThreadPool.cpp main.cpp)
set(HEADER_FILES ThreadPool.h)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

