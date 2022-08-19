// This file is part of the MDCII Game Engine.
// Copyright (C) 2019  Armin Schlegel
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <regex>
#include "CodHelper.h"

//-------------------------------------------------
// Helper
//-------------------------------------------------

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
    while ((start = t_str.find_first_not_of(t_delim, end)) != std::string::npos)
    {
        end = t_str.find(t_delim, start);
        strings.push_back(t_str.substr(start, end - start));
    }

    return strings;
}

std::vector<std::string> mdcii::cod::CodHelper::TrimCommentFromLine(const std::string& t_str)
{
    return SplitByDelimiter(t_str, ";");
}
