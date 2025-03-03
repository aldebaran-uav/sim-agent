cmake_minimum_required(VERSION 3.10)

project(sim-agent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(MAVSDK REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

set(HEADERS
    src/agent.h
    src/commander.h
    src/telemetry_manager.h
    src/server_manager.h
)

set(SOURCES
    src/main.cpp
    src/agent.cpp
    src/commander.cpp
    src/telemetry_manager.cpp
    src/server_manager.cpp
)

# List all source files explicitly
add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    MAVSDK::mavsdk
    CURL::libcurl
    nlohmann_json::nlohmann_json
)