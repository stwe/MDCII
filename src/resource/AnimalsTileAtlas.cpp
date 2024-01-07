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

#include "AnimalsTileAtlas.h"
#include "Game.h"
#include "MdciiException.h"
#include "MdciiUtils.h"
#include "MdciiAssert.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::resource::AnimalsTileAtlas::AnimalsTileAtlas()
{
    MDCII_LOG_DEBUG("[AnimalsTileAtlas::AnimalsTileAtlas()] Create AnimalsTileAtlas.");

    Init();
}

mdcii::resource::AnimalsTileAtlas::~AnimalsTileAtlas() noexcept
{
    MDCII_LOG_DEBUG("[AnimalsTileAtlas::~AnimalsTileAtlas()] Destruct AnimalsTileAtlas.");
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::resource::AnimalsTileAtlas::Init()
{
    MDCII_LOG_DEBUG("[AnimalsTileAtlas::Init()] Start initialization of the Animals Tile Atlas ...");

    LoadAtlasImages();

    for (const auto zoom : magic_enum::enum_values<world::Zoom>())
    {
        LoadHeightsByZoom(zoom);
    }

    MDCII_LOG_DEBUG("[AnimalsTileAtlas::Init()] Animals Tile Atlas were created successfully.");
}

//-------------------------------------------------
// Load
//-------------------------------------------------

void mdcii::resource::AnimalsTileAtlas::LoadAtlasImages()
{
    MDCII_LOG_DEBUG("[AnimalsTileAtlas::LoadAtlasImages()] Start creating Animals Tile Atlas images ...");

    for (auto i{ 0 }; i < NR_OF_SGFX_ATLAS_IMAGES; ++i)
    {
        auto renderable{ std::make_unique<olc::Renderable>() };
        if(renderable->Load(fmt::format("{}atlas/sgfx/{}/{}.png", Game::RESOURCES_REL_PATH, TILE_ATLAS_NAME, i)) != olc::OK)
        {
            throw MDCII_EXCEPTION("[AnimalsTileAtlas::LoadAtlasImages()] Error while loading file.");
        }
        MDCII_ASSERT(renderable->Decal(), "[AnimalsTileAtlas::LoadAtlasImages()] Null pointer.")
        atlas.at(magic_enum::enum_integer(world::Zoom::SGFX)).push_back(std::move(renderable));
    }

    for (auto i{ 0 }; i < NR_OF_MGFX_ATLAS_IMAGES; ++i)
    {
        auto renderable{ std::make_unique<olc::Renderable>() };
        if (renderable->Load(fmt::format("{}atlas/mgfx/{}/{}.png", Game::RESOURCES_REL_PATH, TILE_ATLAS_NAME, i)) != olc::OK)
        {
            throw MDCII_EXCEPTION("[AnimalsTileAtlas::LoadAtlasImages()] Error while loading file.");
        }
        MDCII_ASSERT(renderable->Decal(), "[AnimalsTileAtlas::LoadAtlasImages()] Null pointer.")
        atlas.at(magic_enum::enum_integer(world::Zoom::MGFX)).push_back(std::move(renderable));
    }

    for (auto i{ 0 }; i < NR_OF_GFX_ATLAS_IMAGES; ++i)
    {
        auto renderable{ std::make_unique<olc::Renderable>() };
        if (renderable->Load(fmt::format("{}atlas/gfx/{}/{}.png", Game::RESOURCES_REL_PATH, TILE_ATLAS_NAME, i)) != olc::OK)
        {
            throw MDCII_EXCEPTION("[AnimalsTileAtlas::LoadAtlasImages()] Error while loading file.");
        }
        MDCII_ASSERT(renderable->Decal(), "[AnimalsTileAtlas::LoadAtlasImages()] Null pointer.")
        atlas.at(magic_enum::enum_integer(world::Zoom::GFX)).push_back(std::move(renderable));
    }

    MDCII_LOG_DEBUG("[AnimalsTileAtlas::LoadAtlasImages()] Animals Tile Atlas images were created successfully.");
}

void mdcii::resource::AnimalsTileAtlas::LoadHeightsByZoom(mdcii::world::Zoom t_zoom)
{
    MDCII_LOG_DEBUG("[AnimalsTileAtlas::LoadHeightsByZoom()] Start reading image heights for zoom {} ...", magic_enum::enum_name(t_zoom));

    std::ifstream inputFile(
            fmt::format("{}atlas/{}/{}/heights.txt",
               Game::RESOURCES_REL_PATH, to_lower_case(std::string(magic_enum::enum_name(t_zoom))), TILE_ATLAS_NAME
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
        throw MDCII_EXCEPTION("[AnimalsTileAtlas::LoadHeightsByZoom()] Error while reading file path: " +
                              fmt::format("{}atlas/{}/{}/heights.txt",
                              Game::RESOURCES_REL_PATH, magic_enum::enum_name(t_zoom), TILE_ATLAS_NAME
                              )
        );
    }

    MDCII_LOG_DEBUG("[AnimalsTileAtlas::LoadHeightsByZoom()] Image heights were read successfully.");
}
