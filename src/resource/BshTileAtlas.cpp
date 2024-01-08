// This file is part of the MDCII project.
//
// Copyright (c) 2024. stwe <https://github.com/stwe/MDCII>
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

#include "BshTileAtlas.h"
#include "Game.h"
#include "MdciiException.h"
#include "MdciiUtils.h"
#include "MdciiAssert.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::resource::BshTileAtlas::BshTileAtlas(
    std::string_view t_atlasPath,
    const int t_sgfxImages,
    const int t_mgfxImages,
    const int t_gfxImages
)
    : m_atlasPath{ t_atlasPath }
{
    MDCII_LOG_DEBUG("[BshTileAtlas::BshTileAtlas()] Create BshTileAtlas.");

    MDCII_ASSERT(t_sgfxImages > 0, "[BshTileAtlas::BshTileAtlas()] Invalid number of images.")
    MDCII_ASSERT(t_mgfxImages > 0, "[BshTileAtlas::BshTileAtlas()] Invalid number of images.")
    MDCII_ASSERT(t_gfxImages > 0, "[BshTileAtlas::BshTileAtlas()] Invalid number of images.")

    m_nrImages.at(magic_enum::enum_integer(world::Zoom::SGFX)) = t_sgfxImages;
    m_nrImages.at(magic_enum::enum_integer(world::Zoom::MGFX)) = t_mgfxImages;
    m_nrImages.at(magic_enum::enum_integer(world::Zoom::GFX)) = t_gfxImages;

    Init();
}

mdcii::resource::BshTileAtlas::~BshTileAtlas() noexcept
{
    MDCII_LOG_DEBUG("[BshTileAtlas::~BshTileAtlas()] Destruct BshTileAtlas.");
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

int mdcii::resource::BshTileAtlas::GetAtlasIndex(const int t_gfx, const int t_rows)
{
    return t_gfx / (t_rows * t_rows);
}

olc::vi2d mdcii::resource::BshTileAtlas::GetAtlasOffset(const int t_gfx, const int t_rows)
{
    auto picInPic{ t_gfx % (t_rows * t_rows) };

    return { picInPic % t_rows, picInPic / t_rows };
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::resource::BshTileAtlas::Init()
{
    MDCII_LOG_DEBUG("[BshTileAtlas::Init()] Start initialization of the Bsh Tile Atlas ...");

    LoadAtlasImages();

    for (const auto zoom : magic_enum::enum_values<world::Zoom>())
    {
        LoadHeightsByZoom(zoom);
    }

    MDCII_LOG_DEBUG("[BshTileAtlas::Init()] The Bsh Tile Atlas were initialized successfully.");
}

//-------------------------------------------------
// Load
//-------------------------------------------------

void mdcii::resource::BshTileAtlas::LoadAtlasImages()
{
    MDCII_LOG_DEBUG("[BshTileAtlas::LoadAtlasImages()] Start creating Bsh Tile Atlas images ...");

    LoadAtlasImagesByZoom(world::Zoom::SGFX);
    LoadAtlasImagesByZoom(world::Zoom::MGFX);
    LoadAtlasImagesByZoom(world::Zoom::GFX);

    MDCII_LOG_DEBUG("[BshTileAtlas::LoadAtlasImages()] The Bsh Tile Atlas images were created successfully.");
}

void mdcii::resource::BshTileAtlas::LoadAtlasImagesByZoom(const world::Zoom t_zoom)
{
    const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
    const auto zoomStr{ to_lower_case(std::string(magic_enum::enum_name(t_zoom))) };

    MDCII_LOG_DEBUG("[BshTileAtlas::LoadAtlasImagesByZoom()] Creating Bsh Tile Atlas images for zoom {}.", zoomStr);

    for (auto i{ 0 }; i < m_nrImages.at(zoomInt); ++i)
    {
        auto renderable{ std::make_unique<olc::Renderable>() };
        if(renderable->Load(fmt::format("{}atlas/{}/{}/{}.png", Game::RESOURCES_REL_PATH, zoomStr, m_atlasPath, i)) != olc::OK)
        {
            throw MDCII_EXCEPTION("[BshTileAtlas::LoadAtlasImagesByZoom()] Error while loading Bsh Tile Atlas file.");
        }

        MDCII_ASSERT(renderable->Decal(), "[BshTileAtlas::LoadAtlasImagesByZoom()] Null pointer.")

        m_atlas.at(zoomInt).push_back(std::move(renderable));
    }
}

void mdcii::resource::BshTileAtlas::LoadHeightsByZoom(const world::Zoom t_zoom)
{
    MDCII_LOG_DEBUG("[BshTileAtlas::LoadHeightsByZoom()] Start reading image heights for zoom {} ...", magic_enum::enum_name(t_zoom));

    const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
    const auto zoomStr{ to_lower_case(std::string(magic_enum::enum_name(t_zoom))) };

    const auto fullPath{ fmt::format("{}atlas/{}/{}/heights.txt", Game::RESOURCES_REL_PATH, zoomStr, m_atlasPath) };
    std::ifstream inputFile(fullPath);
    std::string line;

    if (inputFile.is_open())
    {
        while (std::getline(inputFile, line))
        {
            if (!line.empty())
            {
                m_heights.at(zoomInt).push_back(std::stoi(line));
            }
        }

        inputFile.close();
    }
    else
    {
        throw MDCII_EXCEPTION("[BshTileAtlas::LoadHeightsByZoom()] Error while reading file path: " + fullPath);
    }

    MDCII_LOG_DEBUG("[BshTileAtlas::LoadHeightsByZoom()] Image heights were read successfully.");
}
