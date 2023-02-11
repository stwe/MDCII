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
#include "Game.h"
#include "MdciiAssert.h"
#include "MdciiUtils.h"
#include "ogl/OpenGL.h"
#include "ogl/Window.h"
#include "state/StateStack.h"
#include "world/WorldGenerator2.h"
#include "data/Text.h"

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

    begin_right("Editor", 322.0f);

    ImGui::SetWindowSize({ 321.0f, 600.0f });

    m_worldGenerator2->RenderImGui();

    auto bt{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "BackTo") };
    if (ImGui::Button(bt.append(" ").append(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "MainMenu")).c_str()))
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

    m_worldGenerator2 = std::make_unique<world::WorldGenerator2>();

    Log::MDCII_LOG_DEBUG("[WorldGeneratorState::Init()] The world generator state was successfully initialized.");
}
