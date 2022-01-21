# CMakeList.txt : CMake project for Hazel, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

set(PROJ_LIB_NAME "hazel_lib")

include(GenerateExportHeader)

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
  set(OS_WINDOWS 1)
endif()

# Determine the project architecture.
if(NOT DEFINED PROJECT_ARCH)
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(PROJECT_ARCH "x86_64")
  else()
    set(PROJECT_ARCH "x86")
  endif()

  if(OS_MACOSX)
    # PROJECT_ARCH should be specified on Mac OS X.
    message(WARNING "MacOS arch directory is Out!")
  endif()
endif()

file(GLOB_RECURSE ${PROJ_LIB_NAME}_HEADER_FILES src/*.h *.hpp)
file(GLOB_RECURSE ${PROJ_LIB_NAME}_SOURCE_FILES src/*.cpp *.cpp)

source_group("Header Files" FILES ${${PROJ_LIB_NAME}_HEADER_FILES})
source_group("Source Files" FILES ${${PROJ_LIB_NAME}_SOURCE_FILES})

include_directories(
  Dependencies/GLFW/include
  Dependencies/GLEW/include
)

if(OS_MACOSX)
	if((${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64") OR (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64"))
    message("OS_MACOSX_ARM64, using ${PROJECT_SOURCE_DIR}/Dependencies/GLFW/lib-arm64")
    link_directories(
      Dependencies/GLFW/lib-x86_64
    )
  else()
    message("OS_MACOSX, using ${PROJECT_SOURCE_DIR}/Dependencies/GLFW/lib-x86_64")
	  link_directories(
		  Dependencies/GLFW/lib-x86_64
	  )
  endif()
elseif(OS_WINDOWS)
	message("OS_WINDOWS, using {PROJECT_SOURCE_DIR}/Dependencies/GLFW/lib-vc2022")
	link_directories(
		Dependencies/GLFW/lib-vc2022
		Dependencies/GLEW/lib
	)
endif()

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# Add source to this project's executable.
add_library(${PROJ_LIB_NAME} SHARED
  ${${PROJ_LIB_NAME}_HEADER_FILES}
  ${${PROJ_LIB_NAME}_SOURCE_FILES}
)

# TODO: Add tests and install targets if needed.
generate_export_header(${PROJ_LIB_NAME}
  EXPORT_MACRO_NAME ${PROJECT_NAME}Exports
  EXPORT_FILE_NAME ${CMAKE_SOURCE_DIR}/Hazel/src/Hazel/${PROJECT_NAME}Exports.h
)

target_link_libraries(${PROJ_LIB_NAME} PRIVATE
	glfw3
  glew32s
)
