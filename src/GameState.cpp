// This file is part of the MDCII project.
//
// Copyright (c) 2022. stwe <https://github.com/stwe/MDCII>
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

#include <imgui.h>
#include "GameState.h"
#include "ogl/OpenGL.h"
#include "ogl/Window.h"
#include "world/World.h"
#include "state/StateStack.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::GameState::GameState(const state::StateId t_id, std::shared_ptr<state::Context> t_context)
    : State(t_id, std::move(t_context))
{
    Log::MDCII_LOG_DEBUG("[GameState::GameState()] Create GameState.");

    Init();
}

mdcii::GameState::~GameState() noexcept
{
    Log::MDCII_LOG_DEBUG("[GameState::~GameState()] Destruct GameState.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::GameState::Input()
{
    // ESC for quit
    if (context->window->IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        Log::MDCII_LOG_DEBUG("[GameState::Input()] Starts POP GameState.");
        context->stateStack->PopState(GetStateId());
    }
}

void mdcii::GameState::Update()
{
    // nothing to do at the moment
}

void mdcii::GameState::Render()
{
    m_world->Render();
}

void mdcii::GameState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    m_world->RenderImGui();

    ogl::Window::ImGuiEnd();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::GameState::Init()
{
    Log::MDCII_LOG_DEBUG("[GameState::Init()] Initializing game state.");

    switch (GetStateId())
    {
    case state::StateId::NEW_GAME:
        m_world = std::make_shared<world::World>(NEW_GAME_MAP, context, state::StateId::NEW_GAME);
        break;
    case state::StateId::LOADED_GAME:
        m_world = std::make_shared<world::World>(SAVE_GAME_MAP, context, state::StateId::LOADED_GAME);
        break;
    case state::StateId::EXAMPLE_GAME:
        m_world = std::make_shared<world::World>(EXAMPLE_GAME_MAP, context, state::StateId::EXAMPLE_GAME);
        break;
    default:
        m_world = std::make_shared<world::World>(NEW_GAME_MAP, context, state::StateId::NEW_GAME);
    }

    Log::MDCII_LOG_DEBUG("[GameState::Init()] The game state was successfully initialized.");
}
