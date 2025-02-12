cmake_minimum_required(VERSION 3.10)

project(sim-agent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(MAVSDK REQUIRED)

# List all source files explicitly
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/commander.cpp
    src/agent.cpp
    src/agent.h
    src/commander.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    MAVSDK::mavsdk
)