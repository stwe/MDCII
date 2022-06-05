#include <codecvt>
#include <regex>
#include "StringHelper.h"

std::wstring mdcii::cod::stringToWstring(const std::string& t_utf8Str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;

    return conv.from_bytes(t_utf8Str);
}

std::string mdcii::cod::iso_8859_1_to_utf8(const std::string& t_str)
{
    std::string strOut;
    for (const unsigned char ch : t_str)
    {
        if (ch < 0x80)
        {
            strOut.push_back(ch);
        }
        else
        {
            strOut.push_back(0xc0 | ch >> 6);
            strOut.push_back(0x80 | (ch & 0x3f));
        }
    }

    return strOut;
}

std::string mdcii::cod::removeTrailingCarriageReturnNewline(const std::string& t_input)
{
    std::regex r("[\r\n]{2,}");

    return std::regex_replace(t_input, r, "\r\n");
}
