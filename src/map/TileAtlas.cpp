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

#include "TileAtlas.h"
#include "Game.h"
#include "Log.h"
#include "MdciiException.h"
#include "ogl/resource/TextureUtils.h"
#include "ogl/resource/stb_image.h"
#include "ogl/OpenGL.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::map::TileAtlas::TileAtlas()
{
    Log::MDCII_LOG_DEBUG("[TileAtlas::TileAtlas()] Create TileAtlas.");

    Init();
}

mdcii::map::TileAtlas::~TileAtlas() noexcept
{
    Log::MDCII_LOG_DEBUG("[TileAtlas::~TileAtlas()] Destruct TileAtlas.");

    CleanUp();
}

//-------------------------------------------------
// Offset
//-------------------------------------------------

glm::vec2 mdcii::map::TileAtlas::GetTextureOffset(const int t_textureIndex, const int t_nrOfRows)
{
    return { GetTextureXOffset(t_textureIndex, t_nrOfRows), GetTextureYOffset(t_textureIndex, t_nrOfRows) };
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::map::TileAtlas::Init()
{
    magic_enum::enum_for_each<Zoom>([&](const Zoom t_zoom) {
        LoadAtlasImages(t_zoom, IMAGES.at(magic_enum::enum_integer(t_zoom)));
    });
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

float mdcii::map::TileAtlas::GetTextureXOffset(const int t_textureIndex, const int t_nrOfRows)
{
    const auto column{ t_textureIndex % t_nrOfRows };
    return static_cast<float>(column) / static_cast<float>(t_nrOfRows);
}

float mdcii::map::TileAtlas::GetTextureYOffset(const int t_textureIndex, const int t_nrOfRows)
{
    const auto row{ t_textureIndex / t_nrOfRows };
    return static_cast<float>(row) / static_cast<float>(t_nrOfRows);
}

//-------------------------------------------------
// Images
//-------------------------------------------------

void mdcii::map::TileAtlas::LoadAtlasImages(const Zoom t_zoom, const int t_nrOfImages)
{
    Log::MDCII_LOG_DEBUG("[TileAtlas::LoadAtlasImages()] Start creating {} textures from the atlas images for zoom {}...", t_nrOfImages, magic_enum::enum_name(t_zoom));

    std::vector<unsigned char*> images;
    for (auto i{ 0 }; i < t_nrOfImages; ++i)
    {
        int width, height, channels;
        const auto zoomStr{ to_lower_case(std::string(magic_enum::enum_name(t_zoom))) };
        const auto fileName{ Game::RESOURCES_REL_PATH + "atlas/" + zoomStr + "/" + std::to_string(i) + ".png" };

        auto* image{ stbi_load(fileName.c_str(), &width, &height, &channels, 0) };
        if (!image)
        {
            throw MDCII_EXCEPTION("[TileAtlas::LoadAtlasImages()] Image failed to load at path: " + fileName);
        }

        images.push_back(image);
    }

    Log::MDCII_LOG_DEBUG("[TileAtlas::LoadAtlasImages()] {} atlas images were loaded successfully.", images.size());

    CreateTextureArray(t_zoom, images);
    DeleteAtlasImages(t_nrOfImages, images);

    Log::MDCII_LOG_DEBUG("[TileAtlas::LoadAtlasImages()] The textures have been successfully created for zoom {}.", magic_enum::enum_name(t_zoom));
}

void mdcii::map::TileAtlas::DeleteAtlasImages(const int t_nrOfImages, const std::vector<unsigned char*>& t_images)
{
    Log::MDCII_LOG_DEBUG("[TileAtlas::DeleteAtlasImages()] Delete {} images.", t_nrOfImages);

    for (auto i{ 0 }; i < t_nrOfImages; ++i)
    {
        stbi_image_free(t_images.at(i));
    }
}

//-------------------------------------------------
// Texture array
//-------------------------------------------------

void mdcii::map::TileAtlas::CreateTextureArray(const Zoom t_zoom, const std::vector<unsigned char*>& t_images)
{
    const auto id{ ogl::resource::TextureUtils::GenerateNewTextureId() };
    ogl::resource::TextureUtils::Bind(id, GL_TEXTURE_2D_ARRAY);

    const auto zoom{ magic_enum::enum_integer(t_zoom) };
    const auto width{ WIDTHS.at(zoom) };
    const auto height{ HEIGHTS.at(zoom) };
    const auto rows{ ROWS.at(zoom) };
    const auto images{ IMAGES.at(zoom) };

    glTextureStorage3D(
        id,
        1,
        GL_RGBA8,
        static_cast<int32_t>(width) * rows,
        static_cast<int32_t>(height) * rows,
        images
    );

    auto zOffset{ 0 };
    for (const auto* image : t_images)
    {
        glTextureSubImage3D(
            id,
            0,
            0, 0,
            zOffset,
            static_cast<int32_t>(width) * rows,
            static_cast<int32_t>(height) * rows,
            1,
            GL_RGBA,
            GL_UNSIGNED_INT_8_8_8_8_REV,
            image
        );

        zOffset++;
    }

    ogl::resource::TextureUtils::Unbind(GL_TEXTURE_2D_ARRAY);

    textureIds.at(zoom) = id;
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::map::TileAtlas::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[TileAtlas::CleanUp()] Clean up textures.");

    for (const auto id : textureIds)
    {
        ogl::resource::TextureUtils::DeleteTexture(id);
    }
}
