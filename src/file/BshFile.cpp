#include "BshFile.h"
#include "Log.h"
#include "MdciiException.h"
#include "chunk/Chunk.h"
#include "ogl/OpenGL.h"
#include "ogl/resource/TextureUtils.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::file::BshFile::BshFile(std::string t_filePath, std::vector<PaletteFile::Color32Bit> t_palette)
    : BinaryFile(std::move(t_filePath))
    , m_palette{ std::move(t_palette) }
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
    Log::MDCII_LOG_DEBUG("[BshFile::ReadDataFromChunks()] Start reading BSH pixel data from Chunks...");

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

    CreateGLTextures();

    Log::MDCII_LOG_DEBUG("[BshFile::ReadDataFromChunks()] BSH pixel data read successfully.");
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
    bshTexture->colors.resize(static_cast<size_t>(width) * height);

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
            bshTexture->colors[static_cast<size_t>(y) * width + x] = (uint32_t)0x00000000;
            x++;
        }

        const auto numPixels{ static_cast<uint8_t>(*(offset += sizeof(uint8_t))) };

        auto rgbToInt = [](const uint8_t t_red, const uint8_t t_green, const uint8_t t_blue) -> uint32_t
        {
            uint32_t alpha{ 255 };

            return (alpha << 24) |
                (t_red << 16) |
                (t_green << 8) |
                (t_blue << 0);
        };

        for (auto i{ 0 }; i < numPixels; ++i)
        {
            const auto colorIndex{ static_cast<uint8_t>(*(offset += sizeof(uint8_t))) };
            bshTexture->colors[static_cast<size_t>(y) * width + x] = m_palette[colorIndex];
            x++;
        }
    }

    bshTextures.push_back(std::move(bshTexture));
}

void mdcii::file::BshFile::CreateGLTextures()
{
    for (const auto& texture : bshTextures)
    {
        const auto textureId{ ogl::resource::TextureUtils::GenerateNewTextureId() };

        ogl::resource::TextureUtils::Bind(textureId);
        ogl::resource::TextureUtils::UseNoFilter();

        texture->textureId = textureId;

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            texture->width,
            texture->height,
            0,
            GL_BGRA,
            GL_UNSIGNED_INT_8_8_8_8_REV,
            texture->colors.data()
        );

        ogl::resource::TextureUtils::Unbind();
    }
}
