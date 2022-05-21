#include "PaletteFile.h"
#include "MdciiAssert.h"
#include "MdciiException.h"
#include "chunk/Chunk.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::file::PaletteFile::PaletteFile(const std::string& t_filePath)
    : BinaryFile(t_filePath)
{
    Log::MDCII_LOG_DEBUG("[PaletteFile::PaletteFile()] Create PaletteFile.");

    if (chunks.at(0)->id.c_str() != CHUNK_ID)
    {
        throw MDCII_EXCEPTION("[PaletteFile::PaletteFile()] Invalid Chunk Id.");
    }
}

mdcii::file::PaletteFile::~PaletteFile() noexcept
{
    Log::MDCII_LOG_DEBUG("[PaletteFile::~PaletteFile()] Destruct PaletteFile.");
}

//-------------------------------------------------
// BinaryFile Interface
//-------------------------------------------------

void mdcii::file::PaletteFile::ReadDataFromChunks()
{
    Log::MDCII_LOG_DEBUG("[PaletteFile::ReadDataFromChunks()] Start reading Palette data from Chunks...");

    const auto& chunk0{ chunks.at(0) };

    for (size_t i{ 0 }; i < chunk0->length; i += 4)
    {
        const auto red{ chunk0->data.at(i) };
        const auto green{ chunk0->data.at(i + 1) };
        const auto blue{ chunk0->data.at(i + 2) };

        palette.push_back(RgbTo32BitInt(red, green, blue));
    }

    MDCII_ASSERT(palette.size() == NUMBER_OF_COLORS, "[PaletteFile::ReadDataFromChunks()] Invalid number of colors.")

    Log::MDCII_LOG_DEBUG("[PaletteFile::ReadDataFromChunks()] Palette data read successfully.");
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

mdcii::file::PaletteFile::Color32Bit mdcii::file::PaletteFile::RgbTo32BitInt(const uint8_t t_red, const uint8_t t_green, const uint8_t t_blue)
{
    constexpr Color32Bit alpha{ 255 };

    return (alpha << 24) |
        (t_red << 16) |
        (t_green << 8) |
        (t_blue);
}
