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

#include "Game.h"
#include "Log.h"
#include "resource/MdciiFile.h"
#include "resource/OriginalResourcesManager.h"
#include "world/Island.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::Game::Game()
{
    MDCII_LOG_DEBUG("[Game::Game()] Create Game.");

    sAppName = "MDCII";
}

mdcii::Game::~Game()
{
    MDCII_LOG_DEBUG("[Game::~Game()] Destruct Game.");
}

bool mdcii::Game::OnUserCreate()
{
    m_origResMng = std::make_unique<resource::OriginalResourcesManager>();

    m_sprGreen = std::make_unique<olc::Sprite>("resources/textures/gfx/corner_gfx.png");
    m_decGreen = std::make_unique<olc::Decal>(m_sprGreen.get());

    m_sprAtlas0 = std::make_unique<olc::Sprite>("resources/atlas/gfx/stadtfld/0.png");
    m_decAtlas0 = std::make_unique<olc::Decal>(m_sprAtlas0.get());

    if (resource::MdciiFile file{"resources/sav/Example.sav" }; file.LoadJsonFromFile())
    {
        m_islands = file.CreateIslands();
    }

    return true;
}

bool mdcii::Game::OnUserUpdate(float t_elapsedTime)
{
    Clear(olc::BLACK);

    auto toScreen = [&](int t_x, int t_y)
    {
        return olc::vi2d
            {
                (8 * 64) + (t_x - t_y) * (64 / 2),
                (5 * 32) + (t_x + t_y) * (32 / 2)
            };
    };

    for (auto y{ 0 }; y < m_islands.at(0)->height; ++y)
    {
        for (auto x{ 0 }; x < m_islands.at(0)->width; ++x)
        {
            //position.y * width + position.x;
            DrawDecal(toScreen(x, y), m_decGreen.get());
            DrawPartialDecal(toScreen(x, y), m_decAtlas0.get(), olc::vf2d(0 * 64, 0 * 32), olc::vf2d(64, 32));
        }
    }

    return true;
}
