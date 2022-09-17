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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#include <fstream>
#include "BinaryFile.h"
#include "Log.h"
#include "MdciiException.h"
#include "chunk/Chunk.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::file::BinaryFile::BinaryFile(std::string t_filePath)
    : filePath{ std::move(t_filePath) }
{
    Log::MDCII_LOG_DEBUG("[BinaryFile::BinaryFile()] Create BinaryFile.");

    ReadChunksFromFile();
}

mdcii::file::BinaryFile::~BinaryFile() noexcept
{
    Log::MDCII_LOG_DEBUG("[BinaryFile::~BinaryFile()] Destruct BinaryFile.");
}

//-------------------------------------------------
// Read Chunks
//-------------------------------------------------

void mdcii::file::BinaryFile::ReadChunksFromFile()
{
    Log::MDCII_LOG_DEBUG("[BinaryFile::ReadChunksFromFile()] Reading Chunks from {}.", filePath);

    std::ifstream input(filePath, std::ios::binary);

    if (!input.is_open())
    {
        throw MDCII_EXCEPTION("[BinaryFile::ReadChunksFromFile()] Error while opening file " + filePath + ".");
    }

    while (input.peek() != EOF)
    {
        chunks.emplace_back(std::make_unique<chunk::Chunk>(input));
    }

    input.close();

    if (chunks.empty())
    {
        Log::MDCII_LOG_WARN("[BinaryFile::ReadChunksFromFile()] No Chunks were found.");
    }
    else
    {
        Log::MDCII_LOG_DEBUG("[BinaryFile::ReadChunksFromFile()] {} Chunk(s) were successfully read.", chunks.size());
    }
}
