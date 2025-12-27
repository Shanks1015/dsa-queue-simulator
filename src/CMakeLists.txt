cmake_minimum_required(VERSION 3.20)
project(traffic_simulator)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL3 REQUIRED)

add_executable(traffic_sim
    main.cpp
    queue.cpp
)

target_link_libraries(traffic_sim SDL3::SDL3)
