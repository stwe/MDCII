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

#include <vector>
#include <array>

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::resource
{
    /**
     * @brief Forward declaration struct Building.
     */
    struct Building;
}

namespace mdcii::world
{
    //-------------------------------------------------
    // Tile
    //-------------------------------------------------

    /**
     * @brief Represents a Tile.
     */
    struct Tile
    {
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * @brief The total number of rotations.
         *
         * For better readability.
         */
        static constexpr auto NR_OF_ROTATIONS{ 4 };

        /**
         * @brief The number of different animation times.
         */
        static constexpr auto NR_OF_ANIM_TIMES{ 5 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the Building object generated from the haeuser.cod file.
         */
        const resource::Building* building{ nullptr };

        /**
         * @brief The Bsh graphic for each rotation.
         *
         * In some cases the same gfx is used for all rotations.
         */
        std::vector<int> gfxs{};

        /**
         * @brief The rotation of the building.
         */
        int rotation{ 0 };

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
         * @brief The x position of the tile in local/object space of the building.
         */
        int x{ 0 };

        /**
         * @brief The y position of the tile in local/object space of the building.
         */
        int y{ 0 };

        /**
         * @brief Represents the x position for DEG0 of a tile relative to its parent island.
         *
         * This indicates the local position of the tile on the island, not its global world position.
         * For example, if the island starts at position (8, 5) in the world, the first tile will be at posX 0.
         *
         * However, for deep water areas, posX always represents the world position.
         */
        int posX{ -1 };

        /**
         * @brief Represents the y position for DEG0 of a tile relative to its parent island.
         *
         * This indicates the local position of the tile on the island, not its global world position.
         * For example, if the island starts at position (8, 5) in the world, the first tile will be at posY 0.
         *
         * However, for deep water areas, posY always represents the world position.
         */
        int posY{ -1 };

        /**
         * @brief The index for each rotation is needed for sorting.
         */
        std::array<int, NR_OF_ROTATIONS> indices{};

        /**
         * @brief Used to get the current gfx for animation.
         */
        int frame{ 0 };

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * @brief For better readability/convenience.
         *
         * @return True if a Building object is present.
         */
        [[nodiscard]] bool HasBuilding() const { return building != nullptr; }

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * @brief Set the current animation frame.
         *
         * @param t_frameValues The current frame values for all animation times.
         */
        void UpdateFrame(const std::array<int, NR_OF_ANIM_TIMES>& t_frameValues);
    };
}
