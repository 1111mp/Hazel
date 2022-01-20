cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE ${PROJECT_NAME_U}_HEADER_FILES src/*.h *.hpp)
file(GLOB_RECURSE ${PROJECT_NAME_U}_SOURCE_FILES src/*.cpp)

source_group("Header Files" FILES ${${PROJECT_NAME_U}_HEADER_FILES})
source_group("Source Files" FILES ${${PROJECT_NAME_U}_SOURCE_FILES})

include_directories(
  ${CMAKE_SOURCE_DIR}/Hazel/src
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME}
  ${${PROJECT_NAME_U}_HEADER_FILES}
  ${${PROJECT_NAME_U}_SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  hazel_lib
)
