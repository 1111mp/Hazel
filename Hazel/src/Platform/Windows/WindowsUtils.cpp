#include "Hazel/Utils/PlatformUtils.h"

//#include <commdlg.h>
#include <portable-file-dialogs.h>
//#include <GLFW/glfw3.h>
//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <GLFW/glfw3native.h>

#include "Hazel/Core/Application.h"

namespace Hazel {

  std::optional<std::string> FileDialogs::OpenFile(const char* filter)
  {
//    OPENFILENAME openFile;
//    CHAR szFile[260] = { 0 };
//    ZeroMemory(&openFile, sizeof(OPENFILENAME));
//    openFile.lStructSize = sizeof(OPENFILENAME);
//    openFile.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
//    openFile.lpstrFile = szFile;
//    openFile.nMaxFile = sizeof(szFile);
//    openFile.lpstrFilter = filter;
//    openFile.nFilterIndex = 1;
//    openFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
//    if (GetOpenFileNameA(&openFile) == TRUE)
//    {
//      return openFile.lpstrFile;
//    }
//    return std::nullopt;
		auto selection = pfd::open_file("Select a Hazel scene file.", ".", { "Hazel Scene", "*.hazel" }, pfd::opt::none).result();
		if(!selection.empty())
			return selection[0];
		
		return std::nullopt;
  }

  std::optional<std::string> FileDialogs::SaveFile(const char* filter)
  {
//    OPENFILENAMEA openFile;
//    CHAR szFile[260] = { 0 };
//    ZeroMemory(&openFile, sizeof(OPENFILENAME));
//    openFile.lStructSize = sizeof(OPENFILENAME);
//    openFile.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
//    openFile.lpstrFile = szFile;
//    openFile.nMaxFile = sizeof(szFile);
//    openFile.lpstrFilter = filter;
//    openFile.nFilterIndex = 1;
//    // Sets the default extension by extracting it from the filter
//    openFile.lpstrDefExt = strchr(filter, '\0') + 1;
//    openFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
//    if (GetSaveFileNameA(&openFile) == TRUE)
//    {
//      return openFile.lpstrFile;
//    }
//    return std::nullopt;
		auto destination = pfd::save_file("Select a Hazel scene file", ".", {}, pfd::opt::force_overwrite).result();
		if(!destination.empty())
			return destination;
		
		return std::nullopt;
  }

}
