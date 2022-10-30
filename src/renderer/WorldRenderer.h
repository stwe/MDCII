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

#include <memory>
#include "ogl/Window.h"
#include "camera/Camera.h"
#include "world/Zoom.h"
#include "world/Tile.h"
#include "data/Buildings.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * Forward declaration class World.
     */
    class World;

    /**
     * Forward declaration enum class WorldLayerType.
     */
    enum class WorldLayerType;

    /**
     * Forward declaration enum class Rotation.
     */
    enum class Rotation;
}

namespace mdcii::ogl::buffer
{
    /**
     * Forward declaration class Vao.
     */
    class Vao;
}

//-------------------------------------------------
// WorldRenderer
//-------------------------------------------------

namespace mdcii::renderer
{
    /**
     * Renders the world with instancing.
     */
    class WorldRenderer
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        WorldRenderer() = delete;

        /**
         * Constructs a new WorldRenderer object.
         *
         * @param t_world The parent World object.
         */
        explicit WorldRenderer(world::World* t_world);

        WorldRenderer(const WorldRenderer& t_other) = delete;
        WorldRenderer(WorldRenderer&& t_other) noexcept = delete;
        WorldRenderer& operator=(const WorldRenderer& t_other) = delete;
        WorldRenderer& operator=(WorldRenderer&& t_other) noexcept = delete;

        ~WorldRenderer() noexcept;

        //-------------------------------------------------
        // Render
        //-------------------------------------------------

        /**
         * Renders a Layer with the specified zoom and rotation.
         *
         * @param t_layerType The Layer type to render.
         * @param t_zoom The zoom to render for.
         * @param t_rotation The rotation to render for.
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         */
        void Render(
            world::WorldLayerType t_layerType,
            world::Zoom t_zoom,
            world::Rotation t_rotation,
            const ogl::Window& t_window,
            const camera::Camera& t_camera
        ) const;

        /**
         * Renders the Grid Layer with the specified zoom and rotation.
         *
         * @param t_zoom The zoom to render for.
         * @param t_rotation The rotation to render for.
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         */
        void Render(
            world::Zoom t_zoom,
            world::Rotation t_rotation,
            const ogl::Window& t_window,
            const camera::Camera& t_camera
        ) const;

        //-------------------------------------------------
        // Add / remove Gpu data
        //-------------------------------------------------

        /**
         * Deletes a building from the Gpu.
         *
         * @param t_tile The Tile object to delete building Gpu data.
         */
        void DeleteBuildingFromGpu(const world::Tile& t_tile);

        /**
         * Deletes a building from the Gpu.
         *
         * @param t_tiles The Tile objects to delete building Gpu data.
         */
        void DeleteBuildingFromGpu(std::vector<std::unique_ptr<world::Tile>>& t_tiles);

        /**
         * Deletes a building from the Gpu.
         *
         * @param t_tileIndices The Tile indices to delete building Gpu data.
         */
        void DeleteBuildingFromGpu(const std::vector<int32_t>& t_tileIndices);

        /**
         * Deletes a building from Cpu.
         *
         * @param t_tile Tile object where building information should be deleted/overwritten.
         */
        static void DeleteBuildingFromCpu(world::Tile& t_tile);

        /**
         * Deletes a building from Cpu.
         *
         * @param t_tileIndices Tile object indices where building information should be deleted/overwritten.
         */
        void DeleteBuildingFromCpu(const std::vector<int32_t>& t_tileIndices) const;

        /**
         * Adds a building to the Gpu.
         *
         * @param t_building The building to add.
         * @param t_buildingRotation The rotation of the building.
         * @param t_x The world x position.
         * @param t_y The world y position.
         * @param t_tiles To store the created building tiles.
         */
        void AddBuildingToGpu(
            const data::Building& t_building,
            world::Rotation t_buildingRotation,
            int t_x, int t_y,
            std::vector<std::unique_ptr<world::Tile>>& t_tiles
        );

        /**
         * Updates the Gpu data of a Layer object.
         *
         * @param t_instance The instance to change.
         * @param t_layerType The type of the Layer object.
         * @param t_zoom The zoom.
         * @param t_rotation The rotation.
         * @param t_modelMatrix The new model matrix.
         * @param t_atlasNr The new texture atlas number.
         * @param t_offset The new texture offset.
         * @param t_height The new texture height.
         */
        void UpdateGpuData(
            int32_t t_instance,
            world::WorldLayerType t_layerType,
            world::Zoom t_zoom,
            world::Rotation t_rotation,
            const glm::mat4& t_modelMatrix,
            int t_atlasNr,
            const glm::vec2& t_offset,
            float t_height
        );

    protected:

    private:
        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        /**
         * A Vao for each zoom level.
         */
        using Vaos = std::array<std::unique_ptr<ogl::buffer::Vao>, world::NR_OF_ZOOMS>;

        /**
         * A Vaos type for each Layer type (TERRAIN, BUILDINGS, TERRAIN_AND_BUILDINGS, GRID) except type NONE.
         */
        using Layer_Vaos = std::array<Vaos, 4>;

        //-------------------------------------------------
        // Shader constants
        //-------------------------------------------------

        /**
         * The number of the modelMatrices shader binding.
         */
        static constexpr auto MODEL_MATRICES_BINDING{ 0 };

        /**
         * The number of the offsets shader binding.
         */
        static constexpr auto OFFSETS_BINDING{ 1 };

        /**
         * The number of the texture atlas shader binding.
         */
        static constexpr auto TEXTURE_ATLAS_BINDING{ 2 };

        /**
         * The number of the heights shader binding.
         */
        static constexpr auto HEIGHTS_BINDING{ 3 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The parent World object.
         */
        world::World* m_world{ nullptr };

        /**
         * Each Layer has three Vao.
         */
        Layer_Vaos m_vaos{};

        /**
         * Each zoom Level has a different grid texture.
         */
        std::array<std::string, world::NR_OF_ZOOMS> m_gridFileNames{};

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes this renderer.
         */
        void Init();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Add model matrices to the Gpu.
         *
         * @param t_zoom The zoom level.
         * @param t_layerType The type of the Layer.
         */
        void AddModelMatrices(world::Zoom t_zoom, world::WorldLayerType t_layerType);

        /**
         * Add texture info to the Gpu.
         *
         * @param t_zoom The zoom level.
         * @param t_layerType The type of the Layer.
         */
        void AddTextureInfo(world::Zoom t_zoom, world::WorldLayerType t_layerType) const;
    };
}
