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
     * @brief Forward declaration struct Tile.
     */
    struct Tile;

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
         * @brief The tile layers (coast, terrain, buildings, terrain_buildings) of the island.
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
         * @brief Initializes all layers.
         *
         * @param t_game Pointer to the Game.
         */
        void Init(const Game* t_game);

    protected:

    private:
        //-------------------------------------------------
        // Init layer tiles
        //-------------------------------------------------

        /**
         * @brief Initializes all layer tiles.
         *
         * @param t_game Pointer to the Game.
         * @param t_layer Pointer to the Layer.
         */
        void InitLayerTiles(const Game* t_game, Layer* t_layer) const;

        /**
         * @brief Initializes the details of a single tile.
         *
         * @param t_game Pointer to a Game object.
         * @param t_layer Current layer being initialized.
         * @param t_w Width (x) coordinate of the tile.
         * @param t_h Height (y) coordinate of the tile.
         */
        void InitTileDetails(const Game* t_game, Layer* t_layer, int t_w, int t_h) const;

        /**
         * @brief Initializes the details of a tile with a building.
         *
         * @param t_game Pointer to a Game object.
         * @param t_tile The tile to be initialized.
         */
        static void InitBuildingTileDetails(const Game* t_game, Tile* t_tile);

        /**
         * @brief Adjusts the graphical representation of a tile for large buildings.
         *
         * @param t_building The building on this tile.
         * @param t_tile The tile to be adjusted.
         * @param t_gfx The graphical representation of the tile.
         */
        static void AdjustGfxForBigBuildings(const resource::Building* t_building, const Tile* t_tile, int& t_gfx);

        /**
         * @brief Sorts the tiles in a layer for correct rendering order.
         *
         * @param t_layer Pointer to the layer whose tiles are to be sorted.
         */
        static void SortTiles(Layer* t_layer);
    };
}
