#pragma once

// How to add precompiled headers in CMake for visual studio generator
// https://stackoverflow.com/questions/62541168/how-to-add-precompiled-headers-in-cmake-for-visual-studio-generator

#ifdef HZ_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		// See https://github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
		#define NOMINMAX
	#endif
#endif

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Hazel/Core/Log.h"

#include "Hazel/Debug/Instrumentor.h"

#ifdef HZ_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

#ifdef __APPLE__
	#define GL_SILENCE_DEPRECATION
#endif
