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
#include <magic_enum.hpp>
#include "Text.h"
#include "Game.h"
#include "Log.h"
#include "MdciiException.h"

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::data::Text::Init()
{
    Log::MDCII_LOG_DEBUG("[Text::Init()] Start initializing the Text class...");

    if (m_buildings.empty())
    {
        ReadTexts();
    }

    if (m_menus.empty())
    {
        ReadMenus();
    }

    Log::MDCII_LOG_DEBUG("[Text::Init()] Text class successfully initialized.");
}

//-------------------------------------------------
// Read
//-------------------------------------------------

void mdcii::data::Text::ReadTexts()
{
    Log::MDCII_LOG_DEBUG("[Text::ReadTexts()] Start reading Json data...");

    nlohmann::json j = ReadJsonFromFile(Game::RESOURCES_REL_PATH + "data/Texts.json");

    for (const auto& [s, v] : j.items())
    {
        // e.g.: s = WORKSHOPS
        auto section{ magic_enum::enum_cast<Section>(s) };
        if (section.has_value())
        {
            // e.g.: l = de
            for (const auto& [l, o] : v.items())
            {
                std::map<std::string, std::string> e;

                for (const auto& [buildingId, text] : o.items())
                {
                    e.emplace(buildingId, text);
                }

                m_buildings.emplace(
                    std::make_pair(section.value(), l),
                    e
                );
            }
        }
    }

    Log::MDCII_LOG_DEBUG("[Text::ReadTexts()] Json data read successfully.");
}

void mdcii::data::Text::ReadMenus()
{
    Log::MDCII_LOG_DEBUG("[Text::ReadMenus()] Start reading Json data...");

    nlohmann::json j = ReadJsonFromFile(Game::RESOURCES_REL_PATH + "data/Menus.json");

    for (const auto& [l, o] : j.items())
    {
        std::map<std::string, std::string> e;

        for (const auto& [k, v] : o.items())
        {
            e.emplace(k, v);
        }

        m_menus.emplace(l, e);
    }

    Log::MDCII_LOG_DEBUG("[Text::ReadMenus()] Json data read successfully.");
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

nlohmann::json mdcii::data::Text::ReadJsonFromFile(const std::string& t_filePath)
{
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
        throw MDCII_EXCEPTION("[Text::ReadJsonFromFile()] Exception caught while loading file " + t_filePath + ".");
    }

    return j;
}
