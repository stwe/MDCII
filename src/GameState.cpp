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
#include "Log.h"
#include "ogl/OpenGL.h"
#include "ogl/Window.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::GameState::GameState(const state::StateId t_id, state::StateStack* t_stateStack, std::shared_ptr<state::Context> t_context)
    : State(t_id, t_stateStack, std::move(t_context))
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
        RequestStackPop();
    }
}

void mdcii::GameState::Update()
{
}

void mdcii::GameState::Render()
{
}

void mdcii::GameState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    ImGui::Begin("GameState");


    ImGui::End();

    ogl::Window::ImGuiEnd();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::GameState::Init()
{
    Log::MDCII_LOG_DEBUG("[GameState::Init()] Initializing game state.");

    Log::MDCII_LOG_DEBUG("[GameState::Init()] The game state was successfully initialized.");
}
