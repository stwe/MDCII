#pragma once

#include <string>

namespace mdcii::cod
{
    std::wstring stringToWstring(const std::string& t_utf8Str);
    std::string iso_8859_1_to_utf8(const std::string& t_str);
    std::string removeTrailingCarriageReturnNewline(const std::string& t_input);
}