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

        Text();

        Text(const Text& t_other) = delete;
        Text(Text&& t_other) noexcept = delete;
        Text& operator=(const Text& t_other) = delete;
        Text& operator=(Text&& t_other) noexcept = delete;

        ~Text() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] const std::map<key, translation>& GetBuildingsTexts(Section t_section, const language& t_language) const
        {
            return m_buildings.find({ t_section, t_language })->second;
        }

        [[nodiscard]] const translation& GetMenuText(const language& t_language, const key& t_key) const
        {
            return m_menus.find(t_language)->second.at(t_key);
        }

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        std::multimap<std::pair<Section, language>, std::map<key, translation>> m_buildings;
        std::multimap<language, std::map<key, translation>> m_menus;

        //-------------------------------------------------
        // Read
        //-------------------------------------------------

        void ReadTexts();
        void ReadMenus();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        [[nodiscard]] static nlohmann::json ReadJsonFromFile(const std::string& t_filePath);
    };
}
