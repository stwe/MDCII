#pragma once

#include <string>
#include <vector>

namespace mdcii::cod
{
    class CodHelper
    {
    public:

        static std::vector<std::string> ReadFile(const std::string& t_path, bool t_decode, bool t_filterEmptyLines = true);
        static std::vector<std::string> ReadFileAsString(const std::string& t_buffer, bool t_filterEmptyLines = true);

        static std::vector<std::string> RegexMatch(const std::string& t_regex, const std::string& t_str);
        static std::vector<std::string> RegexSearch(const std::string& t_regex, const std::string& t_str);
        static std::string TabsToSpaces(const std::string& t_str);
        static int CountFrontSpaces(const std::string& t_str);
        static std::string RemoveTrailingCharacters(const std::string& t_str, char t_charToRemove);
        static std::string RemoveLeadingCharacters(const std::string& t_str, char t_charToRemove);
        static std::string TrimSpacesLeadingTrailing(const std::string& t_s);
        static bool IsEmpty(const std::string& t_str);
        static bool IsSubstring(const std::string& t_str, const std::string& t_substr);
        static std::vector<std::string> SplitByDelimiter(const std::string& t_str, const std::string& t_delim);
        static std::vector<std::string> TrimCommentFromLine(const std::string& t_str);
        static bool BeginsWith(const std::string& t_str, const std::string& t_begin);
        static std::string RemoveDigits(const std::string& t_str);

    protected:

    private:

    };
}
