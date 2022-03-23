add_executable (daemon main.cpp)
add_library (daemonize STATIC source/daemonize.cpp)
add_library (routines STATIC source/routines.cpp)

find_package (Boost 1.78.0 REQUIRED)
find_package (Threads REQUIRED)

target_include_directories (daemon PRIVATE include)
target_include_directories (daemon PRIVATE ${Boost_INCLUDE_DIRS} )
target_include_directories (daemonize PRIVATE include)
target_include_directories (routines PRIVATE include)

target_link_libraries (daemon PRIVATE daemonize)
target_link_libraries (daemon PRIVATE routines)
target_link_libraries (daemon PRIVATE Threads::Threads)
target_link_libraries (routines PRIVATE Threads::Threads)
target_link_libraries (routines PRIVATE net_routines)
