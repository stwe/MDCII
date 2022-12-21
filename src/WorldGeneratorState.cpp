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
#include "WorldGeneratorState.h"
#include "MdciiAssert.h"
#include "ogl/OpenGL.h"
#include "ogl/Window.h"
#include "state/StateStack.h"
#include "world/WorldGenerator.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::WorldGeneratorState::WorldGeneratorState(const state::StateId t_id, std::shared_ptr<state::Context> t_context)
    : State(t_id, std::move(t_context))
{
    Log::MDCII_LOG_DEBUG("[WorldGeneratorState::WorldGeneratorState()] Create WorldGeneratorState.");

    MDCII_ASSERT(context, "[WorldGeneratorState::WorldGeneratorState()] Null pointer.")

    Init();
}

mdcii::WorldGeneratorState::~WorldGeneratorState() noexcept
{
    Log::MDCII_LOG_DEBUG("[WorldGeneratorState::~WorldGeneratorState()] Destruct WorldGeneratorState.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::WorldGeneratorState::Input()
{
    // ESC for quit
    if (context->window->IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        Log::MDCII_LOG_DEBUG("[WorldGeneratorState::Input()] Starts POP WorldGeneratorState.");
        context->stateStack->PopState(GetStateId());
    }
}

void mdcii::WorldGeneratorState::Update()
{
}

void mdcii::WorldGeneratorState::Render()
{
}

void mdcii::WorldGeneratorState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    auto winW{ static_cast<float>(context->window->GetWidth()) };
    auto winH{ static_cast<float>(context->window->GetHeight()) };

    ImGui::SetNextWindowSize(ImVec2(550.0f, 300.0f), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Pos.x + (winW / 4.0f), ImGui::GetMainViewport()->Pos.y + (winH / 4.0f)), ImGuiCond_Once);

    int windowFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;
    //ImGuiWindowFlags_NoBackground;

    ImGui::SetNextWindowBgAlpha(0.8f);

    ImGui::Begin("World Generator", nullptr, windowFlags);

    m_worldGenerator->RenderImGui();

    if (ImGui::Button("Back to main menu"))
    {
        context->stateStack->PopState(GetStateId());
        context->stateStack->PushState(state::StateId::MAIN_MENU);
    }

    ImGui::End();

    ogl::Window::ImGuiEnd();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::WorldGeneratorState::Init()
{
    Log::MDCII_LOG_DEBUG("[WorldGeneratorState::Init()] Initializing world generator state...");

    m_worldGenerator = std::make_unique<world::WorldGenerator>();

    Log::MDCII_LOG_DEBUG("[WorldGeneratorState::Init()] The world generator state was successfully initialized.");
}
