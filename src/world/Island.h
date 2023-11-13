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
     * @brief Forward declaration class Layer.
     */
    class Layer;

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
         * @brief The tile layers (coast, terrain, buildings) of the island.
         */
        std::array<std::unique_ptr<Layer>, 3> layers;

        /**
         * An Aabb (axis-aligned bounding box) object for collision detection.
         */
        physics::Aabb aabb;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Island();

        Island(const Island& t_other) = delete;
        Island(Island&& t_other) noexcept = delete;
        Island& operator=(const Island& t_other) = delete;
        Island& operator=(Island&& t_other) noexcept = delete;

        ~Island() noexcept;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * @brief Initializes all layers.
         *
         * @param t_game Pointer to the Game.
         */
        void Init(const Game* t_game);

    protected:

    private:
        /**
         * @brief Initializes all layer tiles.
         *
         * @param t_game Pointer to the Game.
         * @param t_layer Pointer to the Layer.
         */
        void InitLayerTiles(const Game* t_game, Layer* t_layer) const;
    };
}
