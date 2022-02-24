cmake_minimum_required(VERSION 3.14)

project(imaqliq LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
  -g3 \
  -Wall \
  -Wextra \
  -pedantic \
  -Weffc++ \
  -Winit-self \
  -Wredundant-decls \
  -Wcast-align \
  -Wundef \
  -Wfloat-equal \
  -Winline \
  -Wunreachable-code \
  -Wmissing-declarations \
  -Wmissing-include-dirs \
  -Wswitch-enum \
  -Wfatal-errors \
  -Wconversion \
  -Wcast-qual \
  -Wduplicated-cond \
  -Wduplicated-branches \
  -Wsuggest-override \
  -Wuninitialized \
  -Wnull-dereference \
")

add_subdirectory(client)
add_subdirectory(daemon)
add_subdirectory(common)