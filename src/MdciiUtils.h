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

#include <string>

constexpr std::size_t operator"" _uz(const unsigned long long t_val)
{
    return t_val;
}

namespace mdcii
{
    /**
     * Convert a string to lower case.
     *
     * @param t_string The string to convert.
     *
     * @return The converted string.
     */
    static std::string to_lower_case(const std::string& t_string)
    {
        auto newString{ t_string };
        for (auto& c : newString)
        {
            c = static_cast<char>(std::tolower(c));
        }

        return newString;
    }

    /**
     * Convert a string to upper case.
     *
     * @param t_string The string to convert.
     *
     * @return The converted string.
     */
    static std::string to_upper_case(const std::string& t_string)
    {
        auto newString{ t_string };
        for (auto& c : newString)
        {
            c = static_cast<char>(std::toupper(c));
        }

        return newString;
    }
}
