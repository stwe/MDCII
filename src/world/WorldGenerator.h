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
#include "data/json.hpp"
#include "physics/Aabb.h"

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
// WorldGenerator
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * Creates a custom world map.
     */
    class [[deprecated("use WorldGenerator2 instead")]] WorldGenerator
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        WorldGenerator();

        WorldGenerator(const WorldGenerator& t_other) = delete;
        WorldGenerator(WorldGenerator&& t_other) noexcept = delete;
        WorldGenerator& operator=(const WorldGenerator& t_other) = delete;
        WorldGenerator& operator=(WorldGenerator&& t_other) noexcept = delete;

        ~WorldGenerator() noexcept;

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
        // Member
        //-------------------------------------------------

        /**
         * The new map file.
         */
        std::ofstream m_file;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initialize class.
         */
        void Init();

        //-------------------------------------------------
        // Create Json
        //-------------------------------------------------

        /**
         * Checks the Aabbs for collisions.
         *
         * @param t_aabbs The Aabbs to be checked.
         *
         * @return False if there is a collision.
         */
        static bool Validate(const std::vector<physics::Aabb>& t_aabbs);

        /**
         * Adds world width and height to an Json value.
         *
         * @param t_j The Json where the values will be added.
         * @param t_width The width of the world.
         * @param t_height The height of the world.
         */
        static void AddWorldValues(nlohmann::json& t_j, int32_t t_width, int32_t t_height);

        /**
         * Adds an island to an Json value.
         *
         * @param t_j The Json where the values will be added.
         * @param t_width The width of the island.
         * @param t_height The height of the island.
         * @param t_worldX The x position in the world.
         * @param t_worldY The y position in the world.
         * @param t_south If true then trees will be created for a southern island.
         */
        static void AddIslandValues(nlohmann::json& t_j, int32_t t_width, int32_t t_height, int32_t t_worldX, int32_t t_worldY, bool t_south);

        //-------------------------------------------------
        // Create Layer
        //-------------------------------------------------

        /**
         * Creates Tile objects for the TerrainLayer.
         *
         * @param t_width The width of the island.
         * @param t_height The height of the island.
         * @param t_terrainTiles The created Tile objects.
         * @param t_south If true then trees will be created for a southern island.
         */
        static void CreateTerrain(
            int32_t t_width,
            int32_t t_height,
            std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles,
            bool t_south
        );

        /**
         * Creates Tile objects for the CoastLayer.
         *
         * @param t_width The width of the island.
         * @param t_height The height of the island.
         * @param t_coastTiles The created Tile objects.
         */
        static void CreateCoast(
            int32_t t_width,
            int32_t t_height,
            std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles
        );

        /**
         * Creates Tile objects for the BuildingsLayer.
         *
         * @param t_width The width of the island.
         * @param t_height The height of the island.
         * @param t_buildingsTiles The created Tile objects.
         */
        static void CreateBuildings(
            int32_t t_width,
            int32_t t_height,
            std::vector<std::shared_ptr<layer::Tile>>& t_buildingsTiles
        );
    };
}
