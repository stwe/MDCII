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

#include "Tile.h"
#include "physics/Aabb.h"

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

namespace mdcii::world
{
    //-------------------------------------------------
    // Island
    //-------------------------------------------------

    /**
     * @brief Represents an Island.
     */
    class Island
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The width of the island.
         */
        int width{ -1 };

        /**
         * @brief The height of the island.
         */
        int height{ -1 };

        /**
         * @brief The X-coordinate of the island's initial position in the world.
         */
        int startX{ -1 };

        /**
         * @brief The Y-coordinate of the island's initial position in the world.
         */
        int startY{ -1 };

        /**
         * @brief The tiles of the island.
         */
        std::vector<Tile> tiles;

        /**
         * @brief The tiles of the island, sorted by index for each rotation.
         */
        std::array<std::vector<Tile>, Tile::NR_OF_ROTATIONS> sortedTiles;

        /**
         * @brief The tiles that are currently being rendered.
         */
        std::vector<Tile> currentTiles;

        /**
         * An Aabb (axis-aligned bounding box) object for collision detection.
         */
        physics::Aabb aabb;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * @brief Initializes all tiles of the island.
         *
         * @param t_game Pointer to the Game.
         */
        void InitTiles(const Game* t_game);

    protected:

    private:

    };
}
