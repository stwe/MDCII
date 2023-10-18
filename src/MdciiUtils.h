// This file is part of the MDCII project.
//
// Copyright (c) 2023. stwe <https://github.com/stwe/MDCII>
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

#include "vendor/nlohmann/json.hpp"

namespace mdcii
{
    //-------------------------------------------------
    // Files
    //-------------------------------------------------

    /**
     * @brief Reads and deserialize a Json file.
     *
     * @param t_filePath The path to the Json file.
     *
     * @return The Json value.
     */
    [[nodiscard]] nlohmann::json read_json_from_file(const std::string& t_filePath);

    //-------------------------------------------------
    // Strings
    //-------------------------------------------------

    /**
     * @brief Convert a string to lower case.
     *
     * @param t_string The string to convert.
     *
     * @return The converted string.
     */
    [[nodiscard]] std::string to_lower_case(const std::string& t_string);

    /**
     * @brief Convert a string to upper case.
     *
     * @param t_string The string to convert.
     *
     * @return The converted string.
     */
    [[nodiscard]] std::string to_upper_case(const std::string& t_string);
}
