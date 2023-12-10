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
#include "vendor/enum/magic_enum.hpp"

namespace mdcii::world
{
    //-------------------------------------------------
    // Layer type
    //-------------------------------------------------

    /**
     * @brief Enum class representing different types of layers.
     */
    enum class LayerType
    {
        COAST, TERRAIN, BUILDINGS, MIXED, DEEP_WATER, NONE
    };

    //-------------------------------------------------
    // Layer
    //-------------------------------------------------

    /**
     * @brief Class representing a layer.
     */
    class Layer
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The type the layer.
         */
        LayerType layerType{ LayerType::NONE };

        /**
         * @brief A vector holding all tiles belonging to this layer.
         */
        std::vector<Tile> tiles;

        /**
         * @brief An array of std::vectors, each representing a rotation of the tiles.
         *
         * Tiles are sorted by index for each rotation.
         */
        std::array<std::vector<Tile>, Tile::NR_OF_ROTATIONS> sortedTiles;

        /**
         * @brief The tiles that are currently being rendered.
         */
        std::vector<Tile> currentTiles;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Layer() = delete;

        /**
         * @brief Constructs a new Layer object.
         *
         * @param t_layerType The type of the layer.
         */
        explicit Layer(LayerType t_layerType);

        Layer(const Layer& t_other) = delete;
        Layer(Layer&& t_other) noexcept = delete;
        Layer& operator=(const Layer& t_other) = delete;
        Layer& operator=(Layer&& t_other) noexcept = delete;

        ~Layer() noexcept;

        //-------------------------------------------------
        // Sort
        //-------------------------------------------------

        /**
         * @brief Sorts the tiles for correct rendering order.
         *
         * The sorted tiles are stored in `sortedTiles`.
         */
        void SortTilesForRendering();

    protected:

    private:

    };
}
