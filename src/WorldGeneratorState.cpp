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

#include "WorldGeneratorState.h"
#include "MdciiAssert.h"
#include "Game.h"
#include "world/World.h"
#include "world/generator/WorldGenerator.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::WorldGeneratorState::WorldGeneratorState(Game* t_game)
    : State(t_game)
{
    MDCII_LOG_DEBUG("[WorldGeneratorState::WorldGeneratorState()] Create WorldGeneratorState.");
}

mdcii::WorldGeneratorState::~WorldGeneratorState() noexcept
{
    MDCII_LOG_DEBUG("[WorldGeneratorState::~WorldGeneratorState()] Destruct WorldGeneratorState.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

bool mdcii::WorldGeneratorState::OnUserCreate(void* t_data)
{
    MDCII_LOG_DEBUG("[WorldGeneratorState::OnUserCreate()] Init WorldGeneratorState.");

    auto* vi2dData{ reinterpret_cast<olc::vi2d*>(t_data) };
    MDCII_ASSERT(vi2dData, "[WorldGeneratorState::OnUserCreate()] Null pointer.")

    worldGenerator = std::make_unique<world::generator::WorldGenerator>(this);
    worldGenerator->world = std::make_unique<world::World>(this, vi2dData->x, vi2dData->y);

    return true;
}

bool mdcii::WorldGeneratorState::OnUserUpdate(const float t_elapsedTime)
{
    // exit
    if (game->GetKey(olc::Key::ESCAPE).bHeld)
    {
        return false;
    }

    // world
    game->SetGameLayer();
    game->Clear(olc::BLACK);
    worldGenerator->OnUserUpdate(t_elapsedTime);

    return true;
}
