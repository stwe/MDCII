// This file is part of the MDCII project.
//
// Copyright (c) 2022. stwe <https://github.com/stwe/MDCII>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#pragma once

#include "Log.h"

#if defined(_WIN64) && defined(_MSC_VER)
    #define MDCII_DEBUG_BREAK __debugbreak()
#elif defined(__linux__) && defined(__GNUC__) && (__GNUC__ >= 7)
    #include <csignal>
    #define MDCII_DEBUG_BREAK raise(SIGTRAP)
#elif defined(__MINGW64__) && defined(__GNUC__) && (__GNUC__ >= 7)
    #define MDCII_DEBUG_BREAK __builtin_trap()
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
