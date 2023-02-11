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
#include "MdciiAssert.h"
#include "MdciiUtils.h"
#include "ogl/OpenGL.h"
#include "ogl/Window.h"
#include "world/World.h"
#include "state/StateStack.h"
#include "data/Text.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::GameState::GameState(const state::StateId t_id, std::shared_ptr<state::Context> t_context)
    : State(t_id, std::move(t_context))
{
    Log::MDCII_LOG_DEBUG("[GameState::GameState()] Create GameState.");

    MDCII_ASSERT(context, "[GameState::GameState()] Null pointer.")

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
        context->stateStack->PopState(GetStateId());
    }
}

void mdcii::GameState::Update()
{
    if (m_world)
    {
        m_world->Update();
    }
}

void mdcii::GameState::Render()
{
    if (m_world)
    {
        m_world->Render();
    }
}

void mdcii::GameState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    if (!m_world)
    {
        begin_centered("Choose a file");

        switch (GetStateId())
        {
        case state::StateId::NEW_GAME:
            RenderFileChooser(m_mapFiles);
            break;
        case state::StateId::LOADED_GAME:
            RenderFileChooser(m_savedGameFiles);
            break;
        default:;
        }

        auto bt{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "BackTo") };
        if (ImGui::Button(bt.append(" ").append(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "MainMenu")).c_str()))
        {
            context->stateStack->PopState(GetStateId());
            context->stateStack->PushState(state::StateId::MAIN_MENU);
        }

        ImGui::End();
    }
    else
    {
        m_world->RenderImGui();
    }

    ogl::Window::ImGuiEnd();
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::GameState::RenderFileChooser(std::vector<std::string>& t_files)
{
    static int32_t fileIndex{ 0 };

    if (t_files.empty())
    {
        ImGui::TextUnformatted(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "MissingFiles").c_str());
    }
    else
    {
        ImGui::ListBox(
            data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "ChooseFile").c_str(),
            &fileIndex,
            vector_getter,
            &t_files,
            static_cast<int32_t>(t_files.size())
        );
        if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "LoadFile").c_str()))
        {
            m_world = std::make_shared<world::World>(t_files.at(fileIndex), context, GetStateId());
        }
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::GameState::Init()
{
    Log::MDCII_LOG_DEBUG("[GameState::Init()] Initializing game state...");

    m_mapFiles = get_files_list("map/", MAP_FILE_EXTENSION);
    m_savedGameFiles = get_files_list("save/", SAVE_GAME_FILE_EXTENSION);

    Log::MDCII_LOG_DEBUG("[GameState::Init()] The game state was successfully initialized.");
}
