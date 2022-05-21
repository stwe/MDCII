#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

//-------------------------------------------------
// Log
//-------------------------------------------------

namespace mdcii
{
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
        // Log
        //-------------------------------------------------

        template <typename ...T>
        static void MDCII_LOG_TRACE(T ...t_args)
        {
            m_logger->trace(t_args...);
        }

        template <typename ...T>
        static void MDCII_LOG_DEBUG(T ...t_args)
        {
            m_logger->debug(t_args...);
        }

        template <typename ...T>
        static void MDCII_LOG_INFO(T ...t_args)
        {
            m_logger->info(t_args...);
        }

        template <typename ...T>
        static void MDCII_LOG_WARN(T ...t_args)
        {
            m_logger->warn(t_args...);
        }

        template <typename ...T>
        static void MDCII_LOG_ERROR(T ...t_args)
        {
            m_logger->error(t_args...);
        }

        template <typename ...T>
        static void MDCII_LOG_FATAL(T ...t_args)
        {
            m_logger->critical(t_args...);
        }

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        inline static std::shared_ptr<spdlog::logger> m_logger;
    };
}
