cmake_minimum_required(VERSION 3.14...3.27)

project(
  logstats
  VERSION 0.1
  DESCRIPTION "An example project with CMake"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(apps)


