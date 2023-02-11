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

#include "data/json.hpp"

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
    [[nodiscard]] nlohmann::json read_json_from_file(const std::string& t_filePath);

    /**
     * Searches for files in the specified path with a given extension.
     *
     * @param t_relPath The relative path to the RESOURCES_REL_PATH.
     * @param t_extension Search for files with this extension.
     *
     * @return A list of found files.
     */
    [[nodiscard]] std::vector<std::string> get_files_list(const std::string& t_relPath, const std::string& t_extension);

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
    [[nodiscard]] std::string to_lower_case(const std::string& t_string);

    /**
     * Convert a string to upper case.
     *
     * @param t_string The string to convert.
     *
     * @return The converted string.
     */
    [[nodiscard]] std::string to_upper_case(const std::string& t_string);

    //-------------------------------------------------
    // ImGui widgets
    //-------------------------------------------------

    /**
     * An ImGui-Widget to render a Toggle-Button.
     *
     * @param t_strId An Id.
     * @param t_v A static bool.
     */
    [[maybe_unused]] void toggle_imgui_button(const std::string& t_strId, bool* t_v);

    /**
     * A custom Begin() with centered window.
     *
     * @param t_name The name of the window.
     *
     * @return Return false to indicate the window is collapsed or fully clipped.
     */
    bool begin_centered(const char* t_name);

    /**
     * A custom Begin() with right aligned window.
     *
     * @param t_name The name of the window.
     * @param t_offset Offset in x direction.
     *
     * @return Return false to indicate the window is collapsed or fully clipped.
     */
    bool begin_right(const char* t_name, float t_offset);

    /**
     * A custom Begin() with bottom right aligned window.
     *
     * @param t_name The name of the window.
     * @param t_offset Offset in x direction.
     *
     * @return Return false to indicate the window is collapsed or fully clipped.
     */
    bool begin_bottom_right(const char* t_name, float t_offset);

    //-------------------------------------------------
    // ImGui helper
    //-------------------------------------------------

    /**
     * Helper to use std::vector with ImGui Listbox.
     *
     * @param t_vec A std::vector of strings.
     * @param t_index The index of an element in the std::vector.
     * @param t_outText The text showing in the ListBox.
     *
     * @return True or false.
     */
    [[nodiscard]] bool vector_getter(void* t_vec, int32_t t_index, const char** t_outText);
}
