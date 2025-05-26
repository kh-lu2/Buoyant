cmake_minimum_required(VERSION 3.20)

project(buoyant)

set(CMAKE_CXX_STANDARD 20)

AUX_SOURCE_DIRECTORY("${CMAKE_CURRENT_LIST_DIR}/src" SRC)

add_executable(${CMAKE_PROJECT_NAME} main.cpp ${SRC})