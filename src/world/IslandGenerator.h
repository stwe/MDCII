// This file is part of the MDCII project.
//
// Copyright (c) 2022. stwe <https://github.com/stwe/MDCII>
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

#include <unordered_set>
#include <magic_enum.hpp>
#include "data/json.hpp"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::layer
{
    /**
     * Forward declaration struct Tile.
     */
    struct Tile;
}

//-------------------------------------------------
// IslandGenerator
//-------------------------------------------------

namespace mdcii::world
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * Forward declaration enum class Rotation.
     */
    enum class Rotation;

    //-------------------------------------------------
    // IslandGenerator
    //-------------------------------------------------

    class IslandGenerator
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        IslandGenerator();

        IslandGenerator(const IslandGenerator& t_other) = delete;
        IslandGenerator(IslandGenerator&& t_other) noexcept = delete;
        IslandGenerator& operator=(const IslandGenerator& t_other) = delete;
        IslandGenerator& operator=(IslandGenerator&& t_other) noexcept = delete;

        ~IslandGenerator() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void RenderImGui();

    protected:

    private:
        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        /**
         * For the rough classification of the map elevation values.
         */
        enum class MapType
        {
            WATER,
            SHALLOW_WATER,
            COAST,
            EMBANKMENT,
            TERRAIN,
            INVALID
        };

        /**
         * To calculate the bitmask.
         */
        enum NeighborFlag
        {
            // -----------------------------
            TERRAIN_FLAG = 0,

            NORTH_WEST_FLAG = 1,
            NORTH_FLAG = 2,
            NORTH_EAST_FLAG = 4,

            WEST_FLAG = 8,
            EAST_FLAG = 16,

            SOUTH_WEST_FLAG = 32,
            SOUTH_FLAG = 64,
            SOUTH_EAST_FLAG = 128,
            // ----------------------------- = 255

            EMBANKMENT_FLAG = 256,
            COAST_FLAG = 512,
            SHALLOW_FLAG = 768
        };

        struct MapTypeCol
        {
            MapType mapType;
            ImU32 color;
        };

        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * Elevation values above this value are used as terrain, all others as water.
         */
        static constexpr auto WATER_LEVEL{ 0.5 };

        // ImGui colors

        static constexpr ImU32 TERRAIN_COL{ IM_COL32(0, 255, 0, 255) };
        static constexpr ImU32 TERRAIN_BORDER_COL{ IM_COL32(22, 227, 15, 255) };
        static constexpr ImU32 EMBANKMENT_COL{ IM_COL32(255, 255, 0, 255) };
        static constexpr ImU32 COAST_COL{ IM_COL32(80, 192, 248, 255) };
        static constexpr ImU32 SHALLOW_WATER_COL{ IM_COL32(7, 69, 235, 255) };
        static constexpr ImU32 WATER_COL{ IM_COL32(0, 0, 255, 255) };
        static constexpr ImU32 INVALID_COL{ IM_COL32(255, 0, 0, 255) };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * Generate trees for an island in the south or north.
         */
        inline static bool m_south{ false };

        /**
         * The width of the island.
         */
        inline static int32_t m_width{ 33 };

        /**
         * The height of the island.
         */
        inline static int32_t m_height{ 30 };

        /**
         * Stores a map type (water or terrain) for each position.
         */
        std::vector<MapType> m_map;

        /**
         * Stores a bitmask value for each position.
         */
        std::vector<int32_t> m_bitmaskValues;

        /**
         * A helper flag that controls the rendering of ImGui menus.
         */
        bool m_render{ true };

        //-------------------------------------------------
        // Noise
        //-------------------------------------------------

        void CalcMapTypes(int32_t t_seed, float t_frequency, int32_t t_width, int32_t t_height);
        void CalcBitmaskValues(const std::vector<MapType>& t_map, std::vector<int32_t>& t_bitmaskValues);

        //-------------------------------------------------
        // Bitmasking
        //-------------------------------------------------

        static int32_t GetNorthValue(const std::vector<MapType>& t_map, int32_t t_x, int32_t t_y, MapType t_mapType);
        static int32_t GetEastValue(const std::vector<MapType>& t_map, int32_t t_x, int32_t t_y, MapType t_mapType);
        static int32_t GetSouthValue(const std::vector<MapType>& t_map, int32_t t_x, int32_t t_y, MapType t_mapType);
        static int32_t GetWestValue(const std::vector<MapType>& t_map, int32_t t_x, int32_t t_y, MapType t_mapType);

        static int32_t GetNorthWestValue(const std::vector<MapType>& t_map, int32_t t_x, int32_t t_y, MapType t_mapType);
        static int32_t GetNorthEastValue(const std::vector<MapType>& t_map, int32_t t_x, int32_t t_y, MapType t_mapType);
        static int32_t GetSouthWestValue(const std::vector<MapType>& t_map, int32_t t_x, int32_t t_y, MapType t_mapType);
        static int32_t GetSouthEastValue(const std::vector<MapType>& t_map, int32_t t_x, int32_t t_y, MapType t_mapType);

        //-------------------------------------------------
        // ImGui
        //-------------------------------------------------

        /**
         * Shows the given map values.
         */
        void RenderMapTypesImGui(const std::vector<MapType>& t_map, const std::string& t_title) const;

        /**
         * Shows the given bitmask values.
         */
        void RenderBitmaskValuesImGui(const std::vector<int32_t>& t_bitmasks, const std::string& t_title) const;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        static int32_t GetIndex(int32_t t_x, int32_t t_y, int32_t t_width);

        void Reset();

        static MapTypeCol Bitmask2MapTypeCol(int32_t t_bitmask);
    };
}
