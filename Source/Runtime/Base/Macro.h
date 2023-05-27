#pragma once

#include "Runtime/Base/Log.h"

#define PERMIT_COPY(class) \
    class(const class&)            = delete; \
    class& operator=(const class&) = delete;

#define PERMIT_MOVE(class) \
    class(class &&)           = delete; \
    class& operator=(class&&) = delete;

// Core log macros
#define WIND_CORE_TRACE(...) ::wind::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define WIND_CORE_INFO(...) ::wind::Log::GetCoreLogger()->info(__VA_ARGS__)
#define WIND_CORE_WARN(...) ::wind::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define WIND_CORE_ERROR(...) ::wind::Log::GetCoreLogger()->error(__VA_ARGS__)
#define WIND_CORE_CRITICAL(...) ::wind::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define WIND_TRACE(...) ::wind::Log::GetClientLogger()->trace(__VA_ARGS__)
#define WIND_INFO(...) ::wind::Log::GetClientLogger()->info(__VA_ARGS__)
#define WIND_WARN(...) ::wind::Log::GetClientLogger()->warn(__VA_ARGS__)
#define WIND_ERROR(...) ::wind::Log::GetClientLogger()->error(__VA_ARGS__)
#define WIND_CRITICAL(...) ::wind::Log::GetClientLogger()->critical(__VA_ARGS__)