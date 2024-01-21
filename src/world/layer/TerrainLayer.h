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
#include "world/tile/TerrainTile.h"

namespace mdcii::world::layer
{
    //-------------------------------------------------
    // TerrainLayer
    //-------------------------------------------------

    /**
     * @brief Represents a TerrainLayer.
     *
     * These layers are used to create the deep water area and different parts of the islands.
     */
    class TerrainLayer : public Layer
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief A vector holding all tiles belonging to this layer.
         */
        std::vector<tile::TerrainTile> tiles;

        /**
         * @brief Contains the tiles for each rotation in the correct order for rendering.
         */
        std::array<std::vector<tile::TerrainTile>, magic_enum::enum_count<Rotation>()> sortedTiles;

        /**
         * @brief To get the correct position of a tile in `sortedTiles` from rotation and render index.
         */
        std::array<std::map<int, int>, magic_enum::enum_count<Rotation>()> sortedIndices;

        /**
         * @brief The tiles that are currently being rendered.
         */
        std::vector<tile::TerrainTile> currentTiles;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        TerrainLayer() = delete;

        /**
         * @brief Constructs a new TerrainLayer object.
         *
         * @param t_world Pointer to the parent World object.
         * @param t_layerType The type of the layer.
         * @param t_width The width of the layer.
         * @param t_height The height of the layer.
         */
        TerrainLayer(const World* t_world, LayerType t_layerType, int t_width, int t_height);

        TerrainLayer(const TerrainLayer& t_other) = delete;
        TerrainLayer(TerrainLayer&& t_other) noexcept = delete;
        TerrainLayer& operator=(const TerrainLayer& t_other) = delete;
        TerrainLayer& operator=(TerrainLayer&& t_other) noexcept = delete;

        ~TerrainLayer() noexcept override;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * @brief Retrieves a reference to the tile at the specified position and rotation from `sortedTiles`.
         *
         * @param t_x The x position on the layer.
         * @param t_y The y position on the layer.
         * @param t_rotation The current camera rotation.
         *
         * @return A reference to the tile.
         */
        [[nodiscard]] tile::TerrainTile& GetSortedTile(int t_x, int t_y, Rotation t_rotation);

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        void CreateLayerTiles(const nlohmann::json& t_json) override;

        //-------------------------------------------------
        // Create Tile
        //-------------------------------------------------

        tile::TerrainTile CreateLayerTile(const nlohmann::json& t_json);

        //-------------------------------------------------
        // Init Tiles
        //-------------------------------------------------

        void InitLayerTiles();
        void InitLayerTile(int t_x, int t_y);

        //-------------------------------------------------
        // Add Tiles
        //-------------------------------------------------

        /**
         * @brief Adds a building to the layer.
         *
         * @param t_buildingTiles The tiles of the building to be added.
         */
        void AddBuilding(std::vector<tile::TerrainTile>& t_buildingTiles);

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
