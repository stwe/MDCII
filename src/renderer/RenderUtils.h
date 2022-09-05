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
#include "ogl/buffer/Vao.h"
#include "ogl/buffer/Vbo.h"

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
         * @return The created Vao.
         */
        static std::unique_ptr<ogl::buffer::Vao> CreateRectangleVao()
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

            // create && bind Vao
            auto vao{ std::make_unique<ogl::buffer::Vao>() };
            vao->Bind();

            // create && bind Vbo
            auto vbo{ std::make_unique<ogl::buffer::Vbo>() };
            vbo->Bind();

            // store static data
            ogl::buffer::Vbo::StoreStaticData(sizeof(vertices), vertices);

            // set buffer layout
            ogl::buffer::Vbo::AddFloatAttribute(0, 4, 4, 0);

            // set draw count
            vao->drawCount = 6;

            // unbind Vbo && Vao
            ogl::buffer::Vbo::Unbind();
            ogl::buffer::Vao::Unbind();

            // save Vbo in the Vao
            vao->vbos.emplace_back(std::move(vbo));

            return vao;
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
