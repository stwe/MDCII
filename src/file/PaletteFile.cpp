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
