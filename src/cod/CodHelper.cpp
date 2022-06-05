#include <fstream>
#include <regex>
#include "CodHelper.h"
#include "StringHelper.h"

std::vector<std::string> mdcii::cod::CodHelper::ReadFile(const std::string& t_path, const bool t_decode, const bool t_filterEmptyLines)
{
    std::ifstream input(t_path, std::ios::binary);
    std::vector<std::string> codTxt;
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    if (t_decode)
    {
        for (auto& c : buffer)
        {
            c = -c;
        }
    }

    std::string line;
    for (size_t i{ 0 }; i < buffer.size() - 1; ++i)
    {
        if (buffer[i + 1] != '\n' && buffer[i] != '\r')
        {
            line.append(1, buffer[i]);
        }
        else
        {
            auto res{ TrimCommentFromLine(TabsToSpaces(line)) };
            if (res.empty())
            {
                line = "";
            }
            else
            {
                line = res[0];
            }

            if (t_filterEmptyLines == false)
            {
                codTxt.push_back(iso_8859_1_to_utf8(line));
            }
            line = "";
            i++; // hop over '\n'
        }
    }

    return codTxt;
}

std::vector<std::string> mdcii::cod::CodHelper::ReadFileAsString(const std::string& t_buffer, bool t_filterEmptyLines)
{
    std::string line;
    std::vector<std::string> codTxt;
    for (size_t i{ 0 }; i < t_buffer.size() - 1; ++i)
    {
        if (t_buffer[i + 1] != '\n' && t_buffer[i] != '\r')
        {
            line.append(1, t_buffer[i]);
        }
        else
        {
            auto res{ TrimCommentFromLine(TabsToSpaces(line)) };
            if (res.empty())
            {
                line = "";
            }
            else
            {
                line = res[0];
            }

            if (t_filterEmptyLines == false)
            {
                codTxt.push_back(line);
            }
            line = "";
            i++; // hop over '\n'
        }
    }

    return codTxt;
}

std::vector<std::string> mdcii::cod::CodHelper::RegexMatch(const std::string& t_regex, const std::string& t_str)
{
    std::vector<std::string> ret;
    const std::regex expr{ t_regex };
    std::smatch what;
    if (std::regex_match(t_str, what, expr))
    {
        for (const auto& i : what)
        {
            ret.push_back(i);
        }
    }

    return ret;
}

std::vector<std::string> mdcii::cod::CodHelper::RegexSearch(const std::string& t_regex, const std::string& t_str)
{
    std::vector<std::string> ret;
    const std::regex expr{ t_regex };
    std::smatch what;
    if (std::regex_search(t_str, what, expr))
    {
        for (const auto& i : what)
        {
            ret.push_back(i);
        }
    }

    return ret;
}

std::string mdcii::cod::CodHelper::TabsToSpaces(const std::string& t_str)
{
    const std::string newtext = "  ";
    const std::regex re("\t");

    std::string result{ std::regex_replace(t_str, re, newtext) };

    return result;
}

int mdcii::cod::CodHelper::CountFrontSpaces(const std::string& t_str)
{
    auto numberOfSpaces{ 0 };
    const auto result{ RegexSearch("(\\s*)(\\w+)", t_str) };
    if (!result.empty())
    {
        for (auto& iter : result[1])
        {
            if (iter == ' ')
            {
                numberOfSpaces++;
            }
            if (iter != ' ')
            {
                break;
            }
        }
    }

    return numberOfSpaces;
}

std::string mdcii::cod::CodHelper::RemoveTrailingCharacters(const std::string& t_str, const char t_charToRemove)
{
    auto ret = t_str;
    ret.erase(ret.find_last_not_of(t_charToRemove) + 1, std::string::npos);

    return ret;
}

std::string mdcii::cod::CodHelper::RemoveLeadingCharacters(const std::string& t_str, const char t_charToRemove)
{
    auto ret = t_str;
    ret.erase(0, std::min(ret.find_first_not_of(t_charToRemove), ret.size() - 1));

    return ret;
}

std::string mdcii::cod::CodHelper::TrimSpacesLeadingTrailing(const std::string& t_s)
{
    auto start = t_s.begin();
    while (start != t_s.end() && std::isspace(*start))
    {
        start++;
    }

    auto end = t_s.end();
    do
    {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return { start, end + 1 };
}

bool mdcii::cod::CodHelper::IsEmpty(const std::string& t_str)
{
    auto hasOnlySpaces = [](const std::string& t_s) {
        return t_s.find_first_not_of(' ') == std::string::npos;
    };

    if (t_str.empty() || hasOnlySpaces(t_str))
    {
        return true;
    }

    return false;
}

bool mdcii::cod::CodHelper::IsSubstring(const std::string& t_str, const std::string& t_substr)
{
    const std::size_t found{ t_str.find(t_substr) };
    if (found != std::string::npos)
    {
        return true;
    }

    return false;
}

std::vector<std::string> mdcii::cod::CodHelper::SplitByDelimiter(const std::string& t_str, const std::string& t_delim)
{
    std::vector<std::string> strings;
    size_t start;
    size_t end{ 0 };
    while ((start = t_str.find_first_not_of(t_delim, end)) != std::string::npos) {
        end = t_str.find(t_delim, start);
        strings.push_back(t_str.substr(start, end - start));
    }

    return strings;
}

std::vector<std::string> mdcii::cod::CodHelper::TrimCommentFromLine(const std::string& t_str)
{
    return SplitByDelimiter(t_str, ";");
}

bool mdcii::cod::CodHelper::BeginsWith(const std::string& t_str, const std::string& t_begin)
{
    if (t_str.rfind(t_begin, 0) == 0)
    {
        return true;
    }

    return false;
}

std::string mdcii::cod::CodHelper::RemoveDigits(const std::string& t_str)
{
    std::string ret{ t_str };
    ret.erase(std::remove_if(std::begin(ret), std::end(ret),
        [](auto t_ch) { return std::isdigit(t_ch); }),
        ret.end());

    return ret;
}
