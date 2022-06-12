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

#pragma once

#include <string>
#include <vector>

namespace mdcii::cod
{
    class CodHelper
    {
    public:

        static std::vector<std::string> RegexMatch(const std::string& t_regex, const std::string& t_str);
        static std::vector<std::string> RegexSearch(const std::string& t_regex, const std::string& t_str);
        static std::string TabsToSpaces(const std::string& t_str);
        static int CountFrontSpaces(const std::string& t_str);
        static std::string TrimSpacesLeadingTrailing(const std::string& t_s);
        static bool IsEmpty(const std::string& t_str);
        static bool IsSubstring(const std::string& t_str, const std::string& t_substr);
        static std::vector<std::string> SplitByDelimiter(const std::string& t_str, const std::string& t_delim);
        static std::vector<std::string> TrimCommentFromLine(const std::string& t_str);

    protected:

    private:

    };
}
