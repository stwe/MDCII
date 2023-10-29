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

#include "TileAtlas.h"
#include "Game.h"
#include "MdciiException.h"
#include "MdciiUtils.h"
#include "MdciiAssert.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::resource::TileAtlas::TileAtlas()
{
    MDCII_LOG_DEBUG("[TileAtlas::TileAtlas()] Create TileAtlas.");

    Init();
}

mdcii::resource::TileAtlas::~TileAtlas() noexcept
{
    MDCII_LOG_DEBUG("[TileAtlas::~TileAtlas()] Destruct TileAtlas.");
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::resource::TileAtlas::Init()
{
    MDCII_LOG_DEBUG("[TileAtlas::Init()] Start initialization of the Tile Atlas ...");

    LoadAtlasImages();

    for (const auto zoom : magic_enum::enum_values<world::Zoom>())
    {
        LoadHeightsByZoom(zoom);
    }

    MDCII_LOG_DEBUG("[TileAtlas::Init()] Tile Atlas were created successfully.");
}

void mdcii::resource::TileAtlas::LoadAtlasImages()
{
    MDCII_LOG_DEBUG("[TileAtlas::LoadAtlasImages()] Start creating Tile Atlas images ...");

    for (auto i{ 0 }; i < NR_OF_SGFX_ATLAS_IMAGES; ++i)
    {
        auto renderable{ std::make_unique<olc::Renderable>() };
        if(renderable->Load(fmt::format("{}atlas/sgfx/stadtfld/{}.png", Game::RESOURCES_REL_PATH, i)) != olc::OK)
        {
            throw MDCII_EXCEPTION("[TileAtlas::LoadAtlasImages()] Error while loading file.");
        }
        MDCII_ASSERT(renderable->Decal(), "[TileAtlas::LoadAtlasImages()] Null pointer.")
        atlas.at(magic_enum::enum_integer(world::Zoom::SGFX)).push_back(std::move(renderable));
    }

    for (auto i{ 0 }; i < NR_OF_MGFX_ATLAS_IMAGES; ++i)
    {
        auto renderable{ std::make_unique<olc::Renderable>() };
        if (renderable->Load(fmt::format("{}atlas/mgfx/stadtfld/{}.png", Game::RESOURCES_REL_PATH, i)) != olc::OK)
        {
            throw MDCII_EXCEPTION("[TileAtlas::LoadAtlasImages()] Error while loading file.");
        }
        MDCII_ASSERT(renderable->Decal(), "[TileAtlas::LoadAtlasImages()] Null pointer.")
        atlas.at(magic_enum::enum_integer(world::Zoom::MGFX)).push_back(std::move(renderable));
    }

    for (auto i{ 0 }; i < NR_OF_GFX_ATLAS_IMAGES; ++i)
    {
        auto renderable{ std::make_unique<olc::Renderable>() };
        if (renderable->Load(fmt::format("{}atlas/gfx/stadtfld/{}.png", Game::RESOURCES_REL_PATH, i)) != olc::OK)
        {
            throw MDCII_EXCEPTION("[TileAtlas::LoadAtlasImages()] Error while loading file.");
        }
        MDCII_ASSERT(renderable->Decal(), "[TileAtlas::LoadAtlasImages()] Null pointer.")
        atlas.at(magic_enum::enum_integer(world::Zoom::GFX)).push_back(std::move(renderable));
    }

    MDCII_LOG_DEBUG("[TileAtlas::LoadAtlasImages()] Tile Atlas images were created successfully.");
}

void mdcii::resource::TileAtlas::LoadHeightsByZoom(const world::Zoom t_zoom)
{
    MDCII_LOG_DEBUG("[TileAtlas::LoadHeightsByZoom()] Start reading image heights for zoom {} ...", magic_enum::enum_name(t_zoom));

    std::ifstream inputFile(
        fmt::format("{}atlas/{}/stadtfld/heights.txt",
        Game::RESOURCES_REL_PATH, to_lower_case(std::string(magic_enum::enum_name(t_zoom)))
        )
    );
    std::string line;

    const auto zoomInt{ magic_enum::enum_integer(t_zoom) };

    if (inputFile.is_open())
    {
        while (std::getline(inputFile, line))
        {
            if (!line.empty())
            {
                heights.at(zoomInt).push_back(std::stoi(line));
            }
        }

        inputFile.close();
    }
    else
    {
        throw MDCII_EXCEPTION("[TileAtlas::LoadHeightsByZoom()] Error while reading file path: " +
           fmt::format("{}atlas/{}/stadtfld/heights.txt",
               Game::RESOURCES_REL_PATH, magic_enum::enum_name(t_zoom)
           )
        );
    }

    MDCII_LOG_DEBUG("[TileAtlas::LoadHeightsByZoom()] Image heights were read successfully.");
}
