#pragma once

#include "BinaryFile.h"

namespace mdcii::file
{
    //-------------------------------------------------
    // BshTexture
    //-------------------------------------------------

    struct BshTexture
    {
        /**
         * The pixel value that is an index of the palette that contains rgb values.
         */
        std::vector<uint8_t> paletteIndices;

        /**
         * The width of the texture.
         */
        uint32_t width{ 0 };

        /**
         * The height of the texture.
         */
        uint32_t height{ 0 };
    };


    //-------------------------------------------------
    // BshFile
    //-------------------------------------------------

    class BshFile : public BinaryFile
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
         * @param t_filePath The Path to the Bsh file.
         */
        explicit BshFile(std::string t_filePath);

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
         * If the next byte is 0xFF the image has reached it’s end.
         */
        static constexpr uint8_t END_MARKER{ 255 };

        /**
         * If the next byte is 0xFE the current pixel line has reached it’s end.
         */
        static constexpr uint8_t END_OF_ROW{ 254 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

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
    };
}
