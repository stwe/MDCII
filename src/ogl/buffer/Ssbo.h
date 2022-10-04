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

//-------------------------------------------------
// Ssbo
//-------------------------------------------------

namespace mdcii::ogl::buffer
{
    /**
     * Represents a Shader Storage Buffer Object.
     */
    class Ssbo
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The handle of the Ssbo.
         */
        uint32_t id{ 0 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Ssbo();

        Ssbo(const Ssbo& t_other) = delete;
        Ssbo(Ssbo&& t_other) noexcept = delete;
        Ssbo& operator=(const Ssbo& t_other) = delete;
        Ssbo& operator=(Ssbo&& t_other) noexcept = delete;

        ~Ssbo() noexcept;

        //-------------------------------------------------
        // Bind / unbind
        //-------------------------------------------------

        /**
         * Binds this Ssbo handle.
         */
        void Bind() const;

        /**
         * Unbinds a Ssbo handle.
         */
        static void Unbind();

        //-------------------------------------------------
        // Data
        //-------------------------------------------------

        /**
         * Copy static data to Gpu.
         *
         * @param t_size The size in bytes.
         * @param t_data Pointer to data that will be copied into the Gpu.
         */
        static void StoreStaticData(uint32_t t_size, const void* t_data);

    protected:

    private:
        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        /**
         * Creates a new Ssbo handle.
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
