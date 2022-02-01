# CMakeList.txt : CMake project for Hazel, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.16)

set(PROJ_LIB_NAME "hazel_lib")

# set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_C_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(GLFW_INCLUDE_NONE)

# include(GenerateExportHeader)

file(GLOB_RECURSE ${PROJ_LIB_NAME}_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h *.hpp)
file(GLOB_RECURSE ${PROJ_LIB_NAME}_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp *.cpp)

source_group("Header Files" FILES ${${PROJ_LIB_NAME}_HEADER_FILES})
source_group("Source Files" FILES ${${PROJ_LIB_NAME}_SOURCE_FILES})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

function(my_add_library)
  foreach(_source IN ITEMS ${ARGN})
    assign_source_group(${_source})
  endforeach()
  add_library(${ARGV})
endfunction(my_add_library)

# set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# Add source to this project's executable.
my_add_library(${PROJ_LIB_NAME} STATIC
  ${${PROJ_LIB_NAME}_HEADER_FILES}
  ${${PROJ_LIB_NAME}_SOURCE_FILES}
)

# set_property(TARGET ${PROJ_LIB_NAME} PROPERTY
  # MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_precompile_headers(${PROJ_LIB_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/hzpch.h
)

# TODO: Add tests and install targets if needed.
# generate_export_header(${PROJ_LIB_NAME}
#   EXPORT_MACRO_NAME ${PROJECT_NAME}Exports
#   EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/src/Hazel/${PROJECT_NAME}Exports.h
# )

target_link_libraries(${PROJ_LIB_NAME} PRIVATE
  glfw
)
