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

#include "World.h"
#include "Island.h"
#include "MdciiAssert.h"
#include "GameState.h"
#include "resource/MdciiFile.h"
#include "renderer/Renderer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::World::World(GameState* t_gameState)
    : gameState{ t_gameState }
{
    MDCII_LOG_DEBUG("[World::World()] Create World.");

    MDCII_ASSERT(gameState, "[World::World()] Null pointer.")

    Init();
}

mdcii::world::World::~World() noexcept
{
    MDCII_LOG_DEBUG("[World::~World()] Destruct World.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::World::OnUserUpdate(const float t_elapsedTime, const olc::vf2d& t_vel)
{
    for (auto const& island : islands)
    {
        island->startX += t_vel.x * t_elapsedTime * 32.0f;
        island->startY += t_vel.y * t_elapsedTime * 32.0f;
    }

    startX += t_vel.x * t_elapsedTime * 32.0f;
    startY += t_vel.y * t_elapsedTime * 32.0f;

    renderer->RenderDeepWater();
    renderer->RenderIslands();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::World::Init()
{
    MDCII_LOG_DEBUG("[World::Init()] Init World.");

    if (resource::MdciiFile file{ "resources/sav/Example.sav" }; file.LoadJsonFromFile())
    {
        islands = file.CreateIslands();
    }

    for (auto const& island : islands)
    {
        island->InitTiles(gameState->game);
    }

    // todo: set world width and height from the savegame

    renderer = std::make_unique<renderer::Renderer>(this);
}
