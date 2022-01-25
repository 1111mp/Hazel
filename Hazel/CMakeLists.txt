# CMakeList.txt : CMake project for Hazel, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.16)

set(PROJ_LIB_NAME "hazel_lib")

add_compile_definitions(GLFW_INCLUDE_NONE)

include(GenerateExportHeader)

file(GLOB_RECURSE ${PROJ_LIB_NAME}_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h *.hpp)
file(GLOB_RECURSE ${PROJ_LIB_NAME}_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp *.cpp)

source_group("Header Files" FILES ${${PROJ_LIB_NAME}_HEADER_FILES})
source_group("Source Files" FILES ${${PROJ_LIB_NAME}_SOURCE_FILES})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/Thirdparty/Glad/include
  ${CMAKE_SOURCE_DIR}/Thirdparty/imgui/backends
)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# Add source to this project's executable.
add_library(${PROJ_LIB_NAME} SHARED
  ${${PROJ_LIB_NAME}_HEADER_FILES}
  ${${PROJ_LIB_NAME}_SOURCE_FILES}
  ${CMAKE_SOURCE_DIR}/Thirdparty/imgui/backends/imgui_impl_glfw.cpp
)

target_precompile_headers(${PROJ_LIB_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/hzpch.h
)

# TODO: Add tests and install targets if needed.
generate_export_header(${PROJ_LIB_NAME}
  EXPORT_MACRO_NAME ${PROJECT_NAME}Exports
  EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/src/Hazel/${PROJECT_NAME}Exports.h
)

if(OS_MACOSX)
  # https://www.glfw.org/docs/latest/build_guide.html#build_link_cmake_package
  # brew install glfw glew
  message(STATUS "glfw3------------opengl----------glew")
  # find_package(glfw3 REQUIRED)
  find_package(OpenGL REQUIRED)
  # find_package(GLEW REQUIRED)
  # find_library(GLEW_LIB glew32s ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GLEW/lib)

  target_link_libraries(${PROJ_LIB_NAME} PRIVATE
    ${OPENGL_LIBRARIES}
    glfw
    Glad
    imgui
    # GLEW::GLEW
    # GLEW_LIB
  )

else()
  find_package(OpenGL REQUIRED)

  target_link_libraries(${PROJ_LIB_NAME} PRIVATE
    ${OPENGL_LIBRARIES}
    glfw
    Glad
    imgui
  )
endif()
