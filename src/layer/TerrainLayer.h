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

#include "GameLayer.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * Forward declaration class Island.
     */
    class Island;
}

//-------------------------------------------------
// TerrainLayer
//-------------------------------------------------

namespace mdcii::layer
{
    //-------------------------------------------------
    // Json
    //-------------------------------------------------

    void to_json(nlohmann::json& t_json, const std::shared_ptr<Tile>& t_tile);
    void from_json(const nlohmann::json& t_json, Tile& t_tile);

    //-------------------------------------------------
    // TerrainLayer
    //-------------------------------------------------

    /**
     * The TerrainLayer contains all the data to render an island.
     */
    class TerrainLayer: public GameLayer
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * This allows the Instance Id to be determined for each (rotated) position in the world.
         * x = worldX
         * y = worldY
         * z = rotation as Int
         * result/value = The Instance Id
         */
        std::unordered_map<glm::ivec3, int32_t> instanceIds;

        /**
         * To store the gfx number for each instance.
         * x = gfx for rot0
         * y = gfx for rot90
         * z = gfx for rot180
         * w = gfx for rot270
         */
        std::vector<glm::ivec4> gfxNumbers;

        /**
         * To store the Building Id for each instance.
         * x = building for rot0
         * y = building for rot90
         * z = building for rot180
         * w = building for rot270
         */
        std::vector<glm::ivec4> buildingIds;

        /**
         * A Ssbo containing the gfx number for each instance.
         */
        std::unique_ptr<ogl::buffer::Ssbo> gfxNumbersSsbo;

        /**
         * A Ssbo containing the Building Id number for each instance.
         */
        std::unique_ptr<ogl::buffer::Ssbo> buildingIdsSsbo;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        TerrainLayer() = delete;

        /**
         * Constructs a new TerrainLayer object.
         *
         * @param t_context Access to shared objects.
         * @param t_world The World object.
         * @param t_island The parent Island object.
         * @param t_layerType The type of this Layer.
         */
        TerrainLayer(std::shared_ptr<state::Context> t_context, world::World* t_world, world::Island* t_island, LayerType t_layerType);

        TerrainLayer(const TerrainLayer& t_other) = delete;
        TerrainLayer(TerrainLayer&& t_other) noexcept = delete;
        TerrainLayer& operator=(const TerrainLayer& t_other) = delete;
        TerrainLayer& operator=(TerrainLayer&& t_other) noexcept = delete;

        ~TerrainLayer() noexcept override;

        //-------------------------------------------------
        // Tiles
        //-------------------------------------------------

        /**
         * Creates the Tile objects from a Json value.
         *
         * @param t_json The Json value.
         */
        void CreateTilesFromJson(const nlohmann::json& t_json);

        /**
         * Returns a Tile object.
         *
         * @param t_x The DEG0 x position.
         * @param t_y The DEG0 y position.
         *
         * @return The Tile object.
         */
        [[nodiscard]] const Tile& GetTile(int32_t t_x, int32_t t_y) const;

        /**
         * Returns a Tile object.
         *
         * @param t_position The DEG0 position.
         *
         * @return The Tile object.
         */
        [[nodiscard]] const Tile& GetTile(const glm::ivec2& t_position) const;

        /**
         * Returns a Tile object.
         *
         * @param t_position The DEG0 position.
         *
         * @return The Tile object.
         */
        [[nodiscard]] Tile& GetTile(const glm::ivec2& t_position);

        /**
         * Resets Tile pointer at given instances in tiles and sortedTiles.
         *
         * @param t_instanceIds Instances at which the pointer should be deleted.
         */
        void ResetTilePointersAt(const std::array<int32_t, world::NR_OF_ROTATIONS>& t_instanceIds);

        /**
         * Stores a new Tile.
         *
         * @param t_tile The Tile to be stored.
         */
        void StoreTile(std::unique_ptr<Tile> t_tile);

        /**
         * Adds some pre-calculations to every Tile object of the Layer,
         * which are necessary to render the Tile on the screen.
         *
         * @param t_tile The Tile object.
         */
        void PreCalcTile(Tile& t_tile) const;

        /**
         * Adds some pre-calculations to every Tile object of the Layer,
         * which are necessary to render the Tile on the screen.
         *
         * @param t_tile The Tile object.
         * @param t_x The x position for Deg0 on the island.
         * @param t_y The y position for Deg0 on the island.
         */
        void PreCalcTile(Tile& t_tile, int32_t t_x, int32_t t_y) const;

        /**
         * Calculates the gfx.
         *
         * @param t_tile The Tile object.
         * @param t_rotation The world rotation.
         *
         * @return The gfx to use for rendering.
         */
        [[nodiscard]] int32_t CalcGfx(const Tile& t_tile, world::Rotation t_rotation) const;

        /**
         * Creates a model matrix for a given Tile object.
         *
         * @param t_tile The Tile object.
         * @param t_zoom The zoom for which to create the model matrix.
         * @param t_rotation The rotation for which to create the model matrix.
         *
         * @return The model matrix.
         */
        [[nodiscard]] glm::mat4 CreateModelMatrix(const Tile& t_tile, world::Zoom t_zoom, world::Rotation t_rotation) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The parent Island object.
         */
        world::Island* m_island{ nullptr };

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        void CreateTiles() override;
        void SortTiles() override;

        void CreateModelMatricesContainer() override;
        void CreateGfxNumbersContainer() override;
        void CreateBuildingIdsContainer() override;

        void StoreGfxNumbersInGpu() override;
        void StoreBuildingIdsInGpu() override;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Adds a Tile pointer from a given Json value.
         *
         * @param t_json The Json value.
         */
        void AddTileFromJson(const nlohmann::json& t_json);
    };
}
