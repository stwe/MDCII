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

#include "BinaryFile.h"

//-------------------------------------------------
// PaletteFile
//-------------------------------------------------

namespace mdcii::file
{
    /**
     * Represents a PaletteFile.
     * The class loads the pallet values from the from the
     * original game's stadtfld.col file.
     */
    class PaletteFile : public BinaryFile
    {
    public:
        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        using Color32Bit = uint32_t;

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The palette RGBA values as int.
         */
        std::vector<Color32Bit> palette;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        PaletteFile() = delete;

        /**
         * Constructs a new PaletteFile object.
         *
         * @param t_filePath The path to the file.
         */
        explicit PaletteFile(const std::string& t_filePath);

        PaletteFile(const PaletteFile& t_other) = delete;
        PaletteFile(PaletteFile&& t_other) noexcept = delete;
        PaletteFile& operator=(const PaletteFile& t_other) = delete;
        PaletteFile& operator=(PaletteFile&& t_other) noexcept = delete;

        ~PaletteFile() noexcept override;

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        /**
         * Reads the data from the Chunk objects.
         */
        void ReadDataFromChunks() override;

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * The number of palette colors.
         */
        static constexpr auto NUMBER_OF_COLORS{ 256 };

        /**
         * The file/chunk Id.
         */
        static constexpr std::string_view CHUNK_ID{ "COL" };

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Stores the 8bit RGBA values in a 32bit int.
         */
        static Color32Bit RgbTo32BitInt(uint8_t t_red, uint8_t t_green, uint8_t t_blue);
    };
}
