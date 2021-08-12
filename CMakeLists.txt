cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_FLAGS "-O3")

project(AstarVis)

set(CMAKE_CXX_STANDARD 14)

file(GLOB SOURCES "src/*.cpp")
set(CMAKE_MODULE_PATH "/usr/share/SFML/cmake/Modules/")

find_package(SFML 2.3 COMPONENTS system window graphics audio REQUIRED)

add_executable(AstarVis ${SOURCES})

include_directories(include)
target_link_libraries(AstarVis sfml-system sfml-window sfml-graphics sfml-audio X11)

add_executable(test tests/tests.cpp tests/catch_amalgamated.cpp)
