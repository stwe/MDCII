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
#include "MdciiAssert.h"
#include "ogl/OpenGL.h"
#include "ogl/Window.h"
#include "state/StateStack.h"

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

    auto winW{ static_cast<float>(context->window->width) };
    auto winH{ static_cast<float>(context->window->height) };

    ImGui::SetNextWindowSize(ImVec2(174.0f, 104.0f), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Pos.x + (winW / 2.5f), ImGui::GetMainViewport()->Pos.y + (winH / 4.0f)), ImGuiCond_Once);

    const int32_t windowFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;
    //ImGuiWindowFlags_NoBackground;

    ImGui::SetNextWindowBgAlpha(0.8f);

    ImGui::Begin("Main Menu", nullptr, windowFlags);

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 230, 230, 255));

    if (ImGui::Button("Create a new world"))
    {
        context->stateStack->PopState(GetStateId());
        context->stateStack->PushState(state::StateId::WORLD_GENERATOR);
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

    ImGui::PopStyleColor();

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
