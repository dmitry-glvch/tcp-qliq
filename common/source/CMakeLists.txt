add_library(net net/net.cpp)

find_package(Boost 1.47.0 REQUIRED)
find_package(Threads REQUIRED)

target_include_directories(net PUBLIC ../include)

target_link_libraries(net PRIVATE Threads::Threads)