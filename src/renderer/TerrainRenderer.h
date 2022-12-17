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

#include "layer/TerrainLayer.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::state
{
    /**
     * Forward declaration struct Context.
     */
    struct Context;
}

namespace mdcii::ogl::buffer
{
    /**
     * Forward declaration class Vao.
     */
    class Vao;

    /**
     * Forward declaration class Ssbo.
     */
    class Ssbo;
}

namespace mdcii::world
{
    /**
     * Forward declaration class TileAtlas.
     */
    class TileAtlas;

    /**
     * Forward declaration enum class Zoom.
     */
    enum class Zoom;

    /**
     * Forward declaration enum class Rotation.
     */
    enum class Rotation;
}

//-------------------------------------------------
// TerrainRenderer
//-------------------------------------------------

namespace mdcii::renderer
{
    /**
     * Renders a TerrainLayer.
     */
    class TerrainRenderer
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        TerrainRenderer() = delete;

        /**
         * Constructs a new TerrainRenderer object.
         *
         * @param t_context Access to shared objects.
         * @param t_tileAtlas The TileAtlas object.
         */
        TerrainRenderer(std::shared_ptr<state::Context> t_context, std::shared_ptr<world::TileAtlas> t_tileAtlas);

        TerrainRenderer(const TerrainRenderer& t_other) = delete;
        TerrainRenderer(TerrainRenderer&& t_other) noexcept = delete;
        TerrainRenderer& operator=(const TerrainRenderer& t_other) = delete;
        TerrainRenderer& operator=(TerrainRenderer&& t_other) noexcept = delete;

        ~TerrainRenderer() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * Updates the TerrainLayer.
         */
        void Update();

        /**
         * Renders a Layer content with the specified zoom and rotation.
         *
         * @param t_modelMatricesSsbos The model matrices Ssbos.
         * @param t_gfxNumbersSsbo The gfx numbers Ssbo.
         * @param t_buildingIdsSsbo The Building Ids Ssbo.
         * @param t_instancesToRender The number of instances to render.
         * @param t_zoom The zoom to render for.
         * @param t_rotation The rotation to render for.
         */
        void Render(
            const layer::GameLayer::Model_Matrices_Ssbos_For_Each_zoom& t_modelMatricesSsbos,
            const ogl::buffer::Ssbo& t_gfxNumbersSsbo,
            const ogl::buffer::Ssbo& t_buildingIdsSsbo,
            int32_t t_instancesToRender,
            world::Zoom t_zoom,
            world::Rotation t_rotation
        ) const;

        /**
         * Renders a TerrainLayer with the specified zoom and rotation.
         *
         * @param t_terrainLayer The TerrainLayer object.
         * @param t_zoom The zoom to render for.
         * @param t_rotation The rotation to render for.
         */
        void Render(const layer::TerrainLayer& t_terrainLayer, world::Zoom t_zoom, world::Rotation t_rotation) const;

    protected:

    private:
        //-------------------------------------------------
        // Shader constants
        //-------------------------------------------------

        /**
         * The number of the modelMatrices shader binding.
         */
        static constexpr auto MODEL_MATRICES_BINDING{ 0 };

        /**
         * The number of the gfxNumbers shader binding.
         */
        static constexpr auto GFX_NUMBERS_BINDING{ 1 };

        /**
         * The number of the buildingInfo shader binding.
         */
        static constexpr auto BUILDING_IDS_BINDING{ 2 };

        /**
         * The number of the heights shader binding.
         */
        static constexpr auto HEIGHTS_BINDING{ 3 };

        /**
         * The number of the animationInfo shader binding.
         */
        static constexpr auto ANIMATIONS_BINDING{ 4 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * To have access to the shared objects (Window, Camera, Original-Assets).
         */
        std::shared_ptr<state::Context> m_context;

        /**
         * An OpenGL texture array for each zoom level.
         */
        std::shared_ptr<world::TileAtlas> m_tileAtlas;

        /**
         * A Vao for each zoom level.
         */
        std::array<std::unique_ptr<ogl::buffer::Vao>, 3> m_vaos;

        /**
         * A Ssbo for each zoom containing the height of each Stadtfld Bsh-Image.
         */
        std::array<std::unique_ptr<ogl::buffer::Ssbo>, 3> m_heightsSsbos;

        /**
         * A Ssbo containing the animation information for each building.
         */
        std::unique_ptr<ogl::buffer::Ssbo> m_animationSsbo;

        /**
         * The number of updates.
         */
        int32_t m_updates{ 0 };

        /**
         * Counters to get animation frames.
         */
        std::vector<int32_t> m_timeCounter{ 0, 0, 0, 0, 0 };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes this renderer.
         */
        void Init();

        //-------------------------------------------------
        // Create buffers
        //-------------------------------------------------

        /**
         * Creates all Vaos.
         */
        void CreateVaos();

        /**
         * Creates a Ssbo for each zoom level which holding the height of each Bsh-Image.
         */
        void CreateHeightsSsbos();

        /**
         * Creates a Ssbo which holding animation info for each building.
         */
        void CreateAnimationInfoSsbo();
    };
}
