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

#include "StateStack.h"
#include "State.h"
#include "Game.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::state::StateStack::StateStack(Game* t_game)
    : m_game{ t_game }
{
    MDCII_LOG_DEBUG("[StateStack::StateStack()] Create StateStack.");

    MDCII_ASSERT(m_game, "[StateStack::StateStack()] Null pointer.")
}

mdcii::state::StateStack::~StateStack() noexcept
{
    MDCII_LOG_DEBUG("[StateStack::~StateStack()] Destruct StateStack.");
}

//-------------------------------------------------
// Stack operations
//-------------------------------------------------

void mdcii::state::StateStack::PushState(const StateId t_id)
{
    MDCII_LOG_DEBUG("[StateStack::PushState()] Add pending stack operation PUSH for state {}.", magic_enum::enum_name(t_id));
    m_pendingChanges.emplace_back(Action::PUSH, t_id);
}

void mdcii::state::StateStack::PopState(const StateId t_id)
{
    MDCII_LOG_DEBUG("[StateStack::PushState()] Add pending stack operation POP for state {}.", magic_enum::enum_name(t_id));
    m_pendingChanges.emplace_back(Action::POP, t_id);
}

void mdcii::state::StateStack::ClearStates()
{
    MDCII_LOG_DEBUG("[StateStack::ClearStates()] Add pending stack operation CLEAR all states.");
    m_pendingChanges.emplace_back(Action::CLEAR, StateId::ALL);
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::state::StateStack::OnUserCreate()
{
    // iterate from top to bottom
    for (auto itr{ m_stack.rbegin() }; itr != m_stack.rend(); ++itr)
    {
        (*itr)->OnUserCreate();
    }

    ApplyPendingChanges();
}

void mdcii::state::StateStack::OnUserUpdate(const float t_elapsedTime)
{
    // iterate from top to bottom
    for (auto itr{ m_stack.rbegin() }; itr != m_stack.rend(); ++itr)
    {
        (*itr)->OnUserUpdate(t_elapsedTime);
    }

    ApplyPendingChanges();
}

void mdcii::state::StateStack::RenderImGui()
{
    /*
    for (const auto& state : m_stack)
    {
        state->StartFrame();

        state->Render();
        state->RenderImGui();

        state->EndFrame();
    }
    */
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

std::unique_ptr<mdcii::state::State> mdcii::state::StateStack::CreateState(const StateId t_id)
{
    MDCII_LOG_DEBUG("[StateStack::CreateState()] Running factory function for state {}.", magic_enum::enum_name(t_id));

    const auto it{ m_factories.find(t_id) };
    MDCII_ASSERT(it != m_factories.end(), "[StateStack::CreateState()] Factory function not found for state " + std::string(magic_enum::enum_name(t_id)) + ".")

    return it->second();
}

void mdcii::state::StateStack::ApplyPendingChanges()
{
    for (const auto& change : m_pendingChanges)
    {
        switch (change.action)
        {
        case Action::PUSH:
            // add element at the end
            m_stack.emplace_back(CreateState(change.id));
            MDCII_LOG_DEBUG("[StateStack::ApplyPendingChanges()] Stack size is {} after PUSH state {}.", m_stack.size(), magic_enum::enum_name(change.id));
            break;
        case Action::POP:
            // removes the last element in the vector
            MDCII_ASSERT(change.id == m_stack.back()->id, "[StateStack::ApplyPendingChanges()] Invalid POP operation.")
            m_stack.pop_back();
            MDCII_LOG_DEBUG("[StateStack::ApplyPendingChanges()] Stack size is {} after POP state {}.", m_stack.size(), magic_enum::enum_name(change.id));
            break;
        case Action::CLEAR:
            m_stack.clear();
            MDCII_LOG_DEBUG("[StateStack::ApplyPendingChanges()] Stack size is {} after CLEAR all states.", m_stack.size());
            break;
        }
    }

    m_pendingChanges.clear();
}
