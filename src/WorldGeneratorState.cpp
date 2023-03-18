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
#include "MdciiException.h"
#include "MdciiAssert.h"
#include "MdciiUtils.h"
#include "ogl/OpenGL.h"
#include "ogl/Window.h"
#include "state/StateStack.h"
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

    begin_right("WorldGeneratorState", 322.0f);

    ImGui::SetWindowSize({ 321.0f, 600.0f });

    auto bt{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "BackTo") };
    if (ImGui::Button(bt.append(" ").append(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "MainMenu")).c_str()))
    {
        context->stateStack->PopState(GetStateId());
        context->stateStack->PushState(state::StateId::MAIN_MENU);
    }

    RenderIslandsImGui();

    ImGui::End();

    RenderWorld();

    ogl::Window::ImGuiEnd();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::WorldGeneratorState::Init()
{
    Log::MDCII_LOG_DEBUG("[WorldGeneratorState::Init()] Initializing world generator state...");

    m_islandFiles = get_files_list("island/", ".isl");

    nlohmann::json j = read_json_from_file(m_islandFiles[0]);

    if (!j.contains("width") || !j.contains("height") || !j.contains("layers"))
    {
        throw MDCII_EXCEPTION("[WorldGeneratorState::Init()] Invalid file format.");
    }

    for (const auto& [k, v] : j.items())
    {
        if (k == "width")
        {
            m_islandWidth = v.get<int32_t>();
        }

        if (k == "height")
        {
            m_islandHeight = v.get<int32_t>();
        }
    }

    Log::MDCII_LOG_DEBUG("[WorldGeneratorState::Init()] The world generator state was successfully initialized.");
}

void mdcii::WorldGeneratorState::RenderIslandsImGui()
{
    ImGui::SliderInt("Island start x", &m_map_x, 0, m_world_width - m_islandWidth - 1);
    ImGui::SliderInt("Island start y", &m_map_y, 0, m_world_height - m_islandHeight - 1);

    if (ImGui::Button("Save World && Island data"))
    {
        Log::MDCII_LOG_DEBUG("[WorldGeneratorState::RenderIslandsImGui()] Save data.");

        // todo: eine Map Datei erstellen mit den Daten der Insel
    }
}

void mdcii::WorldGeneratorState::RenderWorld()
{
    ImGui::SetNextWindowSize(ImVec2(560, 560), ImGuiCond_FirstUseEver);
    ImGui::Begin("World");

    ImGui::SliderInt("World width ", &m_world_width, 64, 256);
    ImGui::SliderInt("World height", &m_world_height, 64, 256);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const ImVec2 p = ImGui::GetCursorScreenPos();

    static ImVec4 blue = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
    static ImVec4 green = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    const ImU32 colBlue = ImColor(blue);
    const ImU32 colGreen = ImColor(green);

    for (auto y{ 0 }; y < m_world_height; ++y)
    {
        for (auto x{ 0 }; x < m_world_width; ++x)
        {
            auto xp{ p.x + x + ((x + 1) * 8) };
            auto yp{ p.y + y + ((y + 1) * 8) };

            auto xpIsland{ p.x + m_map_x + ((m_map_x + 1) * 8) };
            auto ypIsland{ p.y + m_map_y + ((m_map_y + 1) * 8) };

            auto xpIslandEnd{ p.x + (m_map_x + m_islandWidth) + ((m_map_x + m_islandWidth + 1) * 8) };
            auto ypIslandEnd{ p.y + (m_map_y + m_islandHeight) + ((m_map_y + m_islandHeight + 1) * 8) };

            draw_list->AddRectFilled(ImVec2(xp, yp), ImVec2(xp + 8, yp + 8), colBlue, 0.4f);
            draw_list->AddRectFilled(ImVec2(xpIsland, ypIsland), ImVec2(xpIslandEnd + 8, ypIslandEnd + 8), colGreen, 0.4f);
        }
    }

    ImGui::End();
}
