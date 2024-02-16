// This file is part of the MDCII project.
//
// Copyright (c) 2024. stwe <https://github.com/stwe/MDCII>
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace mdcii
{
    //-------------------------------------------------
    // Log
    //-------------------------------------------------

    /**
     * @brief Provides logging functionality using the spdlog library.
     */
    class Log
    {
    public:
        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        static void Init()
        {
            spdlog::set_pattern("%^[%T] %n: %v%$");

            m_logger = spdlog::stdout_color_mt("MDCII");

#ifdef MDCII_DEBUG_BUILD
            m_logger->set_level(spdlog::level::trace);
#else
            m_logger->set_level(spdlog::level::info);
#endif
        }

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        static std::shared_ptr<spdlog::logger>& GetLogger()
        {
            return m_logger;
        }

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        inline static std::shared_ptr<spdlog::logger> m_logger;
    };
}

#define MDCII_LOG_DEBUG(...)    ::mdcii::Log::GetLogger()->debug(__VA_ARGS__)
#define MDCII_LOG_TRACE(...)    ::mdcii::Log::GetLogger()->trace(__VA_ARGS__)
#define MDCII_LOG_INFO(...)     ::mdcii::Log::GetLogger()->info(__VA_ARGS__)
#define MDCII_LOG_WARN(...)     ::mdcii::Log::GetLogger()->warn(__VA_ARGS__)
#define MDCII_LOG_ERROR(...)    ::mdcii::Log::GetLogger()->error(__VA_ARGS__)
#define MDCII_LOG_CRITICAL(...) ::mdcii::Log::GetLogger()->critical(__VA_ARGS__)
