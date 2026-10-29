cmake_minimum_required(VERSION 3.16)
project(custom_ai_from_scratch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_executable(custom_ai main.cpp)
target_link_libraries(custom_ai PRIVATE Threads::Threads)

if(WIN32)
    target_link_libraries(custom_ai PRIVATE ws2_32)
endif()
