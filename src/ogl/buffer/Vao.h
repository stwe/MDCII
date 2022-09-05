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

#include <cstdint>

//-------------------------------------------------
// Vao
//-------------------------------------------------

namespace mdcii::ogl::buffer
{
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

        void Bind() const;
        static void Unbind();

        //-------------------------------------------------
        // Draw
        //-------------------------------------------------

        void DrawPrimitives(uint32_t t_drawMode, int32_t t_first) const;
        void DrawPrimitives(uint32_t t_drawMode) const;
        void DrawPrimitives() const;

    protected:

    private:
        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        void CreateId();

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        void CleanUp() const;
    };
}
