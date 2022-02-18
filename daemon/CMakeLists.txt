add_executable(daemon main.cpp)

target_link_libraries(daemon PRIVATE easy_shutdown)