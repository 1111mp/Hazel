#include "PlatformUtils.h"

#include <portable-file-dialogs.h>
#include "Hazel/Core/Application.h"

namespace Hazel
{
  std::optional<std::string> FileDialogs::OpenFile(const char *filter)
  {
    auto selection = pfd::open_file("Select a Hazel scene file.", ".", {"Hazel Scene", "*.hazel"}, pfd::opt::none).result();
    if (!selection.empty())
      return selection[0];

    return std::nullopt;
  }

  std::optional<std::string> FileDialogs::SaveFile(const char *filter)
  {
    auto destination = pfd::save_file("Select a Hazel scene file", ".", {}, pfd::opt::force_overwrite).result();
    if (!destination.empty())
      return destination;

    return std::nullopt;
  }
}
