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

#include <fstream>
#include <filesystem>
#include <imgui.h>
#include "MdciiUtils.h"
#include "Game.h"
#include "MdciiException.h"
#include "Log.h"

//-------------------------------------------------
// Files
//-------------------------------------------------

nlohmann::json mdcii::read_json_from_file(const std::string& t_filePath)
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

std::vector<std::string> mdcii::get_files_list(const std::string& t_relPath, const std::string& t_extension)
{
    std::vector<std::string> results;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(Game::RESOURCES_REL_PATH + t_relPath, std::filesystem::directory_options::follow_directory_symlink))
    {
        if (is_regular_file(entry.status()) && entry.path().extension().string() == t_extension)
        {
            results.push_back(entry.path().string());
        }
    }

    return results;
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

void mdcii::toggle_imgui_button(const std::string& t_strId, bool* t_v)
{
    auto const* colors{ ImGui::GetStyle().Colors };
    auto pos{ ImGui::GetCursorScreenPos() };
    auto* drawList{ ImGui::GetWindowDrawList() };

    auto height{ ImGui::GetFrameHeight() };
    auto width{ height * 1.55f };
    auto radius{ height * 0.5f };

    ImGui::InvisibleButton(t_strId.c_str(), ImVec2(width, height));

    if (ImGui::IsItemClicked())
    {
        *t_v = !*t_v;
    }

    drawList->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), ImGui::GetColorU32(colors[ImGuiCol_ButtonActive]), height * 0.50f);
    drawList->AddCircleFilled(ImVec2(pos.x + radius + (*t_v ? 1.0f : 0.0f) * (width - radius * 2.0f), pos.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}

//-------------------------------------------------
// ImGui helper
//-------------------------------------------------

bool mdcii::vector_getter(void* t_vec, const int32_t t_index, const char** t_outText)
{
    auto &vector{ *static_cast<std::vector<std::string>*>(t_vec) };
    if (t_index < 0 || t_index >= static_cast<int>(vector.size()))
    {
        return false;
    }

    *t_outText = vector.at(t_index).c_str();

    return true;
}