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

#include "BinaryFile.h"
#include "vendor/olc/olcPixelGameEngine.h"

namespace mdcii::resource
{
    //-------------------------------------------------
    // BshTexture
    //-------------------------------------------------

    /**
     * @brief A Bsh graphic of the original game.
     */
    struct BshTexture
    {
        /**
         * @brief The gfx pixels.
         */
        std::vector<olc::Pixel> pixels;

        /**
         * @brief The gfx width.
         */
        uint32_t width{ 0 };

        /**
         * @brief The gfx height.
         */
        uint32_t height{ 0 };

        /**
         * @brief Gfx stored as an Olc Sprite.
         */
        std::unique_ptr<olc::Sprite> sprite;

        /**
         * @brief Gfx stored as an Olc Decal.
         */
        std::unique_ptr<olc::Decal> decal;
    };

    //-------------------------------------------------
    // BshFile
    //-------------------------------------------------

    /**
     * @brief The class loads a Bsh file.
     */
    class BshFile : public BinaryFile
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief A list with BshTexture objects.
         */
        std::vector<std::unique_ptr<BshTexture>> bshTextures;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        BshFile() = delete;

        /**
         * @brief Constructs a new BshFile object.
         *
         * @param t_filePath The path to the Bsh file.
         * @param t_palette The palette containing the RGBA values of each color.
         */
        explicit BshFile(std::string t_filePath, std::vector<olc::Pixel> t_palette);

        BshFile(const BshFile& t_other) = delete;
        BshFile(BshFile&& t_other) noexcept = delete;
        BshFile& operator=(const BshFile& t_other) = delete;
        BshFile& operator=(BshFile&& t_other) noexcept = delete;

        ~BshFile() noexcept override;

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        /**
         * @brief Reads the data from the Chunk objects.
         */
        void ReadDataFromChunks() override;

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * @brief If the next byte is 0xFF the image has reached its end.
         */
        static constexpr uint8_t END_MARKER{ 255 };

        /**
         * @brief If the next byte is 0xFE the current pixel line has reached its end.
         */
        static constexpr uint8_t END_OF_ROW{ 254 };

        /**
         * @brief The file/chunk Id.
         */
        static constexpr std::string_view CHUNK_ID{ "BSH" };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The palette containing the RGBA values as 32bit integer of each color.
         */
        std::vector<olc::Pixel> m_palette;

        /**
         * @brief The offsets to the Bsh images.
         */
        std::vector<uint32_t> m_offsets;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * @brief Reads the pixel values from the first Chunk and uses it to create BshTexture objects.
         *
         * The BshTexture objects are stored in bshTextures.
         *
         * @param t_offset The offset to a Bsh image.
         */
        void DecodePixelData(uint32_t t_offset);

        /**
         * @brief Makes the BshTexture pixel data accessible.
         */
        void CreateRenderables() const;
    };
}
