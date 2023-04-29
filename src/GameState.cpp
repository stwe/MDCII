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
#include "Game.h"
#include "ogl/Window.h"
#include "world/GameWorld.h"
#include "state/StateStack.h"
#include "data/Text.h"
#include "sound/MusicBuffer.h"
#include "sound/SoundBuffer.h"
#include "sound/SoundSource.h"
#include "file/MdciiResourcesManager.h"
#include "file/OriginalResourcesManager.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::GameState::GameState(const state::StateId t_id, std::shared_ptr<state::Context> t_context)
    : State(t_id, std::move(t_context))
{
    Log::MDCII_LOG_DEBUG("[GameState::GameState()] Create GameState.");

    MDCII_ASSERT(context, "[GameState::GameState()] Null pointer.")

    if (Game::is_sound_device_available && Game::game_type == Game::GameType::HISTORY)
    {
        m_bgMusicBuffer = std::make_unique<sound::MusicBuffer>(context->originalResourcesManager->mp3Files.at("03 - 1st Beginning.mp3"));
        if (m_bgMusicBuffer)
        {
            m_bgMusicBuffer->Play();
        }

        // todo temp code - test sound
        m_soundBuffer = std::make_unique<sound::SoundBuffer>();
        m_id = m_soundBuffer->AddSoundEffect(*context->originalResourcesManager->wavFiles.at("Burg.wav"));
        m_soundSource = std::make_unique<sound::SoundSource>(m_id);
    }
    else
    {
        Log::MDCII_LOG_WARN("[GameState::GameState()] No sound available.");
    }
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
        context->stateStack->PopState(id);
    }
}

void mdcii::GameState::Update()
{
    if (m_bgMusicBuffer)
    {
        m_bgMusicBuffer->IsPlaying() ? m_bgMusicBuffer->UpdateBufferStream() : m_bgMusicBuffer->Play();
    }

    // todo temp code
    if (context->window->IsKeyPressed(GLFW_KEY_S))
    {
        m_soundSource->Play();
    }

    if (m_gameWorld)
    {
        m_gameWorld->Update();
    }
}

void mdcii::GameState::Render()
{
    if (m_gameWorld)
    {
        m_gameWorld->Render();
    }
}

void mdcii::GameState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    if (!m_gameWorld)
    {
        begin_centered("GameState");

        switch (id)
        {
        case state::StateId::NEW_GAME:
            RenderFileChooser(context->mdciiResourcesManager->mapFiles);
            break;
        case state::StateId::LOADED_GAME:
            RenderFileChooser(context->mdciiResourcesManager->saveGameFiles);
            break;
        default:;
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
        m_gameWorld->RenderImGui();
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
        file_chooser(t_files, &fileIndex);
        if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "LoadFile").c_str()))
        {
            m_gameWorld = std::make_shared<world::GameWorld>(context, id, t_files.at(fileIndex));
        }
    }
}
