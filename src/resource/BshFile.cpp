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

#include "BshFile.h"
#include "Log.h"
#include "MdciiException.h"
#include "chunk/Chunk.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::resource::BshFile::BshFile(std::string t_filePath, std::vector<olc::Pixel> t_palette)
    : BinaryFile(std::move(t_filePath))
    , m_palette{ std::move(t_palette) }
{
    MDCII_LOG_DEBUG("[BshFile::BshFile()] Create BshFile.");

    if (chunks.at(0)->id.c_str() != CHUNK_ID) // NOLINT(*-redundant-string-cstr)
    {
        throw MDCII_EXCEPTION("[BshFile::BshFile()] Invalid Chunk Id.");
    }
}

mdcii::resource::BshFile::~BshFile() noexcept
{
    MDCII_LOG_DEBUG("[BshFile::~BshFile()] Destruct BshFile.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::resource::BshFile::ReadDataFromChunks()
{
    MDCII_LOG_DEBUG("[BshFile::ReadDataFromChunks()] Start reading BSH pixel data from Chunks...");

    // get pointer to the first element
    const auto* dataPtr{ reinterpret_cast<const uint32_t*>(chunks.at(0)->data.data()) };

    // read and store the first offset
    const auto firstOffset{ *dataPtr };
    m_offsets.push_back(firstOffset);

    // calc number of textures
    const auto count{ firstOffset / 4u };

    // store other offsets
    for (auto i{ 1u }; i < count; ++i)
    {
        m_offsets.push_back(dataPtr[i]);
    }

    MDCII_LOG_DEBUG("[BshFile::ReadDataFromChunks()] Detected {} offsets.", m_offsets.size());

    // create BshTexture objects
    for (const auto offset : m_offsets)
    {
        DecodePixelData(offset);
    }

    // create renderables
    CreateRenderables();

    MDCII_LOG_DEBUG("[BshFile::ReadDataFromChunks()] BSH pixel data read successfully.");
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::resource::BshFile::DecodePixelData(const uint32_t t_offset)
{
    const auto* offset{ &chunks.at(0)->data[t_offset] };

    const auto width{ *reinterpret_cast<const uint32_t*>(offset) };
    const auto height{ *reinterpret_cast<const uint32_t*>((offset += sizeof(uint32_t))) };
    const auto type{ *reinterpret_cast<const uint32_t*>((offset += sizeof(uint32_t))) };
    const auto len{ *reinterpret_cast<const uint32_t*>((offset += sizeof(uint32_t))) };

    if (width <= 0 || height <= 0)
    {
        throw MDCII_EXCEPTION("[BshFile::DecodePixelData()] Invalid width or height.");
    }

    auto bshTexture{ std::make_unique<BshTexture>() };
    bshTexture->width = width;
    bshTexture->height = height;
    bshTexture->pixels.resize(static_cast<size_t>(width) * height, { 0, 0, 0, 0 });

    auto x{ 0 };
    auto y{ 0 };

    offset += 3;

    while (true)
    {
        const auto numAlpha{ static_cast<uint8_t>(*(offset += sizeof(uint8_t))) };

        if (numAlpha == END_MARKER)
        {
            break;
        }

        if (numAlpha == END_OF_ROW)
        {
            x = 0;
            y++;
            continue;
        }

        for (auto i{ 0 }; i < numAlpha; ++i)
        {
            x++;
        }

        const auto numPixels{ static_cast<uint8_t>(*(offset += sizeof(uint8_t))) };
        for (auto i{ 0 }; i < numPixels; ++i)
        {
            const auto colorIndex{ static_cast<uint8_t>(*(offset += sizeof(uint8_t))) };
            bshTexture->pixels[static_cast<size_t>(y) * width + x] = m_palette[colorIndex];
            x++;
        }
    }

    bshTextures.push_back(std::move(bshTexture));
}

void mdcii::resource::BshFile::CreateRenderables() const
{
    for (const auto& texture : bshTextures)
    {
        texture->sprite = std::make_unique<olc::Sprite>(texture->width, texture->height);

        for (int y{ 0 }; y < static_cast<int>(texture->height); ++y)
        {
            for (int x{ 0 }; x < static_cast<int>(texture->width); ++x)
            {
                texture->sprite->SetPixel(x, y, texture->pixels[static_cast<size_t>(y) * texture->width + x]);
            }
        }

        texture->decal = std::make_unique<olc::Decal>(texture->sprite.get());
    }
}
