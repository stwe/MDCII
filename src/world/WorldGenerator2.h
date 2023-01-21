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

#include <fstream>
#include <vector>
#include <memory>
#include "FastNoiseLite.h"
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
// WorldGenerator2
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
    // WorldGenerator2
    //-------------------------------------------------

    /**
     * An island generator based on Perlin Noise.
     */
    class WorldGenerator2
    {
    public:
        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        /**
         * Possible neighbor flags.
         * Each flag can set by using the OR operator.
         */
        enum NeighborFlag : uint8_t
        {
            NORTH = 1,
            EAST = 2,
            SOUTH = 4,
            WEST = 8
        };

        /**
         * The order of the indices of the neighbors.
         */
        enum class Direction
        {
            N, S, W, E,
            NE, NW, SW, SE
        };

        /**
         * Represents the type or Id of a terrain position (water, island, embankment ...).
         */
        struct MapValue
        {
            int32_t value{ -1 };
            uint8_t neighborFlag{ 0 };
        };

        /**
         * Encapsulates a single position on the map.
         */
        struct Position
        {
            /**
             * example: >=0.5 means land and <0.5 means water.
             */
            static constexpr auto WATER_LEVEL{ 0.5 };

            int32_t x{ -1 };
            int32_t y{ -1 };
            double elevation{ 0.0 };
            MapValue mapValue{};
            std::array<int32_t, 8> neighborIndices{ -1, -1, -1, -1, -1, -1, -1, -1 };

            [[nodiscard]] bool IsElevationAboveWaterLevel() const { return elevation >= WATER_LEVEL; }
        };

        /**
         * Represents the map.
         */
        struct Map
        {
            std::vector<Position> positions;
            int32_t width{ -1 };
            int32_t height{ -1 };
        };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        WorldGenerator2();

        WorldGenerator2(const WorldGenerator2& t_other) = delete;
        WorldGenerator2(WorldGenerator2&& t_other) noexcept = delete;
        WorldGenerator2& operator=(const WorldGenerator2& t_other) = delete;
        WorldGenerator2& operator=(WorldGenerator2&& t_other) noexcept = delete;

        ~WorldGenerator2() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * Renders ImGui menus.
         */
        void RenderImGui();

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * Represents water on the map.
         */
        static constexpr auto MAP_WATER{ 0 };

        /**
         * Represents terrain on the map.
         */
        static constexpr auto MAP_TERRAIN{ 1 };

        /**
         * Represents the embankment before set rotations.
         */
        static constexpr auto MAP_BANK{ 99 };

        static constexpr auto MAP_BANK_ROT0{ 2 };
        static constexpr auto MAP_BANK_ROT1{ 3 };
        static constexpr auto MAP_BANK_ROT2{ 4 };
        static constexpr auto MAP_BANK_ROT3{ 5 };

        static constexpr auto MAP_BANK_CORNER_INSIDE_ROT0{ 6 };
        static constexpr auto MAP_BANK_CORNER_INSIDE_ROT1{ 7 };
        static constexpr auto MAP_BANK_CORNER_INSIDE_ROT2{ 8 };
        static constexpr auto MAP_BANK_CORNER_INSIDE_ROT3{ 9 };

        static constexpr auto MAP_BANK_CORNER_ROT0{ 10 };
        static constexpr auto MAP_BANK_CORNER_ROT1{ 11 };
        static constexpr auto MAP_BANK_CORNER_ROT2{ 12 };
        static constexpr auto MAP_BANK_CORNER_ROT3{ 13 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The file in which the map will be saved.
         */
        std::ofstream m_file;

        /**
         * The FastNoiseLite lib.
         */
        FastNoiseLite m_fastNoise;

        /**
         * The generated map.
         */
        Map m_map;

        /**
         * If true then trees will be created for a southern island.
         */
        bool m_south{ false };

        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        /**
         * Generates elevations using Perlin Noise for a 2D map.
         *
         * @param t_seed Any random number.
         * @param t_frequency The noise frequency.
         * @param t_width The width of the map/island.
         * @param t_height The height of the map/island.
         */
        void CreateElevations(int32_t t_seed, float t_frequency, int32_t t_width, int32_t t_height);

        /**
         * Stores the neighbor indices for each position.
         */
        void StoreNeighbors();

        /**
         * Renders a legend for the map.
         */
        static void RenderLegendImGui() ;

        /**
         * Colored output of the created elevations with ImGui.
         */
        void RenderElevationsImGui() const;

        /**
         * Splits elevation values into water and terrain.
         * The method is a first coarse filter and creates an island.
         */
        void SplitElevationsInWaterAndTerrain();

        /**
         * Creates embankment default values.
         */
        void AddEmbankment();

        /**
         * Sets a different map value for each rotation of the embankment.
         */
        void AlignEmbankment();

        /**
         * Colored output of the map with ImGui.
         */
        void RenderMapImGui() const;

        //-------------------------------------------------
        // Json
        //-------------------------------------------------

        /**
         * Adds world width and height to an Json value.
         *
         * @param t_j The Json where the values will be added.
         * @param t_worldWidth The width of the world.
         * @param t_worldHeight The height of the world.
         */
        static void AddWorldValues(nlohmann::json& t_j, int32_t t_worldWidth, int32_t t_worldHeight);

        /**
         * Adds an island to an Json value.
         *
         * @param t_j The Json where the values will be added.
         * @param t_worldX The x position in the world.
         * @param t_worldY The y position in the world.
         */
        void AddIslandValues(nlohmann::json& t_j, int32_t t_worldX, int32_t t_worldY);

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initialize class.
         */
        void Init();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Converts a 2D position to a 1D index.
         *
         * @param t_x The x position.
         * @param t_y The y position.
         * @param t_width The width of the 2D map.
         *
         * @return The 1D index.
         */
        [[nodiscard]] static int32_t GetIndex(int32_t t_x, int32_t t_y, int32_t t_width);

        /**
         * Checks if a position is on land and has a neighbor on water.
         *
         * @param t_position The position to check.
         *
         * @return True or false.
         */
        [[nodiscard]] bool IsMapTerrainPositionOnSeaSide(const Position& t_position);

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
        static std::unique_ptr<layer::Tile> CreateTile(int32_t t_id, int32_t t_worldX, int32_t t_worldY, Rotation t_rotation) ;

        //-------------------------------------------------
        // Create Tiles
        //-------------------------------------------------

        /**
         * Creates Tile objects for the TerrainLayer.
         *
         * @param t_terrainTiles The created Tile objects.
         */
        void CreateTerrainTiles(std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles) const;

        /**
         * Creates Tile objects for the CoastLayer.
         *
         * @param t_coastTiles The created Tile objects.
         */
        void CreateCoastTiles(std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles) const;

        /**
         * Creates Tile objects for the BuildingsLayer.
         *
         * @param t_buildingsTiles The created Tile objects.
         */
        void CreateBuildingsTiles(std::vector<std::shared_ptr<layer::Tile>>& t_buildingsTiles) const;

        //-------------------------------------------------
        // File
        //-------------------------------------------------

        /**
         * Creates a new file.
         *
         * @param t_fileName The name of the file.
         *
         * @return True or false if error while creating.
         */
        [[nodiscard]] bool CreateFile(const std::string& t_fileName);

        /**
         * Writes the given Json to the newly created file.
         *
         * @param t_j The Json to write.
         */
        void WriteJsonToFile(const nlohmann::json& t_j);
    };
}
