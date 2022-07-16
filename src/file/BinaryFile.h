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

#include <memory>
#include <string>
#include <vector>

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::chunk
{
    class Chunk;
}

//-------------------------------------------------
// BinaryFile
//-------------------------------------------------

namespace mdcii::file
{
    /**
     * Common stuff for all BinaryFile objects.
     */
    class BinaryFile
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The path to the file to load.
         */
        std::string filePath;

        /**
         * Each file has one or many Chunk objects.
         */
        std::vector<std::unique_ptr<chunk::Chunk>> chunks;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        BinaryFile() = delete;

        /**
         * Constructs a new BinaryFile object from a given path.
         *
         * @param t_filePath The path to the file to load.
         */
        explicit BinaryFile(std::string t_filePath);

        BinaryFile(const BinaryFile& t_other) = delete;
        BinaryFile(BinaryFile&& t_other) noexcept = delete;
        BinaryFile& operator=(const BinaryFile& t_other) = delete;
        BinaryFile& operator=(BinaryFile&& t_other) noexcept = delete;

        virtual ~BinaryFile() noexcept;

        //-------------------------------------------------
        // Read Chunks
        //-------------------------------------------------

        /**
         * Opens and reads the file.
         * Chunk objects are created from the file content.
         */
        void ReadChunksFromFile();

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        /**
         * Reads the data from the Chunk objects.
         */
        virtual void ReadDataFromChunks() = 0;

    protected:

    private:

    };
}
