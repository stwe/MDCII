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

#include "PaletteFile.h"
#include "MdciiAssert.h"
#include "MdciiException.h"
#include "chunk/Chunk.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::resource::PaletteFile::PaletteFile(std::string t_filePath)
    : BinaryFile(std::move(t_filePath))
{
    MDCII_LOG_DEBUG("[PaletteFile::PaletteFile()] Create PaletteFile.");

    if (chunks.at(0)->id.c_str() != CHUNK_ID) // NOLINT(*-redundant-string-cstr)
    {
        throw MDCII_EXCEPTION("[PaletteFile::PaletteFile()] Invalid Chunk Id.");
    }
}

mdcii::resource::PaletteFile::~PaletteFile() noexcept
{
    MDCII_LOG_DEBUG("[PaletteFile::~PaletteFile()] Destruct PaletteFile.");
}

//-------------------------------------------------
// BinaryFile Interface
//-------------------------------------------------

void mdcii::resource::PaletteFile::ReadDataFromChunks()
{
    MDCII_LOG_DEBUG("[PaletteFile::ReadDataFromChunks()] Start reading Palette data from Chunks...");

    const auto& chunk0{ chunks.at(0) };

    for (size_t i{ 0 }; i < chunk0->length; i += 4)
    {
        const auto red{ chunk0->data.at(i) };
        const auto green{ chunk0->data.at(i + 1) };
        const auto blue{ chunk0->data.at(i + 2) };

        palette.emplace_back(red, green, blue);
    }

    MDCII_ASSERT(palette.size() == NUMBER_OF_COLORS, "[PaletteFile::ReadDataFromChunks()] Invalid number of colors.")

    MDCII_LOG_DEBUG("[PaletteFile::ReadDataFromChunks()] Palette data read successfully.");
}
