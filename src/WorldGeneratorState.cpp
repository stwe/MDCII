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
#include "data/Text.h"
#include "world/GeneratorWorld.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::WorldGeneratorState::WorldGeneratorState(const state::StateId t_id, std::shared_ptr<state::Context> t_context)
    : State(t_id, std::move(t_context))
{
    Log::MDCII_LOG_DEBUG("[WorldGeneratorState::WorldGeneratorState()] Create WorldGeneratorState.");

    MDCII_ASSERT(context, "[WorldGeneratorState::WorldGeneratorState()] Null pointer.")
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
        context->stateStack->PopState(id);
    }
}

void mdcii::WorldGeneratorState::Update()
{
    if (m_generatorWorld)
    {
        m_generatorWorld->Update();
    }
}

void mdcii::WorldGeneratorState::Render()
{
    if (m_generatorWorld)
    {
        m_generatorWorld->Render();
    }
}

void mdcii::WorldGeneratorState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    static int32_t w{ world::World::WORLD_MIN_WIDTH };
    static int32_t h{ world::World::WORLD_MIN_HEIGHT };

    if (!m_generatorWorld)
    {
        begin_centered("WorldGeneratorState");

        ImGui::SliderInt(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "WorldWidth").c_str(), &w, world::World::WORLD_MIN_WIDTH, world::World::WORLD_MAX_WIDTH);
        ImGui::SliderInt(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "WorldHeight").c_str(), &h, world::World::WORLD_MIN_HEIGHT, world::World::WORLD_MAX_HEIGHT);

        if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "GenerateWorld").c_str()))
        {
            m_generatorWorld = std::make_shared<world::GeneratorWorld>(context, id, w, h);
        }

        auto bt{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "BackTo") };
        if (ImGui::Button(bt.append(" ").append(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "MainMenu")).c_str()))
        {
            context->stateStack->PopState(id);
            context->stateStack->PushState(state::StateId::MAIN_MENU);
        }

        ImGui::End();
    }
    else
    {
        m_generatorWorld->RenderImGui();
    }

    ogl::Window::ImGuiEnd();
}
