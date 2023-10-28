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
#include "GameState.h"
#include "resource/OriginalResourcesManager.h"
#include "resource/TileAtlas.h"
#include "state/StateSystem.h"

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
    origResMng = std::make_unique<resource::OriginalResourcesManager>();
    tileAtlas = std::make_unique<resource::TileAtlas>();

    m_stateSystem = std::make_unique<state::StateSystem>();
    m_stateSystem->AddState(state::StateId::NEW_GAME, new GameState(this));
    m_stateSystem->ChangeState(state::StateId::NEW_GAME);

    return m_stateSystem->OnUserCreate();
}

bool mdcii::Game::OnUserUpdate(float t_elapsedTime)
{
    return m_stateSystem->OnUserUpdate(t_elapsedTime);
}
