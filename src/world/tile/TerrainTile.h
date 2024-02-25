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

#include "Tile.h"
#include "vendor/nlohmann/json.hpp"

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

namespace mdcii::world::tile
{
    //-------------------------------------------------
    // TerrainTile
    //-------------------------------------------------

    /**
     * @brief Represents a TerrainTile.
     *
     * These tiles make up the deep water area and all islands.
     */
    struct TerrainTile : public Tile
    {
        /**
         * @brief Possible tile neighbors. Each flag can set by using the OR operator.
         */
        enum NeighborFlag : uint8_t
        {
            NORTH = 1,
            EAST = 2,
            SOUTH = 4,
            WEST = 8
        };

        /**
         * @brief The value corresponds to the index in the texture atlas.
         */
        enum class NeighborType
        {
            NEIGHBOR_NONE = -1,
            NEIGHBOR_V = 0,
            NEIGHBOR_H = 1,
            NEIGHBOR_C1 = 4,
            NEIGHBOR_T1 = 5,
            NEIGHBOR_C2 = 6,
            NEIGHBOR_T2 = 8,
            NEIGHBOR_X = 9,
            NEIGHBOR_T3 = 10,
            NEIGHBOR_C3 = 12,
            NEIGHBOR_T4 = 13,
            NEIGHBOR_C4 = 14,
        };

        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * @brief The number of possible animation times.
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
         * @brief The type of a possible neighbor.
         */
        NeighborType neighborType{ NeighborType::NEIGHBOR_NONE };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        TerrainTile();

        /**
         * @brief Constructs a new TerrainTile object.
         *
         * @param t_building Pointer to the Building object generated from the haeuser.cod file.
         * @param t_rotation The rotation of the tile/building.
         * @param t_x The x position of the tile in local/object space of the building.
         * @param t_y The y position of the tile in local/object space of the building.
         * @param t_posX Represents the x position for DEG0 of a tile relative to its parent island.
         * @param t_posY Represents the y position for DEG0 of a tile relative to its parent island.
         */
        TerrainTile(const resource::Building* t_building, int t_rotation, int t_x, int t_y, int t_posX, int t_posY);

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * @brief For better readability/convenience.
         *
         * @return True if a Building object is present.
         */
        [[nodiscard]] bool HasBuilding() const { return building != nullptr; }

        /**
         * @brief For better readability/convenience.
         *
         * @return True if a Building object is present and is above water and coast.
         */
        [[nodiscard]] bool HasBuildingAboveWaterAndCoast() const;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * @brief Set the current animation frame.
         *
         * @param t_frameValues The current frame values for all animation times.
         */
        void UpdateFrame(const std::array<int, NR_OF_ANIM_TIMES>& t_frameValues);

        /**
         * @brief Renders some information about this tile using ImGui.
         */
        void RenderImGui() const;

        //-------------------------------------------------
        // Gfx
        //-------------------------------------------------

        /**
         * @brief Calculates a gfx for each rotation.
         */
        void CalculateGfx();

        /**
         * @brief Adjusts gfx values for large buildings.
         *
         * @param t_gfx The gfx of the terrain tile.
         */
        void AdjustGfxForBigBuildings(int& t_gfx) const;

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        /**
         * @brief The tile cannot be rendered if there is no building.
         *
         * @return True if there is no building; otherwise false.
         */
        [[nodiscard]] bool IsNotRenderable() const override { return !HasBuilding(); }

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * @brief Determines the correct NeighborType for this Tile depending on the neighbors.
         *
         * @return True if the type has changed.
         */
        [[nodiscard]] bool DetermineNeighborType();
    };

    //-------------------------------------------------
    // Serializing TerrainTile into Json
    //-------------------------------------------------

    void to_json(nlohmann::json& t_json, const TerrainTile& t_tile);
}
