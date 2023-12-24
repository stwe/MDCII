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

#include "StateSystem.h"
#include "State.h"
#include "Log.h"
#include "vendor/olc/olcPixelGameEngine.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::state::StateSystem::StateSystem()
{
    MDCII_LOG_DEBUG("[StateSystem::StateSystem()] Create StateSystem.");
}

mdcii::state::StateSystem::~StateSystem() noexcept
{
    MDCII_LOG_DEBUG("[StateSystem::~StateSystem()] Destruct StateSystem.");
}

//-------------------------------------------------
// Add && Change
//-------------------------------------------------

void mdcii::state::StateSystem::AddState(const StateId t_stateId, std::unique_ptr<State> t_state)
{
    states.emplace(t_stateId, std::move(t_state));
}

void mdcii::state::StateSystem::ChangeState(const StateId t_stateId)
{
    if (states.contains(t_stateId))
    {
        currentState = states.at(t_stateId).get();
    }
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

bool mdcii::state::StateSystem::OnUserCreate() const
{
    if (currentState)
    {
        return currentState->OnUserCreate();
    }

    return false;
}

bool mdcii::state::StateSystem::OnUserUpdate(const float t_elapsedTime) const
{
    if (currentState)
    {
        return currentState->OnUserUpdate(t_elapsedTime);
    }

    return false;
}
