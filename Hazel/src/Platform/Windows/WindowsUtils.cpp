#include "Hazel/Utils/PlatformUtils.h"

#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Hazel/Core/Application.h"

namespace Hazel {

  std::string FileDialogs::OpenFile(const char* filter)
  {
    OPENFILENAME openFile;
    CHAR szFile[260] = { 0 };
    ZeroMemory(&openFile, sizeof(OPENFILENAME));
    openFile.lStructSize = sizeof(OPENFILENAME);
    openFile.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
    openFile.lpstrFile = szFile;
    openFile.nMaxFile = sizeof(szFile);
    openFile.lpstrFilter = filter;
    openFile.nFilterIndex = 1;
    openFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetOpenFileNameA(&openFile) == TRUE)
    {
      return openFile.lpstrFile;
    }
    return std::string();
  }

  std::string FileDialogs::SaveFile(const char* filter)
  {
    OPENFILENAMEA openFile;
    CHAR szFile[260] = { 0 };
    ZeroMemory(&openFile, sizeof(OPENFILENAME));
    openFile.lStructSize = sizeof(OPENFILENAME);
    openFile.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
    openFile.lpstrFile = szFile;
    openFile.nMaxFile = sizeof(szFile);
    openFile.lpstrFilter = filter;
    openFile.nFilterIndex = 1;
    openFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetSaveFileNameA(&openFile) == TRUE)
    {
      return openFile.lpstrFile;
    }
    return std::string();
  }

}
