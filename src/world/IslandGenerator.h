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
            WATER, TERRAIN
        };

        /**
         * To calculate the bitmask.
         */
        enum NeighborFlag
        {
            GRASS_FLAG = 0,

            NORTH_WEST_FLAG = 1,
            NORTH_FLAG = 2,
            NORTH_EAST_FLAG = 4,

            WEST_FLAG = 8,
            EAST_FLAG = 16,

            SOUTH_WEST_FLAG = 32,
            SOUTH_FLAG = 64,
            SOUTH_EAST_FLAG = 128,

            WATER_FLAG = 256
        };

        enum class AbstractTileType
        {
            UNKNOWN, INVALID,

            WATER, GRASS,

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
            TOP, BOTTOM, LEFT, RIGHT
        };

        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * Elevation values above this value are used as terrain, all others as water.
         */
        static constexpr auto WATER_LEVEL{ 0.5 };

        // todo rotation fails
        static constexpr auto WORLD_WIDTH{ 100 }; // 50
        static constexpr auto WORLD_HEIGHT{ 70 }; // 35

        /**
         * Known mappings Bitmask to TileType.
         */
        const std::unordered_map<int32_t, AbstractTileType> m_bitmaskTileTypes = {
            { 511, AbstractTileType::WATER },
            { 0, AbstractTileType::GRASS },

            { 383, AbstractTileType::CORNER_OUT_TL },
            { 479, AbstractTileType::CORNER_OUT_TR },
            { 507, AbstractTileType::CORNER_OUT_BL },
            { 510, AbstractTileType::CORNER_OUT_BR },

            { 267, AbstractTileType::CORNER_IN_TL },
            { 271, AbstractTileType::CORNER_IN_TL },
            { 299, AbstractTileType::CORNER_IN_TL },
            { 303, AbstractTileType::CORNER_IN_TL },
            { 431, AbstractTileType::CORNER_IN_TL },

            { 278, AbstractTileType::CORNER_IN_TR },
            { 279, AbstractTileType::CORNER_IN_TR },
            { 406, AbstractTileType::CORNER_IN_TR },
            { 407, AbstractTileType::CORNER_IN_TR },
            { 439, AbstractTileType::CORNER_IN_TR },

            { 360, AbstractTileType::CORNER_IN_BL },
            { 361, AbstractTileType::CORNER_IN_BL },
            { 365, AbstractTileType::CORNER_IN_BL },
            { 488, AbstractTileType::CORNER_IN_BL },
            { 489, AbstractTileType::CORNER_IN_BL },

            { 464, AbstractTileType::CORNER_IN_BR },
            { 468, AbstractTileType::CORNER_IN_BR },
            { 496, AbstractTileType::CORNER_IN_BR },
            { 500, AbstractTileType::CORNER_IN_BR },
            { 501, AbstractTileType::CORNER_IN_BR },

            { 287, AbstractTileType::TOP },
            { 319, AbstractTileType::TOP },
            { 415, AbstractTileType::TOP },
            { 447, AbstractTileType::TOP },

            { 504, AbstractTileType::BOTTOM },
            { 505, AbstractTileType::BOTTOM },
            { 508, AbstractTileType::BOTTOM },
            { 509, AbstractTileType::BOTTOM },

            { 363, AbstractTileType::LEFT },
            { 367, AbstractTileType::LEFT },
            { 491, AbstractTileType::LEFT },
            { 495, AbstractTileType::LEFT },

            { 470, AbstractTileType::RIGHT },
            { 471, AbstractTileType::RIGHT },
            { 502, AbstractTileType::RIGHT },
            { 503, AbstractTileType::RIGHT }
        };

        const std::unordered_map<AbstractTileType, int32_t> m_tileTypeBitmasks = {
            { AbstractTileType::WATER, 511 },
            { AbstractTileType::GRASS, 0 },
            { AbstractTileType::CORNER_OUT_TL, 383 },
            { AbstractTileType::CORNER_IN_TL, 267 },
            { AbstractTileType::CORNER_IN_TR, 278 },
            { AbstractTileType::CORNER_IN_BL, 360 },
            { AbstractTileType::CORNER_IN_BR, 464 },
            { AbstractTileType::TOP, 287 },
            { AbstractTileType::BOTTOM, 504 },
            { AbstractTileType::LEFT, 363 },
            { AbstractTileType::RIGHT, 470 },
        };

        /**
         * A char for each TileType.
         */
        const std::unordered_map<AbstractTileType, const char*> m_tileTypeChars = {
            { AbstractTileType::UNKNOWN, "?" },
            { AbstractTileType::INVALID, "╳" },

            { AbstractTileType::WATER, "~" },
            { AbstractTileType::GRASS, "░" },

            { AbstractTileType::CORNER_OUT_TL, "╭" },
            { AbstractTileType::CORNER_OUT_TR, "╮" },
            { AbstractTileType::CORNER_OUT_BL, "╰" },
            { AbstractTileType::CORNER_OUT_BR, "╯" },

            { AbstractTileType::CORNER_IN_TL, "┘" },
            { AbstractTileType::CORNER_IN_TR, "└" },
            { AbstractTileType::CORNER_IN_BL, "┐" },
            { AbstractTileType::CORNER_IN_BR, "┌" },

            { AbstractTileType::TOP, "─" },
            { AbstractTileType::BOTTOM, "─" },
            { AbstractTileType::LEFT, "│" },
            { AbstractTileType::RIGHT, "│" },
        };

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
        std::vector<MapType> m_terrainValues;

        /**
         * Stores a bitmask value for each position.
         */
        std::vector<int32_t> m_bitmaskValues;

        /**
         * Stores invalid positions.
         */
        std::unordered_set<int32_t> m_invalid;

        /**
         * A flag that controls the rendering of ImGui menus.
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
         * The map has been divided into terrain and water. The method shows the values.
         */
        void RenderMapTypesImGui();

        /**
         * Shows the calculated bitmask values.
         */
        void RenderBitmaskValuesImGui();

        /**
         * Shows the calculated bitmask values as characters.
         */
        void RenderBitmaskValuesAsCharsImGui();

        /**
         * Save data to file.
         */
        void SaveIslandImGui();

        //-------------------------------------------------
        // Json
        //-------------------------------------------------

        void AddIslandJson(nlohmann::json& t_json);

        //-------------------------------------------------
        // Tiles
        //-------------------------------------------------

        /**
         * Creates Tile objects for the TerrainLayer.
         *
         * @param t_terrainTiles The Tile objects.
         */
        void CreateTerrainTiles(std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles);

        /**
         * Creates Tile objects for the CoastLayer.
         *
         * @param t_coastTiles The Tile objects.
         */
        void CreateCoastTiles(std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles);

        /**
         * Creates Tile objects for the BuildingsLayer.
         *
         * @param t_buildingsTiles The Tile objects.
         */
        void CreateBuildingsTiles(std::vector<std::shared_ptr<layer::Tile>>& t_buildingsTiles);

        /**
         * Creates a Tile.
         *
         * @param t_id The Building Id.
         * @param t_worldX The world x position.
         * @param t_worldY The world y position.
         * @param t_rotation The rotation of the building.
         *
         * @return The created Tile object pointer.
         */
        static std::unique_ptr<layer::Tile> CreateTile(int32_t t_id, int32_t t_worldX, int32_t t_worldY, Rotation t_rotation);

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        static int32_t GetIndex(int32_t t_x, int32_t t_y, int32_t t_width);

        void Reset();
    };
}
