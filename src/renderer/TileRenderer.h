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

#include "ogl/Window.h"
#include "camera/Camera.h"

//-------------------------------------------------
// TileRenderer
//-------------------------------------------------

namespace mdcii::renderer
{
    /**
     * Represents a tile renderer.
     */
    class TileRenderer
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        TileRenderer();

        TileRenderer(const TileRenderer& t_other) = delete;
        TileRenderer(TileRenderer&& t_other) noexcept = delete;
        TileRenderer& operator=(const TileRenderer& t_other) = delete;
        TileRenderer& operator=(TileRenderer&& t_other) noexcept = delete;

        ~TileRenderer() noexcept;

        //-------------------------------------------------
        // Render
        //-------------------------------------------------

        /**
         * Renders a single textured tile.
         *
         * @param t_modelMatrix The model matrix to transform the position into world space.
         * @param t_textureId The OpenGL texture handle.
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         * @param t_selected True if the tile is selected.
         */
        void RenderTile(
            const glm::mat4& t_modelMatrix,
            uint32_t t_textureId,
            const ogl::Window& t_window,
            const camera::Camera& t_camera,
            bool t_selected = false
        ) const;

        /**
         * Renders a single textured tile for mouse picking.
         *
         * @param t_modelMatrix The model matrix to transform the position into world space.
         * @param t_textureId The OpenGL texture handle.
         * @param t_color The Id color.
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         */
        void RenderTileForMousePicking(
            const glm::mat4& t_modelMatrix,
            uint32_t t_textureId,
            const glm::vec3& t_color,
            const ogl::Window& t_window,
            const camera::Camera& t_camera
        ) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The OpenGL Vertex Array Object handle.
         */
        uint32_t m_vao{ 0 };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Creates the VAO which consists a VBO to store the vertices and sets the buffer layout.
         */
        void Init();
    };
}
