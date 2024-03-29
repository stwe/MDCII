// This file is part of the MDCII project.
//
// Copyright (c) 2024. stwe <https://github.com/stwe/MDCII>
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

#include "resource/MdciiFile.h"

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

    /**
     * @brief Searches for MDCII Json files in the specified path with a given extension.
     *
     * @param t_relPath The relative path to the RESOURCES_REL_PATH.
     * @param t_extension Search for files with this extension.
     *
     * @return A list of found files.
     */
    [[nodiscard]] std::vector<resource::MdciiFile> get_mdcii_files_list(const std::string& t_relPath, const std::string& t_extension);

    /**
     * @brief Creates and opens a new file.
     *
     * @param t_fileName The name of the file.
     * @param t_file The opened file.
     *
     * @return True if success or false if error while creating (e.g. if the file already exists).
     */
    [[nodiscard]] bool create_file(const std::string& t_fileName, std::ofstream& t_file);

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

    //-------------------------------------------------
    // ImGui widgets
    //-------------------------------------------------

    /**
     * @brief Provides a simple mechanism to choose a MDCII Json file.
     *
     * @param t_files A list of files.
     *
     * @return The index of the selected file, or -1 if nothing is selected.
     */
    [[nodiscard]] int render_mdcii_file_chooser(std::vector<resource::MdciiFile>& t_files);

    /**
     * @brief A text field used to enter a file name.
     *
     * @param t_label The label of the text field.
     * @param t_str The entered text.
     */
    void save_file_button(const char* t_label, std::string* t_str);

    /**
     * @brief A ListBox showing MDCII Json files.
     *
     * @param t_files The files to display.
     * @param t_currentItem The index of the currently selected file.
     */
    void file_chooser(std::vector<resource::MdciiFile>& t_files, int* t_currentItem);

    //-------------------------------------------------
    // ImGui helper
    //-------------------------------------------------

    /**
     * @brief Helper to use std::vector with ImGui Listbox.
     *
     * @param t_vec A std::vector of strings.
     * @param t_index The index of an element in the std::vector.
     * @param t_outText The text showing in the ListBox.
     *
     * @return True or false.
     */
    [[nodiscard]] bool vector_getter(void* t_vec, int32_t t_index, const char** t_outText);
}
