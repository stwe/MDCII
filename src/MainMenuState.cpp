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
#include "MainMenuState.h"
#include "ogl/OpenGL.h"
#include "ogl/Window.h"
#include "state/StateStack.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::MainMenuState::MainMenuState(state::StateId t_id, std::shared_ptr<state::Context> t_context)
    : State(t_id, std::move(t_context))
{
    Log::MDCII_LOG_DEBUG("[MainMenuState::MainMenuState()] Create MainMenuState.");

    Init();
}

mdcii::MainMenuState::~MainMenuState() noexcept
{
    Log::MDCII_LOG_DEBUG("[MainMenuState::~MainMenuState()] Destruct MainMenuState.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::MainMenuState::Input()
{
    // ESC for quit
    if (context->window->IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        Log::MDCII_LOG_DEBUG("[MainMenuState::Input()] Starts POP MainMenuState.");
        context->stateStack->PopState(GetStateId());
    }
}

void mdcii::MainMenuState::Update()
{
    // nothing to do at the moment
}

void mdcii::MainMenuState::Render()
{
    // nothing to do at the moment
}

void mdcii::MainMenuState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    ImGui::Begin("Main Menu");

    if (ImGui::Button("Create a new world"))
    {
        context->stateStack->PopState(GetStateId());
        context->stateStack->PushState(state::StateId::WORLD_GENERATOR);
    }

    if (ImGui::Button("Start the example"))
    {
        context->stateStack->PopState(GetStateId());
        context->stateStack->PushState(state::StateId::EXAMPLE_GAME);
    }

    if (ImGui::Button("Start a new game"))
    {
        context->stateStack->PopState(GetStateId());
        context->stateStack->PushState(state::StateId::NEW_GAME);
    }

    if (ImGui::Button("Load an existing game"))
    {
        context->stateStack->PopState(GetStateId());
        context->stateStack->PushState(state::StateId::LOADED_GAME);
    }

    if (ImGui::Button("Exit"))
    {
        context->stateStack->PopState(GetStateId());
    }

    ImGui::End();

    ogl::Window::ImGuiEnd();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::MainMenuState::Init()
{
    Log::MDCII_LOG_DEBUG("[MainMenuState::Init()] Initializing main menu state.");


    Log::MDCII_LOG_DEBUG("[MainMenuState::Init()] The main menu state was successfully initialized.");
}
