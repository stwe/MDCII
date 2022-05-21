#pragma once

#include "Log.h"

#if defined(_WIN64) && defined(_MSC_VER)
    #define MDCII_DEBUG_BREAK __debugbreak()
#elif defined(__linux__) && defined(__GNUC__) && (__GNUC__ >= 7)
    #include <csignal>
    #define MDCII_DEBUG_BREAK raise(SIGTRAP)
#else
    #error Unsupported platform or compiler!
#endif

#ifdef MDCII_DEBUG_BUILD
    #define MDCII_ENABLE_ASSERTS
#endif

#ifdef MDCII_ENABLE_ASSERTS
    #define MDCII_ASSERT(x, ...) { if(!(x)) { mdcii::Log::MDCII_LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); MDCII_DEBUG_BREAK; } }
#else
    #define MDCII_ASSERT(x, ...)
#endif
