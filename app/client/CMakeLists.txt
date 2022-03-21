add_executable(client main.cpp)

find_package(Boost 1.78.0 REQUIRED)
find_package(Threads REQUIRED)

target_include_directories(client PRIVATE ${Boost_INCLUDE_DIRS} )

target_link_libraries(client PRIVATE net_routines)
target_link_libraries(client PRIVATE Threads::Threads)
