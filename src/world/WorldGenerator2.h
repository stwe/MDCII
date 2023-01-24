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

#include <imgui.h>
#include <fstream>
#include "Rotation.h"
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
        // Types
        //-------------------------------------------------

        /**
         * Textfilter using for input filenames.
         */
        struct TextFilters
        {
            static int FilterAZ09(ImGuiInputTextCallbackData* t_data)
            {
                if (auto c{ t_data->EventChar }; (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
                {
                    return 0;
                }

                return 1;
            }
        };

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
         * The order of the neighbor indices in Position.
         */
        enum class Direction
        {
            N, S, W, E, NE, NW, SW, SE
        };

        /**
         * A map type for each position.
         */
        enum class MapType
        {
            WATER, TERRAIN, EMBANKMENT, BEACH
        };

        /**
         * Building Id and rotation.
         */
        struct TileInfo
        {
            int32_t id{ -1 };
            Rotation rotation{ Rotation::DEG0 };
        };

        /**
         * Some info about a position.
         */
        struct PositionInfo
        {
            MapType type{ MapType::WATER };
            TileInfo tileInfo{};
            std::array<uint8_t, 4> neighborFlags{ 0, 0, 0, 0 };
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
            PositionInfo positionInfo{};
            std::array<int32_t, 8> neighborIndices{ -1, -1, -1, -1, -1, -1, -1, -1 };

            Position() = delete;

            Position(const int32_t t_x, const int32_t t_y, const double t_elevation)
                : x{ t_x }
                , y{ t_y }
                , elevation{ t_elevation }
            {}

            [[nodiscard]] bool IsElevationAboveWaterLevel() const { return elevation >= WATER_LEVEL; }
        };

        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        // Embankment

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
         * The file in which will the generated map will be saved.
         */
        std::ofstream m_file;

        //-------------------------------------------------
        // Elevations
        //-------------------------------------------------

        /**
         * Generates elevations using Perlin Noise for a 2D map.
         *
         * @param t_seed Any random number.
         * @param t_frequency The noise frequency.
         * @param t_width The width of the island.
         * @param t_height The height of the island.
         *
         * @return List of Position objects.
         */
        [[nodiscard]] static std::vector<Position> CreateElevations(int32_t t_seed, float t_frequency, int32_t t_width, int32_t t_height);

        //-------------------------------------------------
        // Neighbor indices
        //-------------------------------------------------

        /**
         * Stores the neighbor indices for each Position object.
         *
         * @param t_positions A list of Position objects.
         * @param t_width The width of the island.
         * @param t_height The height of the island.
         */
        static void StoreNeighborsIndices(std::vector<Position>& t_positions, int32_t t_width, int32_t t_height);

        //-------------------------------------------------
        // Map types
        //-------------------------------------------------

        /**
         * Splits elevation values into water and terrain.
         * The method is a first coarse filter and creates an island.
         *
         * @param t_positions A list of Position objects.
         */
        static void SplitElevationsInWaterAndTerrain(std::vector<Position>& t_positions);

        /**
         * Adds the Embankment map type to the given positions.
         *
         * @param t_positions A list of Position objects.
         */
        static void AddEmbankment(std::vector<Position>& t_positions);

        /**
         * Adds the Beach map type to the given positions.
         *
         * @param t_positions A list of Position objects.
         */
        static void AddBeach(std::vector<Position>& t_positions);

        /**
         * Checks if a position has a given neighbor map type.
         *
         * @param t_positions A list of Position objects.
         * @param t_position The Position object to check.
         * @param t_mapType The neighbor type whose presence is to be checked.
         *
         * @return True or false.
         */
        [[nodiscard]] static bool HasNeighborMapType(
            const std::vector<Position>& t_positions,
            const Position& t_position,
            MapType t_mapType
        );

        /**
         * Creates neighbor values for the given map type.
         *
         * @param t_positions A list of Position objects.
         * @param t_mapType The map type.
         */
        static void CreateNeighbors(std::vector<Position>& t_positions, MapType t_mapType);

        /**
         * Validate positions of the embankment.
         *
         * @param t_positions A list of Position objects.
         */
        static void ValidateEmbankment(std::vector<Position>& t_positions);

        /**
         * Removes invalid embankment positions.
         *
         * @param t_positions A list of Position objects.
         *
         * @return True if a position is removed, otherwise false.
         */
        [[nodiscard]] static bool RemoveInvalidEmbankment(std::vector<Position>& t_positions);

        /**
         * Sets the final map value for each rotation of the embankment.
         *
         * @param t_positions A list of Position objects.
         */
        static void SetFinalEmbankment(std::vector<Position>& t_positions);

        /**
         * Sets the final map value for each rotation of the beach.
         *
         * @param t_positions A list of Position objects.
         */
        static void SetFinalBeach(std::vector<Position>& t_positions);

        //-------------------------------------------------
        // ImGui
        //-------------------------------------------------

        /**
         * Renders a legend for the map.
         */
        static void RenderLegendImGui();

        /**
         * Colored output of the created elevations with ImGui.
         *
         * @param t_positions A list of Position objects.
         * @param t_width The width of the island.
         * @param t_height The height of the island.
         */
        static void RenderElevationValuesImGui(const std::vector<Position>& t_positions, int32_t t_width, int32_t t_height);

        /**
         * Colored output of the map types with ImGui.
         *
         * @param t_positions A list of Position objects.
         * @param t_width The width of the island.
         * @param t_height The height of the island.
         */
        static void RenderMapTypesImGui(const std::vector<Position>& t_positions, int32_t t_width, int32_t t_height);

        /**
         * Colored output of the map neighbor values with ImGui.
         *
         * @param t_positions A list of Position objects.
         * @param t_width The width of the island.
         * @param t_height The height of the island.
         */
        static void RenderMapNeighborValuesImGui(const std::vector<Position>& t_positions, int32_t t_width, int32_t t_height);

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
         * @param t_positions A list of Position objects.
         * @param t_width The width of the island.
         * @param t_height The height of the island.
         * @param t_worldX The x position of the island in the world.
         * @param t_worldY The y position of the island in the world.
         * @param t_south Generates trees for a southern island else for a northern one.
         */
        static void AddIslandValues(
            nlohmann::json& t_j,
            const std::vector<Position>& t_positions,
            int32_t t_width,
            int32_t t_height,
            int32_t t_worldX,
            int32_t t_worldY,
            bool t_south
        );

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Converts a 2D position to a 1D index.
         *
         * @param t_x The x position.
         * @param t_y The y position.
         * @param t_width The width of a 2D map.
         *
         * @return The 1D index.
         */
        [[nodiscard]] static int32_t GetIndex(int32_t t_x, int32_t t_y, int32_t t_width);

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
        // Create Tiles
        //-------------------------------------------------

        /**
         * Creates Tile objects for the TerrainLayer.
         *
         * @param t_terrainTiles The Tile objects.
         * @param t_positions A list of Position objects.
         * @param t_width The width of the Layer.
         * @param t_height The height of the Layer.
         * @param t_south Generates trees for a southern island else for a northern one.
         */
        static void CreateTerrainTiles(
            std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles,
            const std::vector<Position>& t_positions,
            int32_t t_width,
            int32_t t_height,
            bool t_south
        );

        /**
         * Creates Tile objects for the CoastLayer.
         *
         * @param t_coastTiles The Tile objects.
         * @param t_positions A list of Position objects.
         * @param t_width The width of the Layer.
         * @param t_height The height of the Layer.
         */
        static void CreateCoastTiles(
            std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles,
            const std::vector<Position>& t_positions,
            int32_t t_width,
            int32_t t_height
        );

        /**
         * Creates Tile objects for the BuildingsLayer.
         *
         * @param t_buildingsTiles The Tile objects.
         * @param t_positions A list of Position objects.
         * @param t_width The width of the Layer.
         * @param t_height The height of the Layer.
         */
        static void CreateBuildingsTiles(
            std::vector<std::shared_ptr<layer::Tile>>& t_buildingsTiles,
            const std::vector<Position>& t_positions,
            int32_t t_width,
            int32_t t_height
        );

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
