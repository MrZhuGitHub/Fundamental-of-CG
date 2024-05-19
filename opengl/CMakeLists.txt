cmake_minimum_required(VERSION 2.8)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ldl -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ldl -g")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rd-party-lib/glm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rd-party-lib/stb)
include_directories(/usr/include/freetype2)
 
project(test)
 
find_package(glfw3 REQUIRED)
 
file(GLOB project_file glad.c main.cpp)

aux_source_directory(. SRC_DIR)
add_executable(${PROJECT_NAME} ${SRC_DIR} ${project_file})
 
target_link_libraries(${PROJECT_NAME} glfw assimp freetype)
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})