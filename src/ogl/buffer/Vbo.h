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
// Vbo
//-------------------------------------------------

namespace mdcii::ogl::buffer
{
    /**
     * Represents a Vertex Buffer Object.
     */
    class Vbo
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The handle of the Vbo.
         */
        uint32_t id{ 0 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Vbo();

        Vbo(const Vbo& t_other) = delete;
        Vbo(Vbo&& t_other) noexcept = delete;
        Vbo& operator=(const Vbo& t_other) = delete;
        Vbo& operator=(Vbo&& t_other) noexcept = delete;

        ~Vbo() noexcept;

        //-------------------------------------------------
        // Bind / unbind
        //-------------------------------------------------

        /**
         * Binds this Vbo handle.
         */
        void Bind() const;

        /**
         * Unbinds a Vbo handle.
         */
        static void Unbind();

        //-------------------------------------------------
        // Data
        //-------------------------------------------------

        static void ReserveMemory(uint32_t t_size);
        static void StoreStaticData(uint32_t t_size, const void* t_data);
        static void StoreData(int32_t t_offset, uint32_t t_size, const void* t_data);

        //-------------------------------------------------
        // Attributes
        //-------------------------------------------------

        /**
         * Specified how OpenGL should interpret the vertex data.
         *
         * @param t_index The location of the vertex attribute.
         * @param t_nrOfFloatComponents The size of the vertex attribute.
         * @param t_nrOfAllFloats The space between consecutive vertex attributes.
         * @param t_startPoint The offset of where the position data begins in the buffer.
         * @param t_instancing Call glVertexAttribDivisor if true.
         */
        static void AddFloatAttribute(
            uint32_t t_index,
            int32_t t_nrOfFloatComponents,
            int32_t t_nrOfAllFloats,
            uint64_t t_startPoint,
            bool t_instancing = false
        );

        /**
         * Specified how OpenGL should interpret the vertex data.
         *
         * @param t_index The location of the vertex attribute.
         * @param t_nrOfIntComponents The size of the vertex attribute.
         * @param t_nrOfAllInts The space between consecutive vertex attributes.
         * @param t_startPoint The offset of where the position data begins in the buffer.
         * @param t_instancing Call glVertexAttribDivisor if true.
         */
        static void AddIntAttribute(
            uint32_t t_index,
            int32_t t_nrOfIntComponents,
            int32_t t_nrOfAllInts,
            uint64_t t_startPoint,
            bool t_instancing
        );

    protected:

    private:
        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        /**
         * Creates a new Vbo handle.
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
