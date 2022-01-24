cmake_minimum_required (VERSION 3.16)

# Determine the platform.
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  message("It's OS_MACOSX!")
  set(OS_MACOSX 1)
  set(OS_POSIX 1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  message("It's OS_LINUX!")
  set(OS_LINUX 1)
  set(OS_POSIX 1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  message("It's OS_WINDOWS!")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
  set(CMAKE_CXX_FLAGS_DIST "${CMAKE_CXX_FLAGS_RELEASE} /MD")
  set(OS_WINDOWS 1)
endif()

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
  spdlog::spdlog
  hazel_lib
)
