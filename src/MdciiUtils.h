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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#pragma once

#include <fstream>
#include "data/json.hpp"
#include "MdciiException.h"
#include "Log.h"

//-------------------------------------------------
// Operators
//-------------------------------------------------

constexpr std::size_t operator"" _uz(const unsigned long long t_val)
{
    return t_val;
}

//-------------------------------------------------
// Utils
//-------------------------------------------------

namespace mdcii
{
    //-------------------------------------------------
    // Files
    //-------------------------------------------------

    /**
     * Reads and deserialize a Json file.
     *
     * @param t_filePath The path to the Json file.
     *
     * @return The Json value.
     */
    [[nodiscard]] static nlohmann::json read_json_from_file(const std::string& t_filePath)
    {
        Log::MDCII_LOG_DEBUG("[read_json_from_file] Starts creating Json value from file {}...", t_filePath);

        nlohmann::json j;

        std::ifstream jsonFile;
        jsonFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            jsonFile.open(t_filePath);
            j = nlohmann::json::parse(jsonFile);
            jsonFile.close();
        }
        catch (const std::ifstream::failure&)
        {
            throw MDCII_EXCEPTION("[read_json_from_file()] Exception caught while loading file " + t_filePath + ".");
        }

        Log::MDCII_LOG_DEBUG("[read_json_from_file] The Json value was created successfully.");

        return j;
    }

    //-------------------------------------------------
    // Strings
    //-------------------------------------------------

    /**
     * Convert a string to lower case.
     *
     * @param t_string The string to convert.
     *
     * @return The converted string.
     */
    [[nodiscard]] static std::string to_lower_case(const std::string& t_string)
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
    [[nodiscard]] static std::string to_upper_case(const std::string& t_string)
    {
        auto newString{ t_string };
        for (auto& c : newString)
        {
            c = static_cast<char>(std::toupper(c));
        }

        return newString;
    }
}
