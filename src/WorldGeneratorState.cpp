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

#include "WorldGeneratorState.h"
#include "Log.h"
#include "Game.h"
#include "world/WorldGenerator.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::WorldGeneratorState::WorldGeneratorState(Game* t_game)
    : State(t_game)
{
    MDCII_LOG_DEBUG("[WorldGeneratorState::WorldGeneratorState()] Create WorldGeneratorState.");

    worldGenerator = std::make_unique<world::WorldGenerator>(this);
}

mdcii::WorldGeneratorState::~WorldGeneratorState() noexcept
{
    MDCII_LOG_DEBUG("[WorldGeneratorState::~WorldGeneratorState()] Destruct WorldGeneratorState.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

bool mdcii::WorldGeneratorState::OnUserCreate()
{
    MDCII_LOG_DEBUG("[WorldGeneratorState::OnUserCreate()] Init WorldGeneratorState.");

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
