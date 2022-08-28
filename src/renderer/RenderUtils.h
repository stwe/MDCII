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

#include <glm/ext/matrix_transform.hpp>
#include "MdciiAssert.h"
#include "ogl/OpenGL.h"

//-------------------------------------------------
// RenderUtils
//-------------------------------------------------

namespace mdcii::renderer
{
    /**
     * Static helper methods for rendering.
     */
    class RenderUtils
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        RenderUtils(RenderUtils&& t_other) noexcept = delete;
        RenderUtils(const RenderUtils& t_other) = delete;
        RenderUtils& operator=(const RenderUtils& t_other) = delete;
        RenderUtils& operator=(RenderUtils&& t_other) noexcept = delete;

        //-------------------------------------------------
        // Transform
        //-------------------------------------------------

        /**
         * Calculates a model matrix to transform an isometric position
         * from object space into world space.
         *
         * @param t_isoPosition The isometric coordinates.
         * @param t_size The scale.
         *
         * @return The model matrix.
         */
        static auto GetModelMatrix(const glm::vec2& t_isoPosition, const glm::vec2& t_size) -> glm::mat4
        {
            auto modelMatrix{ glm::mat4(1.0f) };

            modelMatrix = translate(modelMatrix, glm::vec3(t_isoPosition.x, t_isoPosition.y, 0.0f));
            modelMatrix = scale(modelMatrix, glm::vec3(t_size.x, t_size.y, 1.0f));

            return modelMatrix;
        }

        //-------------------------------------------------
        // Mesh
        //-------------------------------------------------

        /**
         * Creates Vao and Vbo to render a rectangle.
         *
         * @param t_vao Pointer to a Vao handle.
         */
        static void CreateRectangleVao(uint32_t* t_vao)
        {
            constexpr float vertices[]{
                // pos      // tex
                0.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,

                0.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 0.0f
            };

            // create Vao
            glGenVertexArrays(1, t_vao);
            MDCII_ASSERT(*t_vao, "[RenderUtils::CreateRectangleVao()] Invalid Vao handle.")
            Log::MDCII_LOG_DEBUG("[RenderUtils::CreateRectangleVao()] A new Vao was created with the Id: {}.", *t_vao);

            // bind Vao
            glBindVertexArray(*t_vao);

            // create Vbo
            uint32_t vbo;
            glGenBuffers(1, &vbo);
            MDCII_ASSERT(vbo, "[RenderUtils::CreateRectangleVao()] Invalid Vbo handle.")
            Log::MDCII_LOG_DEBUG("[RenderUtils::CreateRectangleVao()] A new Vbo was created with the Id: {}.", vbo);

            // bind Vbo
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            // store data
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            // set buffer layout
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

            // unbind Vbo
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // unbind Vao
            glBindVertexArray(0);
        }

    protected:

    private:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        RenderUtils() = default;
        ~RenderUtils() noexcept = default;
    };
}
