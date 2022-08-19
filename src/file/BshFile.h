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
#include "PaletteFile.h"

namespace mdcii::file
{
    //-------------------------------------------------
    // BshTexture
    //-------------------------------------------------

    /**
     * A bsh graphic of the original game.
     */
    struct BshTexture
    {
        /**
         * The pixels of the graphic or the palette index.
         */
        std::vector<PaletteFile::Color32Bit> pixel;

        /**
         * The gfx width.
         */
        uint32_t width{ 0 };

        /**
         * The gfx height.
         */
        uint32_t height{ 0 };

        /**
         * The OpenGL texture handle of this gfx.
         * So this graphic is accessible to the GPU.
         */
        uint32_t textureId{ 0 };
    };


    //-------------------------------------------------
    // BshFile
    //-------------------------------------------------

    /**
     * The class loads a Bsh file.
     */
    class BshFile: public BinaryFile
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * A list with BshTexture objects.
         */
        std::vector<std::unique_ptr<BshTexture>> bshTextures;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        BshFile() = delete;

        /**
         * Constructs a new BshFile object.
         *
         * @param t_filePath The path to the Bsh file.
         * @param t_palette The palette containing the RGBA values as 32bit integer of each color.
         */
        explicit BshFile(std::string t_filePath, std::vector<PaletteFile::Color32Bit> t_palette);

        BshFile(const BshFile& t_other) = delete;
        BshFile(BshFile&& t_other) noexcept = delete;
        BshFile& operator=(const BshFile& t_other) = delete;
        BshFile& operator=(BshFile&& t_other) noexcept = delete;

        ~BshFile() noexcept override;

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
         * If the next byte is 0xFF the image has reached it�s end.
         */
        static constexpr uint8_t END_MARKER{ 255 };

        /**
         * If the next byte is 0xFE the current pixel line has reached it�s end.
         */
        static constexpr uint8_t END_OF_ROW{ 254 };

        /**
         * The file/chunk Id.
         */
        static constexpr std::string_view CHUNK_ID{ "BSH" };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The palette containing the RGBA values as 32bit integer of each color.
         */
        std::vector<PaletteFile::Color32Bit> m_palette;

        /**
         * The offsets to the Bsh images.
         */
        std::vector<uint32_t> m_offsets;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Reads the pixel values from the first Chunk and uses it to create BshTexture objects.
         * The BshTexture objects are stored in bshTextures.
         *
         * @param t_offset The offset to a Bsh image.
         */
        void DecodePixelData(uint32_t t_offset);

        /**
         * Makes the BshTexture pixel data accessible to the GPU.
         */
        void CreateGLTextures() const;

        //-------------------------------------------------
        // CleanUp
        //-------------------------------------------------

        /**
         * Clean up OpenGL / delete textures.
         */
        void CleanUp() const;
    };
}
