// This file is part of the MDCII project.
//
// Copyright (c) 2023. stwe <https://github.com/stwe/MDCII>
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
#include <string>
#include <vector>

//-------------------------------------------------
// Chunk
//-------------------------------------------------

namespace mdcii::chunk
{
    /**
     * @brief Represents a Chunk.
     *
     * The Chunks file format is used for all binary type files.
     * The Chunk header consists of a type Id as an ASCII string that is padded with
     * junk to fill a full 16 bytes. Then comes a 4-byte integer that indicates the size
     * of the data block without the header.
     */
    class Chunk
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The Chunk type identifier.
         */
        std::string id;

        /**
         * @brief The size of the Chunk, excluding the header.
         */
        uint32_t length{ 0u };

        /**
         * @brief The Chunk data.
         */
        std::vector<uint8_t> data;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Chunk() = delete;

        /**
         * @brief Constructs a new Chunk object.
         *
         * @param t_input The content of a file.
         */
        explicit Chunk(std::ifstream& t_input);

        Chunk(const Chunk& t_other) = delete;
        Chunk(Chunk&& t_other) noexcept = delete;
        Chunk& operator=(const Chunk& t_other) = delete;
        Chunk& operator=(Chunk&& t_other) noexcept = delete;

        ~Chunk() noexcept;

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * @brief The size in bytes of the Chunk type identifier.
         */
        static constexpr auto ID_SIZE_IN_BYTES{ 16 };
    };
}
