project(client)

add_executable(client main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC common)

find_package(Boost 1.47.0 REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS} )