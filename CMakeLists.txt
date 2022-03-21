cmake_minimum_required(VERSION 3.13)

project(imaqliq LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fcoroutines")


set (CONAN_DIR "${CMAKE_SOURCE_DIR}/conan/")
list(APPEND CMAKE_MODULE_PATH ${CONAN_DIR})


add_subdirectory(lib)
add_subdirectory(app/client)
add_subdirectory(app/daemon)