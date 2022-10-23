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
#include "ogl/OpenGL.h"
#include "ogl/Window.h"
#include "state/StateStack.h"
#include "world/WorldGenerator.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::WorldGeneratorState::WorldGeneratorState(state::StateId t_id, std::shared_ptr<state::Context> t_context)
    : State(t_id, std::move(t_context))
{
    Log::MDCII_LOG_DEBUG("[WorldGeneratorState::WorldGeneratorState()] Create WorldGeneratorState.");

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
    // nothing to do at the moment
}

void mdcii::WorldGeneratorState::Render()
{
    // nothing to do at the moment
}

void mdcii::WorldGeneratorState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    ImGui::Begin("World Generator");

    static int width{ 8 };
    static int height{ 8 };
    ImGui::SliderInt("World width: ", &width, 8, 128);
    ImGui::SliderInt("World height:", &height, 8, 128);

    if (ImGui::Button("Generate a new world in data/NewWorld.json"))
    {
        world::WorldGenerator worldGenerator{ width, height, "data/NewWorld.json" };
    }

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
    Log::MDCII_LOG_DEBUG("[WorldGeneratorState::Init()] Initializing world generator state.");


    Log::MDCII_LOG_DEBUG("[WorldGeneratorState::Init()] The world generator state was successfully initialized.");

}
