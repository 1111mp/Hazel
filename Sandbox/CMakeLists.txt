cmake_minimum_required (VERSION 3.16)

file(GLOB_RECURSE ${PROJECT_NAME_U}_HEADER_FILES src/*.h *.hpp)
file(GLOB_RECURSE ${PROJECT_NAME_U}_SOURCE_FILES src/*.cpp)

source_group("Header Files" FILES ${${PROJECT_NAME_U}_HEADER_FILES})
source_group("Source Files" FILES ${${PROJECT_NAME_U}_SOURCE_FILES})

include_directories(
  ${CMAKE_SOURCE_DIR}/Hazel/src
)

function(my_add_executable)
  foreach(_source IN ITEMS ${ARGN})
    assign_source_group(${_source})
  endforeach()
  add_executable(${ARGV})
endfunction(my_add_executable)

# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

my_add_executable(${PROJECT_NAME}
  ${${PROJECT_NAME_U}_HEADER_FILES}
  ${${PROJECT_NAME_U}_SOURCE_FILES}
)

find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
  hazel_lib
  spdlog::spdlog
  ${OPENGL_LIBRARIES}
  glfw
  Glad
  imgui
)
