#pragma once

#include <exception>
#include <sstream>

//-------------------------------------------------
// MdciiException
//-------------------------------------------------

namespace mdcii
{
    class MdciiException : public std::exception
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MdciiException(const int t_line, const char* t_file, std::string t_message)
            : m_line{ t_line }
            , m_file{ t_file }
            , m_message{ std::move(t_message) }
        {}

        //-------------------------------------------------
        // What
        //-------------------------------------------------

        const char* what() const noexcept override
        {
            std::ostringstream oss;

            oss << "occurred: \n";
            oss << GetOriginString();

            m_whatBuffer = oss.str();

            return m_whatBuffer.c_str();
        }

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        int m_line;
        std::string m_file;
        std::string m_message;

        mutable std::string m_whatBuffer;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        std::string GetOriginString() const
        {
            std::ostringstream oss;

            oss << "[Message]: " << m_message << "\n";
            oss << "[File]: " << m_file << "\n";
            oss << "[Line]: " << m_line;

            return oss.str();
        }
    };
}

#define MDCII_EXCEPTION(message) mdcii::MdciiException(__LINE__, __FILE__, message)
