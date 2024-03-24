cmake_minimum_required(VERSION 3.18)

project(raytrace)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -std=c++17 -g")

aux_source_directory(. SRC_DIR)

add_executable(${PROJECT_NAME} ${SRC_DIR})
target_link_libraries(${PROJECT_NAME} pthread)