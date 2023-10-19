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
#include "Log.h"
#include "Game.h"

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
    MDCII_LOG_DEBUG("[TileAtlas::Init()] Start creating atlas images ...");

    for (auto i{ 0 }; i < NR_OF_GFX_ATLAS_IMAGES; ++i)
    {
        const auto path{ Game::RESOURCES_REL_PATH + "atlas/gfx/stadtfld/"  + std::to_string(i) + ".png" };
        auto spr{ std::make_unique<olc::Sprite>(path) };
        auto dec{ std::make_unique<olc::Decal>(spr.get()) };

        gfxSprAtlas.at(i) = std::move(spr);
        gfxDecAtlas.at(i) = std::move(dec);
    }

    std::ifstream inputFile(Game::RESOURCES_REL_PATH + "atlas/gfx/stadtfld/heights.txt" );
    std::string line;

    if (inputFile.is_open())
    {
        while (std::getline(inputFile, line))
        {
            if (!line.empty())
            {
                gfxHeights.push_back(std::stoi(line));
            }
        }

        inputFile.close();
    }
    else
    {
        // todo exception
    }

    MDCII_LOG_DEBUG("[TileAtlas::Init()] Atlas images were created successfully.");
}
