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

#include "ecs/entt.hpp"
#include "Rotation.h"
#include "Zoom.h"

//-------------------------------------------------
// MapTile
//-------------------------------------------------

namespace mdcii::map
{
    /**
     * MapTile represents a single part of a layer.
     */
    struct MapTile
    {
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The building Id from the haeuser.cod.
         */
        int32_t buildingId{ -1 };

        /**
         * The rotation of the building.
         */
        Rotation rotation{ Rotation::DEG0 };

        /**
         * Example: Bakery
         *     width = 2
         *     height = 2
         *
         *     object space:
         *     -------------------------------
         *     | x = 0, y = 0 | x = 1, y = 0 |
         *     -------------------------------
         *     | x = 0, y = 1 | x = 1, y = 1 |
         *     -------------------------------
         */

        /**
         * The x position of the tile in local/object space of the building.
         */
        int32_t x{ 0 };

        /**
         * The y position of the tile in local/object space of the building.
         */
        int32_t y{ 0 };

        /**
         * The x position on the world map.
         */
        int32_t mapX{ -1 };

        /**
         * The y position on the world map.
         */
        int32_t mapY{ -1 };

        /**
         * A screen position for each zoom and each rotation (world space positions).
         */
        std::array<std::array<glm::vec2, NR_OF_ROTATIONS>, NR_OF_ZOOMS> screenPositions{};

        /**
         * The index for each rotation is needed for sorting.
         */
        std::array<int32_t, NR_OF_ROTATIONS> indices{};

        /**
         * The Bsh graphic for each rotation.
         * In some cases the same gfx is used for all rotations.
         */
        std::vector<int32_t> gfxs{};

        /**
         * Stores the entity handle created for this tile.
         */
        entt::entity entity{ entt::null };

        /**
         * Stores another entity handle created for this tile.
         * This is needed to show a grid for debugging purposes.
         */
        entt::entity gridEntity{ entt::null };

        /**
         * If a building requires more than 1x1 tiles, all indices are stored here.
         */
        std::vector<int32_t> connectedMapTiles;

        /**
         * For better readability/convenience.
         *
         * @return True if a valid building Id is present.
         */
        [[nodiscard]] bool HasBuilding() const { return buildingId >= 0; }

        //-------------------------------------------------
        // Render
        //-------------------------------------------------

        /**
         * Renders ImGui menus.
         *
         * @param t_heading Shows the heading.
         */
        void RenderImGui(bool t_heading = true) const;
    };
}
