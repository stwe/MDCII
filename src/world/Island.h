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

namespace mdcii::world
{
    /**
     * @brief Forward declaration struct Tile.
     */
    struct Tile;

    /**
     * @brief Forward declaration class Layer.
     */
    class Layer;

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
        // Constants
        //-------------------------------------------------

        /**
         * @brief Number of layers used here.
         */
        static constexpr auto NR_OF_LAYERS{ 4 };

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
         * @brief The tile layers (coast, terrain, buildings, mixed) of the island.
         */
        std::array<std::unique_ptr<Layer>, NR_OF_LAYERS> layers;

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
         * @brief Initializes the island.
         */
        void Init();

    protected:

    private:
        //-------------------------------------------------
        // Layer
        //-------------------------------------------------

        /**
         * @brief Initialises the COAST, TERRAIN and BUILDINGS layer of the island.
         */
        void InitLayers();

        /**
         * @brief Initialises the mixed layer of the island, which is created from all other layers.
         */
        void InitMixedLayer();

        /**
         * @brief Populates the mixed layer with data from other layers.
         *
         * @param t_coastLayer Coast layer of the island.
         * @param t_terrainLayer Terrain layer of the island.
         * @param t_buildingsLayer Buildings layer of the island.
         * @param t_mixedLayer The mixed layer to be populated.
         * @param t_h The height index.
         * @param t_w The width index.
         */
        void PopulateMixedLayer(const Layer* t_coastLayer, const Layer* t_terrainLayer, const Layer* t_buildingsLayer, Layer* t_mixedLayer, int t_w, int t_h) const;

        /**
         * @brief Checks if a tile should be replaced in the mixed layer.
         *
         * @param t_layer Layer to check the tile from.
         * @param t_buildingsLayer Buildings layer of the island.
         * @param t_index Index of the tile to check.
         *
         * @return True if the tile should be replaced; false otherwise.
         */
        static bool ShouldReplaceTile(const Layer* t_layer, const Layer* t_buildingsLayer, int t_index);

        //-------------------------------------------------
        // Layer tiles
        //-------------------------------------------------

        /**
         * @brief Initializes all layer tiles.
         *
         * @param t_layer Pointer to the Layer.
         */
        void InitLayerTiles(Layer* t_layer) const;

        /**
         * @brief Initializes the details of a single tile.
         *
         * @param t_layer Current layer being initialized.
         * @param t_w Width (x) coordinate of the tile.
         * @param t_h Height (y) coordinate of the tile.
         */
        void InitTileDetails(Layer* t_layer, int t_w, int t_h) const;

        /**
         * @brief Initializes the details of a tile with a building.
         *
         * @param t_tile The tile to be initialized.
         */
        void InitBuildingTileDetails(Tile* t_tile) const;

        /**
         * @brief Adjusts the graphical representation of a tile for large buildings.
         *
         * @param t_tile The tile to be adjusted.
         * @param t_gfx The graphical representation of the tile.
         */
        void AdjustGfxForBigBuildings(const Tile* t_tile, int& t_gfx) const;

        /**
         * @brief Sorts the tiles in a layer for correct rendering order.
         *
         * @param t_layer Pointer to the layer whose tiles are to be sorted.
         */
        void SortTiles(Layer* t_layer) const;
    };
}
