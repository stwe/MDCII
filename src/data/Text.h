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
#include <map>
#include "json.hpp"

namespace mdcii::data
{
    class Text
    {
    public:
        using language = std::string;
        using key = std::string;
        using translation = std::string;

        //-------------------------------------------------
        // Sections
        //-------------------------------------------------

        enum class Section
        {
            PUBLIC_BUILDINGS,
            FARMS,
            WORKSHOPS,
            WATER_RELATED,
            MILITARY
        };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Text(const Text& t_other) = delete;
        Text(Text&& t_other) noexcept = delete;
        Text& operator=(const Text& t_other) = delete;
        Text& operator=(Text&& t_other) noexcept = delete;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        static void Init();

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        static const std::map<key, translation>& GetBuildingsTexts(const Section t_section, const language& t_language)
        {
            return m_buildings.find({ t_section, t_language })->second;
        }

        static const translation& GetMenuText(const language& t_language, const key& t_key)
        {
            return m_menus.find(t_language)->second.at(t_key);
        }

        static std::string GetTextForBuildingId(const Section t_section, const int t_buildingId, const language& t_language)
        {
            if (GetBuildingsTexts(t_section, t_language).count(std::to_string(t_buildingId)))
            {
                return GetBuildingsTexts(t_section, t_language).at(std::to_string(t_buildingId));
            }

            return { "Translation is missing." };
        }

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        inline static std::multimap<std::pair<Section, language>, std::map<key, translation>> m_buildings;
        inline static std::multimap<language, std::map<key, translation>> m_menus;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Text() = default;
        ~Text() noexcept = default;

        //-------------------------------------------------
        // Read
        //-------------------------------------------------

        static void ReadTexts();
        static void ReadMenus();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        [[nodiscard]] static nlohmann::json ReadJsonFromFile(const std::string& t_filePath);
    };
}
