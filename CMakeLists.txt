cmake_minimum_required(VERSION 3.22.1)
project(coroutine-test-2)
set(CMAKE_CXX_STANDARD 20)
add_subdirectory(lib)
add_subdirectory(main)
add_subdirectory(scheduler)
add_subdirectory(event-loop)