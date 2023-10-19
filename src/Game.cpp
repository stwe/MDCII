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
#include "resource/TileAtlas.h"
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

    if (resource::MdciiFile file{ "resources/sav/Example.sav" }; file.LoadJsonFromFile())
    {
        m_islands = file.CreateIslands();
    }

    m_tileAtlas = std::make_unique<resource::TileAtlas>();

    for (auto const& island : m_islands)
    {
        for (auto& tile : island->tiles)
        {
            if (tile.HasBuilding())
            {
                const auto& building{ m_origResMng->GetBuildingById(tile.buildingId) };
                tile.gfx = building.gfx;
                tile.posoffs = building.posoffs;
                tile.heights[2] = m_tileAtlas->gfxHeights.at(tile.gfx);
            }
        }
    }

    return true;
}

bool mdcii::Game::OnUserUpdate(float t_elapsedTime)
{
    // simple camera

    olc::vf2d vVel = { 0.0f, 0.0f };

    if (GetKey(olc::Key::W).bHeld)
    {
        vVel += { 0, -1 };
    }
    if (GetKey(olc::Key::S).bHeld)
    {
        vVel += { 0, +1 };
    }
    if (GetKey(olc::Key::A).bHeld)
    {
        vVel += { -1, 0 };
    }
    if (GetKey(olc::Key::D).bHeld)
    {
        vVel += { +1, 0 };
    }

    // exit

    if (GetKey(olc::Key::ESCAPE).bHeld)
    {
        return false;
    }

    // render

    Clear(olc::BLACK);

    auto toScreen = [&](int t_x, int t_y, float t_startX, float t_startY)
    {
        return olc::vi2d
            {
                ((int)t_startX * 64) + (t_x - t_y) * (64 / 2),
                ((int)t_startY * 32) + (t_x + t_y) * (32 / 2)
            };
    };

    auto getAtlasIndex = [](int t_gfx, int t_rows)
    {
        return (t_gfx / (t_rows * t_rows));
    };

    auto getOffset = [](int t_gfx, int t_rows)
    {
        auto picInPic{ t_gfx % (t_rows * t_rows) };

        return olc::vi2d(picInPic % t_rows, picInPic / t_rows);
    };

    for (auto const& island : m_islands)
    {
        island->x += vVel.x * t_elapsedTime * 8.0f;
        island->y += vVel.y * t_elapsedTime * 8.0f;

        for (auto y{ 0 }; y < island->height; ++y)
        {
            for (auto x{ 0 }; x < island->width; ++x)
            {
                const auto index{ y * island->width + x };
                const auto& tile{ island->tiles.at(index) };

                if (tile.HasBuilding() && tile.gfx != -1)
                {
                    auto atlas{ getAtlasIndex(tile.gfx, 16) };
                    auto offsetXy{ getOffset(tile.gfx, 16) };
                    auto sc{ toScreen(x, y, island->x, island->y) };
                    auto offset{ 0.0f };

                    if (tile.IsAboveWater() && tile.heights[2] > 31)
                    {
                        offset = (float)tile.heights[2] - 31.0f;
                    }

                    DrawPartialDecal(
                        olc::vf2d(
                            (float)sc.x,
                            (float)sc.y - offset
                        ),
                        m_tileAtlas->gfxDecAtlas.at(atlas).get(),
                        olc::vf2d(
                            (float)offsetXy.x * 64.0f,
                            (float)offsetXy.y * 286.0f
                        ),
                        olc::vf2d(64.0f, 286.0f)
                    );
                }
            }
        }
    }

    return true;
}
