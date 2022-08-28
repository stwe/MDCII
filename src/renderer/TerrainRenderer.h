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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#pragma once

#include <array>
#include "ogl/Window.h"
#include "camera/Camera.h"
#include "map/MapLayer.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::map
{
    class Map;
}

//-------------------------------------------------
// TerrainRenderer
//-------------------------------------------------

namespace mdcii::renderer
{
    /**
     * Instanced terrain rendering.
     */
    class TerrainRenderer
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        /**
         * Constructs a new TerrainRenderer object.
         *
         * @param t_map The parent Map object.
         */
        explicit TerrainRenderer(map::Map* t_map);

        TerrainRenderer(const TerrainRenderer& t_other) = delete;
        TerrainRenderer(TerrainRenderer&& t_other) noexcept = delete;
        TerrainRenderer& operator=(const TerrainRenderer& t_other) = delete;
        TerrainRenderer& operator=(TerrainRenderer&& t_other) noexcept = delete;

        ~TerrainRenderer() noexcept;

        //-------------------------------------------------
        // Render
        //-------------------------------------------------

        void RenderTiles(
            map::Zoom t_zoom,
            map::Rotation t_rotation,
            const ogl::Window& t_window,
            const camera::Camera& t_camera
        ) const;

        //-------------------------------------------------
        // Instancing
        //-------------------------------------------------

        void AddModelMatrices(
            map::Zoom t_zoom,
            const map::MapLayer::Model_Matrices_For_Each_Rotation& t_modelMatrices,
            int32_t t_instances
        );

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The parent Map object.
         */
        map::Map* m_map{ nullptr };

        /**
         * The OpenGL Vertex Array Object handles.
         */
        std::array<uint32_t, map::NR_OF_ZOOMS> m_vaos{};

        /**
         * Number of instances to render.
         */
        int32_t m_instances{ 0 };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes the class.
         */
        void Init();
    };
}
