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

#include <cstdint>
#include <memory>
#include <vector>

//-------------------------------------------------
// Vao
//-------------------------------------------------

namespace mdcii::ogl::buffer
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * Forward declaration class Vbo.
     */
    class Vbo;

    /**
     * Forward declaration class Ssbo.
     */
    class Ssbo;

    /**
     * Represents a Vertex Array Object.
     */
    class Vao
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The handle of the Vao.
         */
        uint32_t id{ 0 };

        /**
         * Number of vertices to draw.
         */
        int32_t drawCount{ 0 };

        /**
         * To store assigned Vbos objects.
         */
        std::vector<std::unique_ptr<Vbo>> vbos;

        /**
         * For storing multiple assigned Ssbo containers.
         */
        std::vector<std::vector<std::unique_ptr<Ssbo>>> ssbos;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Vao();

        Vao(const Vao& t_other) = delete;
        Vao(Vao&& t_other) noexcept = delete;
        Vao& operator=(const Vao& t_other) = delete;
        Vao& operator=(Vao&& t_other) noexcept = delete;

        ~Vao() noexcept;

        //-------------------------------------------------
        // Bind / unbind
        //-------------------------------------------------

        /**
         * Binds this Vao handle.
         */
        void Bind() const;

        /**
         * Unbinds a Vao handle.
         */
        static void Unbind();

        //-------------------------------------------------
        // Draw
        //-------------------------------------------------

        /**
         * Draws primitives using the currently active shader.
         *
         * @param t_drawMode The OpenGL primitive type to draw.
         * @param t_first The starting index of the vertex array (usually 0).
         */
        void DrawPrimitives(uint32_t t_drawMode, int32_t t_first) const;

        /**
         * Draws primitives using the currently active shader.
         *
         * @param t_drawMode The OpenGL primitive type to draw.
         */
        void DrawPrimitives(uint32_t t_drawMode) const;

        /**
         * Draws primitives using the currently active shader.
         */
        void DrawPrimitives() const;

        /**
         * Draw multiple instances with a single call.
         *
         * @param t_drawMode The OpenGL primitive type to draw.
         * @param t_first The starting index of the vertex array (usually 0).
         * @param t_instances The number of instances to render.
         */
        void DrawInstanced(uint32_t t_drawMode, int32_t t_first, int32_t t_instances) const;

        /**
         * Draw multiple instances with a single call.
         *
         * @param t_drawMode The OpenGL primitive type to draw.
         * @param t_instances The number of instances to render.
         */
        [[maybe_unused]] void DrawInstanced(uint32_t t_drawMode, int32_t t_instances) const;

        /**
         * Draw multiple instances with a single call.
         *
         * @param t_instances The number of instances to render.
         */
        void DrawInstanced(int32_t t_instances) const;

    protected:

    private:
        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        /**
         * Creates a new Vao handle.
         */
        void CreateId();

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up / delete handle.
         */
        void CleanUp() const;
    };
}
