project(daemon)

add_executable(daemon main.cpp)

add_library(deamonize source/deamonize.cpp)
target_include_directories(deamonize PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PUBLIC common)
target_link_libraries(${PROJECT_NAME} PRIVATE deamonize)

find_package(Boost 1.47.0 REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS} )