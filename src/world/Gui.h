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

#include <vector>
#include <string>

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii
{
    /**
     * @brief Forward declaration class Game.
     */
    class Game;
}

namespace mdcii::resource
{
    /**
     * @brief Forward declaration struct Building.
     */
    struct Building;
}

namespace mdcii::world
{
    /**
     * @brief Forward declaration enum class Rotation.
     */
    enum class Rotation;

    /**
     * @brief Forward declaration class World.
     */
    class World;

    //-------------------------------------------------
    // SelectBuilding
    //-------------------------------------------------

    /**
     * @brief A building selected in the Gui in a specific rotation.
     */
    struct SelectBuilding
    {
        const resource::Building* building;
        Rotation rotation;
    };

    //-------------------------------------------------
    // Gui
    //-------------------------------------------------

    /**
     * @brief Renders ImGui menus.
     */
    class Gui
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Currently selected building in the Gui.
         */
        static SelectBuilding select_building;

        //-------------------------------------------------
        // ImGui
        //-------------------------------------------------

        /**
         * @brief Renders an ImGui menu to select buildings.
         *
         * @param t_game Pointer to the parent Game object.
         */
        static void RenderAddBuildingsGui(const Game* t_game);

        /**
         * @brief Shows ImGui elements to rotate a selected building.
         *
         * @param t_building The building to be rotated.
         */
        static void RotatableBuildingGui(const resource::Building* t_building);

        /**
         * @brief Renders an ImGui menu to save the game.
         *
         * @param t_world Pointer to the World object.
         */
        static void SaveGameGui(const World* t_world);

    protected:

    private:

        /**
         * @brief Renders the buildings for a specific title.
         *
         * @param t_game Pointer to the parent Game object.
         * @param t_title The title of the buildings.
         * @param t_buildingIds The IDs of the buildings.
         * @param t_buildingNames The names of the buildings.
         */
        static void RenderBuildingsByTitleGui(
            const Game* t_game,
            const std::string& t_title,
            const std::vector<int>& t_buildingIds,
            const std::vector<std::string_view>& t_buildingNames
        );
    };
}
