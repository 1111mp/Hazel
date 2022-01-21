#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Hazel {

  class HazelExports Log 
  {
  public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger>& GetHazelLogger() { return s_HazelLogger; }
    inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
  
  private:
    static std::shared_ptr<spdlog::logger> s_HazelLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
  };

}

// Hazel log macros
#define HZ_HAZEL_TRACE(...) ::Hazel::Log::GetHazelLogger()->trace(__VA_ARGS__)
#define HZ_HAZEL_INFO(...) ::Hazel::Log::GetHazelLogger()->info(__VA_ARGS__)
#define HZ_HAZEL_WARN(...) ::Hazel::Log::GetHazelLogger()->warn(__VA_ARGS__)
#define HZ_HAZEL_ERROR(...) ::Hazel::Log::GetHazelLogger()->error(__VA_ARGS__)
#define HZ_HAZEL_FATAL(...) ::Hazel::Log::GetHazelLogger()->fatal(__VA_ARGS__)

// Client log macros
#define HZ_TRACE(...) ::Hazel::Log::GetClientLogger()->trace(__VA_ARGS__)
#define HZ_INFO(...) ::Hazel::Log::GetClientLogger()->info(__VA_ARGS__)
#define HZ_WARN(...) ::Hazel::Log::GetClientLogger()->warn(__VA_ARGS__)
#define HZ_ERROR(...) ::Hazel::Log::GetClientLogger()->error(__VA_ARGS__)
#define HZ_FATAL(...) ::Hazel::Log::GetClientLogger()->fatal(__VA_ARGS__)
