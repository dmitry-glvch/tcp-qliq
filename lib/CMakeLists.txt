find_package(Boost 1.78.0 REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(range-v3 REQUIRED)


add_library(net_routines source/net_routines/net_routines.cpp)

target_include_directories(net_routines PUBLIC include/net_routines)
target_link_libraries(net_routines PRIVATE Threads::Threads)

target_include_directories (net_routines PRIVATE ${OpenSSL_INCLUDE_DIRS})
target_link_libraries (net_routines PRIVATE ${OpenSSL_LIBRARIES})

target_include_directories (net_routines PRIVATE ${range-v3_INCLUDE_DIRS})
target_link_libraries (net_routines PRIVATE ${range-v3_LIBRARIES})