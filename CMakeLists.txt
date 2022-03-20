# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.16)

project (Hazel C CXX)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

configure_file (
  "${PROJECT_SOURCE_DIR}/HazelConfig.h.in"
  "${PROJECT_SOURCE_DIR}/Hazel/src/HazelConfig.h"
)

# Determine the platform.
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  message("It's OS_MACOSX!")
  set(OS_MACOSX 1)
  set(OS_POSIX 1)
  add_compile_definitions(HZ_PLATFORM_MACOS)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  message("It's OS_LINUX!")
  set(OS_LINUX 1)
  set(OS_POSIX 1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(OS_WINDOWS 1)
  if(CMAKE_CL_64)
    message("It's OS_WINDOWS_x64!")
    set(platform x64)
    add_compile_definitions(HZ_PLATFORM_WINDOWS)
  else(CMAKE_CL_64)
    message("It's OS_WINDOWS_x86!")
    set(platform x86)
  endif(CMAKE_CL_64)
endif()


if(OS_MACOSX)
	if((${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64") OR (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64"))
    message("---------------OS_MACOSX_ARM64---------------")
    set(OS_MACOSX_ARM64 1)
    # link_directories(${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GLEW/lib)
  else()
    message("-------------------OS_MACOSX-----------------")
  endif()
elseif(OS_WINDOWS)
	message("-------------------OS_WINDOWS-----------------")
endif()

# https://cmake.org/cmake/help/latest/variable/CMAKE_MSVC_RUNTIME_LIBRARY.html#:~:text=CMAKE_MSVC_RUNTIME_LIBRARY%20%C2%B6%20New%20in%20version%203.15.%20Select%20the,the%20try_compile%20%28%29%20command%20into%20the%20test%20project.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Determine the project architecture.
if(NOT DEFINED PROJECT_ARCH)
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(PROJECT_ARCH "x86_64")
  else()
    set(PROJECT_ARCH "x86")
  endif()

  if(OS_MACOSX)
    # PROJECT_ARCH should be specified on Mac OS X.
    message("MacOS arch directory is Out!")
  endif()
endif()

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
  add_compile_definitions(HZ_ENABLE_ASSERTS HZ_DEBUG)
endif()

add_compile_definitions(HZ_RENDERER_OPENGL)
# add_compile_definitions(HZ_RENDERER_VULKAN)

function(assign_source_group)
  foreach(_source IN ITEMS ${ARGN})
    if (IS_ABSOLUTE "${_source}")
      file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
    else()
      set(_source_rel "${_source}")
    endif()
    get_filename_component(_source_path "${_source_rel}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
  endforeach()
endfunction(assign_source_group)

include_directories(
  "Thirdparty/Glad/include"
  "Thirdparty/spdlog/include"
  "Thirdparty/imgui"
  "Thirdparty/glm"
  "Thirdparty/stb_image"
  "Thirdparty/entt/include"
  "Thirdparty/yaml-cpp/include"
  "Thirdparty/ImGuizmo"
)

# Include sub-projects.
add_subdirectory("Thirdparty")
add_subdirectory("Hazel")
add_subdirectory("Hazelnut")
add_subdirectory("Sandbox")
