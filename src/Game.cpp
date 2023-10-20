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
#include "resource/MdciiFile.h"
#include "resource/OriginalResourcesManager.h"
#include "resource/TileAtlas.h"
#include "world/Island.h"
#include "renderer/Renderer.h"

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
    m_renderer = std::make_unique<renderer::Renderer>();

    tileAtlas = std::make_unique<resource::TileAtlas>();

    if (resource::MdciiFile file{ "resources/sav/Example.sav" }; file.LoadJsonFromFile())
    {
        islands = file.CreateIslands();
    }

    for (auto const& island : islands)
    {
        for (auto& tile : island->tiles)
        {
            if (tile.HasBuilding())
            {
                const auto& building{ m_origResMng->GetBuildingById(tile.buildingId) };
                tile.gfx = building.gfx;
                tile.posoffs = building.posoffs;
                for (const auto value : magic_enum::enum_values<world::Zoom>())
                {
                    const auto zoomInt{ magic_enum::enum_integer(value) };
                    tile.heights[zoomInt] = tileAtlas->heights.at(zoomInt).at(tile.gfx);
                    tile.SetOffset(value);
                }
            }
        }
    }

    return true;
}

bool mdcii::Game::OnUserUpdate(float t_elapsedTime)
{
    // zoom
    if (GetMouseWheel() > 0)
    {
        --zoom;
        MDCII_LOG_DEBUG("Zoom-- {}", magic_enum::enum_name(zoom));
    }
    if (GetMouseWheel() < 0)
    {
        ++zoom;
        MDCII_LOG_DEBUG("Zoom++ {}", magic_enum::enum_name(zoom));
    }

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

    // camera update
    for (auto const& island : islands)
    {
        island->x += vVel.x * t_elapsedTime * 32.0f;
        island->y += vVel.y * t_elapsedTime * 32.0f;
    }

    // render islands
    Clear(olc::BLACK);
    m_renderer->Render(this);

    return true;
}
