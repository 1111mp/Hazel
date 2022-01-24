# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.16)

project (Hazel C CXX)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

include_directories(
  Thirdparty/spdlog/include
)

# Include sub-projects.
add_subdirectory(Thirdparty)
add_subdirectory(Hazel)
add_subdirectory(Sandbox)
