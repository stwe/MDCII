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
#include "Tile.h"

//-------------------------------------------------
// WorldLayer
//-------------------------------------------------

namespace mdcii::layer
{
    /**
     * The WorldLayer contains all the data to render the deep water area of the world.
     */
    class WorldLayer : public GameLayer
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * Contains all Tile pointers in the order DEG0.
         */
        std::vector<std::shared_ptr<Tile>> tiles;

        /**
         * A vector of Tile pointers for each rotation.
         * The Tile pointers are in the correct order for rendering.
         */
        std::array<std::vector<std::shared_ptr<Tile>>, world::NR_OF_ROTATIONS> sortedTiles;

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

        WorldLayer() = delete;

        /**
         * Constructs a new WorldLayer object.
         *
         * @param t_context Access to shared objects.
         * @param t_world The World object.
         */
        WorldLayer(std::shared_ptr<state::Context> t_context, world::World* t_world);

        WorldLayer(const WorldLayer& t_other) = delete;
        WorldLayer(WorldLayer&& t_other) noexcept = delete;
        WorldLayer& operator=(const WorldLayer& t_other) = delete;
        WorldLayer& operator=(WorldLayer&& t_other) noexcept = delete;

        ~WorldLayer() noexcept override;

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * Each model matrix is based on this Building Id.
         */
        static constexpr auto WATER_BUILDING_ID{ 1201 };

        /**
         * Each model matrix is based on this gfx number.
         */
        static constexpr auto WATER_GFX{ 758 };

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
         * Creates a model matrix for a given Tile object.
         *
         * @param t_tile The Tile object.
         * @param t_zoom The zoom for which to create the model matrix.
         * @param t_rotation The rotation for which to create the model matrix.
         *
         * @return The model matrix.
         */
        [[nodiscard]] glm::mat4 CreateModelMatrix(const Tile& t_tile, world::Zoom t_zoom, world::Rotation t_rotation) const;
    };
}
