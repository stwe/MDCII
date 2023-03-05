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
         * Menu string for some map types.
         */
        static constexpr std::array MAP_TYPES_STRINGS = { "Water", "Shallow", "Coast", "Embankment", "Terrain" };

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
         * The tile type for bitmask values by map type.
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
        static constexpr ImU32 TERRAIN_BUTTON_COL{ IM_COL32(0, 127, 0, 255) };

        static constexpr ImU32 TERRAIN_BORDER_COL{ IM_COL32(22, 227, 15, 255) };

        static constexpr ImU32 EMBANKMENT_COL{ IM_COL32(255, 255, 0, 255) };
        static constexpr ImU32 EMBANKMENT_BUTTON_COL{ IM_COL32(127, 127, 0, 255) };

        static constexpr ImU32 COAST_COL{ IM_COL32(80, 192, 248, 255) };
        static constexpr ImU32 COAST_BUTTON_COL{ IM_COL32(40, 96, 124, 255) };

        static constexpr ImU32 SHALLOW_WATER_COL{ IM_COL32(7, 69, 235, 255) };
        static constexpr ImU32 SHALLOW_WATER_BUTTON_COL{ IM_COL32(3, 34, 117, 255) };

        static constexpr ImU32 WATER_COL{ IM_COL32(0, 0, 255, 255) };
        static constexpr ImU32 WATER_BUTTON_COL{ IM_COL32(0, 0, 127, 255) };

        static constexpr ImU32 INVALID_COL{ IM_COL32(255, 0, 0, 255) };
        static constexpr ImU32 INVALID_BUTTON_COL{ IM_COL32(127, 0, 0, 255) };

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
        bool m_noise{ false };

        /**
         * The currently selected map type.
         */
        MapType m_selMapType{ MapType::WATER };

        /**
         * The currently selected tile type.
         */
        inline static TileType m_sel_tile_type_button{ TileType::CORNER_OUT_TL };

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

        /**
         * Renders a button for the given map type.
         *
         * @param t_mapType The map type of the button.
         * @param t_char A character for the button.
         * @param t_index The index within the map.
         * @param t_func A callback function run if button pressed.
         */
        static void RenderCharButtonImGui(MapType t_mapType, std::string t_char, int32_t t_index, const std::function<void()>& t_func) ;

        /**
         * Menu to edit the island.
         */
        void RenderEditMenuImGui();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Calc 2D array indices into 1D array index.
         *
         * @param t_x The x position.
         * @param t_y The y position.
         * @param t_width The array width.
         *
         * @return The index.
         */
        [[nodiscard]] static int32_t GetIndex(int32_t t_x, int32_t t_y, int32_t t_width);

        /**
         * Resets all values for a new island.
         */
        void Reset();

        /**
         * Returns a map type and color for the given bitmask.
         *
         * @param t_bitmask The bitmask.
         *
         * @return Map type and color.
         */
        [[nodiscard]] static MapTypeCol Bitmask2MapTypeCol(int32_t t_bitmask);

        /**
         * Adds a map type (SHALLOW_WATER, COAST, EMBANKMENT) to the island.
         *
         * @param t_mapType The map type to add.
         */
        void AddMapType(MapType t_mapType);

        /**
         * Checks whether there is a tile type for the specified map type and bitmask.
         *
         * @param t_mapType A map type.
         * @param t_bitmask A bitmask.
         *
         * @return True or false.
         */
        [[nodiscard]] bool IsTileTypeKnown(MapType t_mapType, int32_t t_bitmask) const;

        /**
         * Returns the tile type for the map type and bitmask.
         *
         * @param t_mapType A map type.
         * @param t_bitmask A bitmask.
         *
         * @return The tile type.
         */
        [[nodiscard]] TileType GetTileType(MapType t_mapType, int32_t t_bitmask) const;

        /**
         * Updates the tile type.
         *
         * @param t_index The index within the map.
         */
        void UpdateBySelection(int32_t t_index);
    };
}
