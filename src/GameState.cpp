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

#include "GameState.h"
#include "MdciiAssert.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::GameState::GameState(state::StateId t_id, Game* t_game)
    : State(t_id, t_game)
{
    MDCII_LOG_DEBUG("[GameState::GameState()] Create GameState.");

    MDCII_ASSERT(m_game, "[GameState::GameState()] Null pointer.")
}

mdcii::GameState::~GameState() noexcept
{
    MDCII_LOG_DEBUG("[GameState::~GameState()] Destruct GameState.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::GameState::OnUserCreate()
{

}

void mdcii::GameState::OnUserUpdate(const float t_elapsedTime)
{

}

void mdcii::GameState::RenderImGui()
{

}
