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
#include "Game.h"
#include "MdciiAssert.h"
#include "MdciiUtils.h"
#include "ogl/OpenGL.h"
#include "ogl/Window.h"
#include "state/StateStack.h"
#include "data/Text.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::MainMenuState::MainMenuState(const state::StateId t_id, std::shared_ptr<state::Context> t_context)
    : State(t_id, std::move(t_context))
{
    Log::MDCII_LOG_DEBUG("[MainMenuState::MainMenuState()] Create MainMenuState.");

    MDCII_ASSERT(context, "[MainMenuState::MainMenuState()] Null pointer.")

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
}

void mdcii::MainMenuState::Render()
{
}

void mdcii::MainMenuState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    begin_centered("MainMenu");

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Editor").c_str()))
    {
        context->stateStack->PopState(GetStateId());
        context->stateStack->PushState(state::StateId::WORLD_GENERATOR);
    }

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "NewGame").c_str()))
    {
        context->stateStack->PopState(GetStateId());
        context->stateStack->PushState(state::StateId::NEW_GAME);
    }

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "LoadGame").c_str()))
    {
        context->stateStack->PopState(GetStateId());
        context->stateStack->PushState(state::StateId::LOADED_GAME);
    }

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Exit").c_str()))
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
    Log::MDCII_LOG_DEBUG("[MainMenuState::Init()] Initializing main menu state...");

    Log::MDCII_LOG_DEBUG("[MainMenuState::Init()] The main menu state was successfully initialized.");
}
