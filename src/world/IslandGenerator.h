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
         * For the classification of the map elevation values.
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
         * Stores a map type and a color.
         */
        struct MapTypeCol
        {
            MapType mapType;
            ImU32 color;
        };

        /**
         * To calculate the bitmask.
         */
        enum NeighborFlag
        {
            TERRAIN_FLAG = 0,

            NORTH_WEST_FLAG = 1,
            NORTH_FLAG = 2,
            NORTH_EAST_FLAG = 4,

            WEST_FLAG = 8,
            EAST_FLAG = 16,

            SOUTH_WEST_FLAG = 32,
            SOUTH_FLAG = 64,
            SOUTH_EAST_FLAG = 128,

            EMBANKMENT_FLAG = 256,
            COAST_FLAG = 512,
            SHALLOW_FLAG = 768
        };

        /**
         * Common types that can be mapped to bitmasks.
         */
        enum class TileType
        {
            /**
             * For invalid bitmask values.
             */
            INVALID,

            /**
             *     COTL    COTR
             *     x----------x
             *     |          |
             *     x----------x
             *     COBL    COBR
             */
            CORNER_OUT_TL, CORNER_OUT_TR, CORNER_OUT_BL, CORNER_OUT_BR,

            /**
             *     -----------
             *     |         |
             * ----x         x----
             *     CITL      CITR
             */
            CORNER_IN_TL, CORNER_IN_TR,

            /**
             *     CIBL      CIBR
             * ----x         x----
             *     |         |
             *     -----------
             */
            CORNER_IN_BL, CORNER_IN_BR,

            /**
             *          TOP
             *      ----------
             *      |        |
             * LEFT |        | RIGHT
             *      |        |
             *      ----------
             *        BOTTOM
             */
            TOP, BOTTOM, LEFT, RIGHT,
        };

        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * Known embankment bitmask values.
         */
        const std::unordered_map<int32_t, TileType> m_embankmentBitmaskTileTypes = {
            { 383, TileType::CORNER_OUT_TL },
            { 479, TileType::CORNER_OUT_TR },
            { 507, TileType::CORNER_OUT_BL },
            { 510, TileType::CORNER_OUT_BR },

            { 267, TileType::CORNER_IN_TL },
            { 271, TileType::CORNER_IN_TL },
            { 299, TileType::CORNER_IN_TL },
            { 303, TileType::CORNER_IN_TL },
            { 431, TileType::CORNER_IN_TL },

            { 278, TileType::CORNER_IN_TR },
            { 279, TileType::CORNER_IN_TR },
            { 406, TileType::CORNER_IN_TR },
            { 407, TileType::CORNER_IN_TR },
            { 439, TileType::CORNER_IN_TR },

            { 360, TileType::CORNER_IN_BL },
            { 361, TileType::CORNER_IN_BL },
            { 365, TileType::CORNER_IN_BL },
            { 488, TileType::CORNER_IN_BL },
            { 489, TileType::CORNER_IN_BL },

            { 464, TileType::CORNER_IN_BR },
            { 468, TileType::CORNER_IN_BR },
            { 496, TileType::CORNER_IN_BR },
            { 500, TileType::CORNER_IN_BR },
            { 501, TileType::CORNER_IN_BR },

            { 287, TileType::TOP },
            { 319, TileType::TOP },
            { 415, TileType::TOP },
            { 447, TileType::TOP },

            { 504, TileType::BOTTOM },
            { 505, TileType::BOTTOM },
            { 508, TileType::BOTTOM },
            { 509, TileType::BOTTOM },

            { 363, TileType::LEFT },
            { 367, TileType::LEFT },
            { 491, TileType::LEFT },
            { 495, TileType::LEFT },

            { 470, TileType::RIGHT },
            { 471, TileType::RIGHT },
            { 502, TileType::RIGHT },
            { 503, TileType::RIGHT },
        };

        /**
         * Known coast bitmask values.
         */
        const std::unordered_map<int32_t, TileType> m_coastBitmaskTileTypes = {
            { 639, TileType::CORNER_OUT_TL },
            { 735, TileType::CORNER_OUT_TR },
            { 763, TileType::CORNER_OUT_BL },
            { 766, TileType::CORNER_OUT_BR },

            { 523, TileType::CORNER_IN_TL },
            { 527, TileType::CORNER_IN_TL },
            { 555, TileType::CORNER_IN_TL },
            { 559, TileType::CORNER_IN_TL },

            { 534, TileType::CORNER_IN_TR },
            { 535, TileType::CORNER_IN_TR },
            { 662, TileType::CORNER_IN_TR },
            { 663, TileType::CORNER_IN_TR },

            { 616, TileType::CORNER_IN_BL },
            { 617, TileType::CORNER_IN_BL },
            { 744, TileType::CORNER_IN_BL },
            { 745, TileType::CORNER_IN_BL },

            { 720, TileType::CORNER_IN_BR },
            { 724, TileType::CORNER_IN_BR },
            { 752, TileType::CORNER_IN_BR },
            { 756, TileType::CORNER_IN_BR },

            { 527, TileType::TOP },
            { 543, TileType::TOP },
            { 575, TileType::TOP },
            { 671, TileType::TOP },

            { 760, TileType::BOTTOM },
            { 761, TileType::BOTTOM },
            { 764, TileType::BOTTOM },

            { 619, TileType::LEFT },
            { 623, TileType::LEFT },
            { 747, TileType::LEFT },

            { 726, TileType::RIGHT },
            { 727, TileType::RIGHT },
            { 758, TileType::RIGHT },
        };

        /**
         * Known shallow bitmask values.
         */
        const std::unordered_map<int32_t, TileType> m_shallowBitmaskTileTypes = {
            { 895, TileType::CORNER_OUT_TL },
            { 991, TileType::CORNER_OUT_TR },
            { 1019, TileType::CORNER_OUT_BL },
            { 1022, TileType::CORNER_OUT_BR },

            { 779, TileType::CORNER_IN_TL },
            { 783, TileType::CORNER_IN_TL },
            { 815, TileType::CORNER_IN_TL },
            { 811, TileType::CORNER_IN_TL },

            { 790, TileType::CORNER_IN_TR },
            { 791, TileType::CORNER_IN_TR },
            { 918, TileType::CORNER_IN_TR },
            { 919, TileType::CORNER_IN_TR },

            { 872, TileType::CORNER_IN_BL },
            { 873, TileType::CORNER_IN_BL },
            { 1000, TileType::CORNER_IN_BL },
            { 1001, TileType::CORNER_IN_BL },

            { 976, TileType::CORNER_IN_BR },
            { 980, TileType::CORNER_IN_BR },
            { 1008, TileType::CORNER_IN_BR },
            { 1012, TileType::CORNER_IN_BR },

            { 799, TileType::TOP },
            { 831, TileType::TOP },
            { 927, TileType::TOP },

            { 1016, TileType::BOTTOM },
            { 1017, TileType::BOTTOM },
            { 1020, TileType::BOTTOM },

            { 875, TileType::LEFT },
            { 879, TileType::LEFT },
            { 1003, TileType::LEFT },

            { 982, TileType::RIGHT },
            { 983, TileType::RIGHT },
            { 1014, TileType::RIGHT },
        };

        /**
         * A char for each tile type.
         */
        const std::unordered_map<TileType, const char*> m_tileTypeChars = {
            { TileType::INVALID, "x" },

            { TileType::CORNER_OUT_TL, "╭" },
            { TileType::CORNER_OUT_TR, "╮" },
            { TileType::CORNER_OUT_BL, "╰" },
            { TileType::CORNER_OUT_BR, "╯" },

            { TileType::CORNER_IN_TL, "┘" },
            { TileType::CORNER_IN_TR, "└" },
            { TileType::CORNER_IN_BL, "┐" },
            { TileType::CORNER_IN_BR, "┌" },

            { TileType::TOP, "─" },
            { TileType::BOTTOM, "─" },
            { TileType::LEFT, "│" },
            { TileType::RIGHT, "│" },
        };

        /**
         * Known bitmask tile types by map type.
         */
        const std::unordered_map<MapType, std::unordered_map<int32_t, TileType>> m_bitmaskTileTypes = {
            { MapType::EMBANKMENT, m_embankmentBitmaskTileTypes },
            { MapType::COAST, m_coastBitmaskTileTypes },
            { MapType::SHALLOW_WATER, m_shallowBitmaskTileTypes }
        };

        /**
         * Elevation values above this value are used as terrain, all others as water.
         */
        static constexpr auto WATER_LEVEL{ 0.5 };

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
         * Stores a map type for each position.
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
        void CalcBitmaskValues();

        //-------------------------------------------------
        // Bitmasking
        //-------------------------------------------------

        int32_t GetNorthValue(int32_t t_x, int32_t t_y);
        int32_t GetEastValue(int32_t t_x, int32_t t_y);
        int32_t GetSouthValue(int32_t t_x, int32_t t_y);
        int32_t GetWestValue(int32_t t_x, int32_t t_y);

        int32_t GetNorthWestValue(int32_t t_x, int32_t t_y);
        int32_t GetNorthEastValue(int32_t t_x, int32_t t_y);
        int32_t GetSouthWestValue(int32_t t_x, int32_t t_y);
        int32_t GetSouthEastValue(int32_t t_x, int32_t t_y);

        //-------------------------------------------------
        // ImGui
        //-------------------------------------------------

        /**
         * Shows the given map values.
         */
        void RenderMapTypesImGui() const;

        /**
         * Shows the given bitmask values.
         */
        void RenderBitmaskValuesImGui() const;

        /**
         * Shows the calculated bitmask values as characters.
         */
        void RenderBitmaskValuesAsCharsImGui();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        static int32_t GetIndex(int32_t t_x, int32_t t_y, int32_t t_width);

        void Reset();

        static MapTypeCol Bitmask2MapTypeCol(int32_t t_bitmask);

        void AddMapType(MapType t_mapType);
    };
}
