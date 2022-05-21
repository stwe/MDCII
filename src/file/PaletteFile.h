#pragma once

#include "BinaryFile.h"

//-------------------------------------------------
// PaletteFile
//-------------------------------------------------

namespace mdcii::file
{
    class PaletteFile : public BinaryFile
    {
    public:
        using Color32Bit = int32_t;

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The palette RGB values as int.
         */
        std::vector<Color32Bit> palette;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        PaletteFile() = delete;

        /**
         * Constructs a new PaletteFile object.
         *
         * @param t_filePath The Path to the file.
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

        static constexpr auto NUMBER_OF_COLORS{ 256 };

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        static Color32Bit RgbToInt(uint8_t t_red, uint8_t t_green, uint8_t t_blue);
    };
}
