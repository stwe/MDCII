#pragma once

#include "BinaryFile.h"
#include "PaletteFile.h"

namespace mdcii::file
{
    //-------------------------------------------------
    // BshTexture
    //-------------------------------------------------

    struct BshTexture
    {
        std::vector<PaletteFile::Color32Bit> pixel;
        uint32_t width{ 0 };
        uint32_t height{ 0 };
        uint32_t textureId{ 0 };
    };


    //-------------------------------------------------
    // BshFile
    //-------------------------------------------------

    /**
     * The class loads a Bsh file.
     */
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
         * If the next byte is 0xFF the image has reached it’s end.
         */
        static constexpr uint8_t END_MARKER{ 255 };

        /**
         * If the next byte is 0xFE the current pixel line has reached it’s end.
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
