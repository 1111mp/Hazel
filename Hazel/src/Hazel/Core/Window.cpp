#include "Window.h"

#if defined(HZ_PLATFORM_WINDOWS)
	#include "Platform/Windows/WindowsWindow.h"
#elif defined(HZ_PLATFORM_MACOS)
	#include "Platform/MacOS/MacOSWindow.h"
#endif

namespace Hazel
{

	Scope<Window> Window::Create(const WindowProps& props)
	{
		#if defined(HZ_PLATFORM_WINDOWS)
			return CreateScope<WindowsWindow>(props);
		#elif defined(HZ_PLATFORM_MACOS)
			return CreateScope<MacOSWindow>(props);
		#else
			HZ_CORE_ASSERT(false, "Unknown platform!");
			return nullptr;
		#endif
	}

}
