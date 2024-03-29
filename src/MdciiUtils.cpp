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

#include <fstream>
#include "MdciiUtils.h"
#include "MdciiException.h"
#include "Log.h"
#include "Game.h"
#include "Intl.h"
#include "vendor/imgui/imgui_stdlib.h"

//-------------------------------------------------
// Files
//-------------------------------------------------

nlohmann::json mdcii::read_json_from_file(const std::string& t_filePath)
{
    MDCII_LOG_DEBUG("[read_json_from_file()] Starts creating Json value from file {}...", t_filePath);

    nlohmann::json j;

    std::ifstream jsonFile;
    jsonFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        jsonFile.open(t_filePath);
        j = nlohmann::json::parse(jsonFile);

        if (!j.is_object())
        {
            throw MDCII_EXCEPTION("[read_json_from_file()] Invalid Json object.");
        }

        if (j.empty())
        {
            throw MDCII_EXCEPTION("[read_json_from_file()] Invalid Json value.");
        }

        jsonFile.close();
    }
    catch (const std::ifstream::failure&)
    {
        throw MDCII_EXCEPTION("[read_json_from_file()] Exception caught while loading file " + t_filePath + ".");
    }

    MDCII_LOG_DEBUG("[read_json_from_file()] The Json value was created successfully.");

    return j;
}

std::vector<mdcii::resource::MdciiFile> mdcii::get_mdcii_files_list(const std::string& t_relPath, const std::string& t_extension)
{
    std::vector<resource::MdciiFile> results;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(fmt::format("{}{}", Game::RESOURCES_REL_PATH, t_relPath), std::filesystem::directory_options::follow_directory_symlink))
    {
        if (is_regular_file(entry.status()) && entry.path().extension().string() == t_extension)
        {
            results.emplace_back(entry.path().string());
        }
    }

    return results;
}

bool mdcii::create_file(const std::string& t_fileName, std::ofstream& t_file)
{
    if (std::filesystem::exists(t_fileName))
    {
        MDCII_LOG_WARN("[create_file()] The {} file already exists.", t_fileName);

        return false;
    }

    MDCII_LOG_DEBUG("[create_file()] Create new file {}.", t_fileName);

    t_file.open(t_fileName);
    if (!t_file.is_open())
    {
        throw MDCII_EXCEPTION("[create_file()] Error while opening file " + t_fileName + ".");
    }

    return true;
}

//-------------------------------------------------
// Strings
//-------------------------------------------------

std::string mdcii::to_lower_case(const std::string& t_string)
{
    auto newString{ t_string };
    for (auto& c : newString)
    {
        c = static_cast<char>(std::tolower(c));
    }

    return newString;
}

std::string mdcii::to_upper_case(const std::string& t_string)
{
    auto newString{ t_string };
    for (auto& c : newString)
    {
        c = static_cast<char>(std::toupper(c));
    }

    return newString;
}

//-------------------------------------------------
// ImGui widgets
//-------------------------------------------------

int mdcii::render_mdcii_file_chooser(std::vector<resource::MdciiFile>& t_files)
{
    static int32_t fileIndex{ 0 };

    if (t_files.empty())
    {
        ImGui::Text(_("Missing Files"));
    }
    else
    {
        mdcii::file_chooser(t_files, &fileIndex);
        if (ImGui::Button(_("Load File")))
        {
            ImGui::CloseCurrentPopup();
            return fileIndex;
        }
    }

    return -1;
}

void mdcii::save_file_button(const char* t_label, std::string* t_str)
{
    ImGui::InputText(t_label, t_str, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CallbackCharFilter, [](ImGuiInputTextCallbackData* t_data) {
        if (const auto c{ t_data->EventChar }; (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
        {
            return 0;
        }

        return 1;
    });
}

void mdcii::file_chooser(std::vector<resource::MdciiFile>& t_files, int* t_currentItem)
{
    ImGui::ListBox(
            _("Choose File"),
        t_currentItem,
        vector_getter,
        &t_files,
        static_cast<int32_t>(t_files.size())
    );
}

//-------------------------------------------------
// ImGui helper
//-------------------------------------------------

bool mdcii::vector_getter(void* t_vec, const int32_t t_index, const char** t_outText)
{
    const auto &vector{ *static_cast<std::vector<resource::MdciiFile>*>(t_vec) };
    if (t_index < 0 || t_index >= static_cast<int>(vector.size()))
    {
        return false;
    }

    *t_outText = vector.at(t_index).GetFileName().c_str();

    return true;
}
