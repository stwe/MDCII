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

#include "IslandGeneratorState.h"
#include "MdciiAssert.h"
#include "MdciiUtils.h"
#include "Game.h"
#include "ogl/OpenGL.h"
#include "ogl/Window.h"
#include "state/StateStack.h"
#include "world/IslandGenerator.h"
#include "data/Text.h"
#include "vendor/imgui/imgui.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::IslandGeneratorState::IslandGeneratorState(mdcii::state::StateId t_id, std::shared_ptr<state::Context> t_context)
    : State(t_id, std::move(t_context))
{
    Log::MDCII_LOG_DEBUG("[IslandGeneratorState::IslandGeneratorState()] Create IslandGeneratorState.");

    MDCII_ASSERT(context, "[IslandGeneratorState::IslandGeneratorState()] Null pointer.")

    Init();
}

mdcii::IslandGeneratorState::~IslandGeneratorState() noexcept
{
    Log::MDCII_LOG_DEBUG("[IslandGeneratorState::~IslandGeneratorState()] Destruct IslandGeneratorState.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::IslandGeneratorState::Input()
{
    // ESC for quit
    if (context->window->IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        Log::MDCII_LOG_DEBUG("[IslandGeneratorState::Input()] Starts POP IslandGeneratorState.");
        context->stateStack->PopState(id);
    }
}

void mdcii::IslandGeneratorState::Update()
{
}

void mdcii::IslandGeneratorState::Render()
{
}

void mdcii::IslandGeneratorState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    begin_right("IslandGeneratorState", 342.0f);

    ImGui::SetWindowSize({ 341.0f, 640.0f });

    m_islandGenerator->RenderImGui();

    auto bt{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "BackTo") };
    if (ImGui::Button(bt.append(" ").append(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "MainMenu")).c_str()))
    {
        context->stateStack->PopState(id);
        context->stateStack->PushState(state::StateId::MAIN_MENU);
    }

    ImGui::End();

    ogl::Window::ImGuiEnd();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::IslandGeneratorState::Init()
{
    Log::MDCII_LOG_DEBUG("[IslandGeneratorState::Init()] Initializing island generator state...");

    m_islandGenerator = std::make_unique<world::IslandGenerator>();

    Log::MDCII_LOG_DEBUG("[IslandGeneratorState::Init()] The island generator state was successfully initialized.");
}
