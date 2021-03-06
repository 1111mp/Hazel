cmake_minimum_required (VERSION 3.16)

set(PROJ_NAME "Hazelnut")

file(GLOB_RECURSE ${PROJ_NAME}_HEADER_FILES src/*.h *.hpp)
file(GLOB_RECURSE ${PROJ_NAME}_SOURCE_FILES src/*.cpp)

source_group("Header Files" FILES ${${PROJ_NAME}_HEADER_FILES})
source_group("Source Files" FILES ${${PROJ_NAME}_SOURCE_FILES})

function(my_add_executable)
  foreach(_source IN ITEMS ${ARGN})
    assign_source_group(${_source})
  endforeach()
  add_executable(${ARGV})
endfunction(my_add_executable)

# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

my_add_executable(Hazelnut
  ${${PROJ_NAME}_HEADER_FILES}
  ${${PROJ_NAME}_SOURCE_FILES}
)

target_include_directories(Hazelnut PRIVATE
  ${CMAKE_SOURCE_DIR}/Hazel/src
  src
)

target_link_libraries(Hazelnut PRIVATE
  hazel_lib
  spdlog::spdlog
  glfw
  Glad
  imgui
  stb_image
  yaml-cpp
)
