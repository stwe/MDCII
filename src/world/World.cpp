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
#include "camera/Camera.h"

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
    camera->OnUserUpdate(t_elapsedTime, t_vel);

    renderer->RenderDeepWater();
    renderer->RenderIslands();
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

olc::vi2d mdcii::world::World::ToScreen(const int t_x, const int t_y) const
{
    const auto position{ rotate_position(t_x, t_y, worldWidth, worldHeight, gameState->rotation) };

    return olc::vi2d{
        (static_cast<int>(gameState->world->camera->origin.x) * get_tile_width(gameState->zoom))
        + (position.x - position.y) * get_tile_width_half(gameState->zoom),
        (static_cast<int>(gameState->world->camera->origin.y) * get_tile_height(gameState->zoom))
        + (position.x + position.y) * get_tile_height_half(gameState->zoom)
    };
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
    camera = std::make_unique<camera::Camera>();
}
