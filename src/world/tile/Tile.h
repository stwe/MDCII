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

namespace mdcii::world
{
    /**
     * @brief Forward declaration enum class Rotation.
     */
    enum class Rotation;
}

namespace mdcii::world::tile
{
    //-------------------------------------------------
    // Tile
    //-------------------------------------------------

    /**
     * @brief Represents a Tile.
     *
     * The base class of all Tile objects.
     */
    struct Tile
    {
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The rotation  of the tile.
         *
         * There are rotations for the terrain (islands, deep water): [DEG0, DEG90, DEG180, DEG270].
         * Figures use additional rotations.
         */
        int rotation{ 0 };

        /**
         * @brief Represents the x position for the first rotation (e.g. DEG0) of a tile relative to its parent island.
         *
         * This indicates the local position of the tile on the island, not its global world position.
         * For deep water areas, posX always represents the world position.
         */
        int posX{ -1 };

        /**
         * @brief Represents the y position for the first rotation (e.g. DEG0) of a tile relative to its parent island.
         *
         * This indicates the local position of the tile on the island, not its global world position.
         * For deep water areas, posY always represents the world position.
         */
        int posY{ -1 };

        /**
         * @brief The gfx for each tile rotation.
         *
         * In some cases the same gfx is used for all rotations.
         * There are rotations for the terrain (islands, deep water): [DEG0, DEG90, DEG180, DEG270].
         * Figures may use additional rotations.
         */
        std::vector<int> gfxs{};

        /**
         * @brief The render index for each rotation.
         *
         * The tiles with their gfx are always rendered from top right to bottom left.
         * This index is responsible for sorting the tile in each rotation.
         * The values are calculated using the GetRenderIndex() method.
         */
        std::vector<int> renderIndices{};

        /**
         * @brief Used to get the current gfx for animation.
         */
        int frame{ 0 };

        /**
         * @brief Can be used e.g. as a flag to highlight the tile.
         */
        int tintFlag{ -1 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Tile();

        /**
         * @brief Constructs a new Tile object.
         *
         * @param t_rotation The rotation of the tile.
         * @param t_posX The x position for the first rotation (e.g. DEG0) of a tile relative to its parent island.
         * @param t_posY The y position for the first rotation (e.g. DEG0) of a tile relative to its parent island.
         */
        Tile(int t_rotation, int t_posX, int t_posY);

        virtual ~Tile() noexcept;

        //-------------------------------------------------
        // Render index
        //-------------------------------------------------

        /**
         * @brief 2D/1D - mapping of a position.
         *
         * The method determines a value for a specific position and rotation, which is used
         * to sort the tile for rendering. The value can also be negative.
         *
         * @param t_x The 2D x position.
         * @param t_y The 2D y position.
         * @param t_width The width of the 2D array.
         * @param t_height The height of the 2D array.
         * @param t_rotation The given position is previously rotated by the specified rotation.
         *
         * @return The 1D index.
         */
        [[nodiscard]] static int GetRenderIndex(int t_x, int t_y, int t_width, int t_height, Rotation t_rotation);

        /**
         * @brief Calculates the render index for the tile position for all rotation directions.
         *
         * @param t_width The width of the 2D array.
         * @param t_height The height of the 2D array.
         */
        void SetRenderIndices(int t_width, int t_height);
    };
}