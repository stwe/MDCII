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

#include "layer/GridLayer.h"

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
     * Forward declaration enum class Zoom.
     */
    enum class Zoom;

    /**
     * Forward declaration enum class Rotation.
     */
    enum class Rotation;
}

//-------------------------------------------------
// GridRenderer
//-------------------------------------------------

namespace mdcii::renderer
{
    /**
     * A renderer to render the world grid.
     */
    class GridRenderer
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        GridRenderer() = delete;

        /**
         * Constructs a new GridRenderer object.
         *
         * @param t_context Access to shared objects.
         */
        explicit GridRenderer(std::shared_ptr<state::Context> t_context);

        GridRenderer(const GridRenderer& t_other) = delete;
        GridRenderer(GridRenderer&& t_other) noexcept = delete;
        GridRenderer& operator=(const GridRenderer& t_other) = delete;
        GridRenderer& operator=(GridRenderer&& t_other) noexcept = delete;

        ~GridRenderer() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * Renders the GridLayer with the specified zoom and rotation.
         *
         * @param t_gridLayer The GridLayer object to render.
         * @param t_zoom The zoom to render for.
         * @param t_rotation The rotation to render for.
         */
        void Render(const layer::GridLayer& t_gridLayer, world::Zoom t_zoom, world::Rotation t_rotation) const;

    protected:

    private:
        //-------------------------------------------------
        // Shader constants
        //-------------------------------------------------

        /**
         * The number of the modelMatrices shader binding.
         */
        static constexpr auto MODEL_MATRICES_BINDING{ 0 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * To have access to the shared objects (Window, Camera, Original-Assets).
         */
        std::shared_ptr<state::Context> m_context;

        /**
         * A Vao for each zoom level.
         */
        std::array<std::unique_ptr<ogl::buffer::Vao>, world::NR_OF_ZOOMS> m_vaos;

        /**
         * Each zoom has a different grid texture.
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
        // Create buffers
        //-------------------------------------------------

        /**
         * Creates all Vaos.
         */
        void CreateVaos();
    };
}
