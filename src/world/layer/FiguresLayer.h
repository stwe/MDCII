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

#include "Layer.h"
#include "world/Rotation.h"
#include "world/tile/FigureTile.h"

namespace mdcii::world::layer
{
    //-------------------------------------------------
    // FiguresLayer
    //-------------------------------------------------

    /**
     * @brief Represents a FiguresLayer.
     *
     * Creates all figures.
     */
    class FiguresLayer : public Layer
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief A vector holding all tiles belonging to this layer.
         */
        std::vector<tile::FigureTile> tiles;

        /**
         * @brief Contains the tiles for each rotation in the correct order for rendering.
         */
        std::array<std::vector<tile::FigureTile>, magic_enum::enum_count<Rotation>()> sortedTiles;

        /**
         * @brief To get the correct position of a tile in `sortedTiles` from rotation and render index.
         */
        std::array<std::map<int, int>, magic_enum::enum_count<Rotation>()> sortedIndices;

        /**
         * @brief The tiles that are currently being rendered.
         */
        std::vector<tile::FigureTile> currentTiles;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        FiguresLayer() = delete;

        /**
         * @brief Constructs a new FiguresLayer object.
         *
         * @param t_world Pointer to the parent World object.
         * @param t_width The width of the layer.
         * @param t_height The height of the layer.
         */
        FiguresLayer(const World* t_world, int t_width, int t_height);

        FiguresLayer(const FiguresLayer& t_other) = delete;
        FiguresLayer(FiguresLayer&& t_other) noexcept = delete;
        FiguresLayer& operator=(const FiguresLayer& t_other) = delete;
        FiguresLayer& operator=(FiguresLayer&& t_other) noexcept = delete;

        ~FiguresLayer() noexcept override;

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        void CreateLayerTiles(const nlohmann::json& t_json) override;
        void InitLayerTiles() override;

        //-------------------------------------------------
        // Create Tile
        //-------------------------------------------------

        tile::FigureTile CreateLayerTile(const nlohmann::json& t_json);

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
