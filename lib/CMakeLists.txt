find_package(Boost 1.47.0 REQUIRED)
find_package(Threads REQUIRED)


add_library(net_routines source/net_routines/net_routines.cpp)
target_include_directories(net_routines PUBLIC include/net_routines)
target_link_libraries(net_routines PRIVATE Threads::Threads)