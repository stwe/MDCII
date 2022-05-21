#include "BshFile.h"
#include "Log.h"
#include "MdciiException.h"
#include "chunk/Chunk.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::file::BshFile::BshFile(std::string t_filePath)
    : BinaryFile(std::move(t_filePath))
{
    Log::MDCII_LOG_DEBUG("[BshFile::BshFile()] Create BshFile.");
}

mdcii::file::BshFile::~BshFile() noexcept
{
    Log::MDCII_LOG_DEBUG("[BshFile::~BshFile()] Destruct BshFile.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::file::BshFile::ReadDataFromChunks()
{
    Log::MDCII_LOG_DEBUG("[BshFile::ReadDataFromChunks()] Start reading BSH data from Chunks...");

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

    Log::MDCII_LOG_DEBUG("[BshFile::ReadDataFromChunks()] Detected {} offsets.", m_offsets.size());

    // create a BshTexture objects
    for (const auto offset : m_offsets)
    {
        DecodePixelData(offset);
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::file::BshFile::DecodePixelData(const uint32_t t_offset)
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
    bshTexture->paletteIndices.resize(static_cast<size_t>(width) * height);

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
            bshTexture->paletteIndices[static_cast<size_t>(y) * width + x] = 0;
            x++;
        }

        const auto numPixels{ static_cast<uint8_t>(*(offset += sizeof(uint8_t))) };

        for (auto i{ 0 }; i < numPixels; ++i)
        {
            const auto colorIndex{ static_cast<uint8_t>(*(offset += sizeof(uint8_t))) };
            bshTexture->paletteIndices[static_cast<size_t>(y) * width + x] = colorIndex;
            x++;
        }
    }

    bshTextures.push_back(std::move(bshTexture));
}
