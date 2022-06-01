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
