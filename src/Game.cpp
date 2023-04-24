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

#include "Game.h"
#include "MdciiException.h"
#include "MainMenuState.h"
#include "IslandGeneratorState.h"
#include "WorldGeneratorState.h"
#include "GameState.h"
#include "camera/Camera.h"
#include "state/StateStack.h"
#include "ogl/Window.h"
#include "file/OriginalResourcesManager.h"
#include "file/MdciiResourcesManager.h"
#include "data/Text.h"
#include "sound/SoundDevice.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::Game::Game()
{
    Log::MDCII_LOG_DEBUG("[Game::Game()] Create Game.");
}

mdcii::Game::~Game() noexcept
{
    Log::MDCII_LOG_DEBUG("[Game::~Game()] Destruct Game.");
}

//-------------------------------------------------
// Run
//-------------------------------------------------

void mdcii::Game::Run()
{
    Log::MDCII_LOG_DEBUG("[Game::Run()] Starts running game.");

    Init();
    GameLoop();
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::Game::Init()
{
    Log::MDCII_LOG_DEBUG("[Game::Init()] Initializing game...");

    CreateObjects();
    Start();

    Log::MDCII_LOG_DEBUG("[Game::Init()] The game was successfully initialized.");
}

void mdcii::Game::Input() const
{
    m_stateStack->Input();
}

void mdcii::Game::Update() const
{
    m_stateStack->Update();
}

void mdcii::Game::Render() const
{
    m_stateStack->Render();
}

//-------------------------------------------------
// Game loop
//-------------------------------------------------

void mdcii::Game::GameLoop() const
{
    Log::MDCII_LOG_DEBUG("[Game::GameLoop()] Starting the game loop.");

    auto previousTime{ glfwGetTime() };
    auto lag{ 0.0 };

    auto infoTimer{ previousTime };
    auto render{ 0 };
    auto updates{ 0 };

    while (!m_window->WindowShouldClose())
    {
        const auto currentTime{ glfwGetTime() };
        const auto elapsedTime{ currentTime - previousTime };
        previousTime = currentTime;

        lag += elapsedTime;

        Input();

        while (lag >= FRAME_TIME)
        {
            Update();
            updates++;
            lag -= FRAME_TIME;
        }

        if (m_stateStack->IsEmpty())
        {
            m_window->Close();
        }

        Render();
        render++;

        if (glfwGetTime() - infoTimer > 1.0)
        {
            infoTimer++;

            std::stringstream ss;
#ifdef MDCII_DEBUG_BUILD
            ss << m_window->GetTitle() << " [DEBUG BUILD Version: " << VERSION << "]"
               << "   |   Render: " << render << "   |   Updates: " << updates;
#else
            ss << m_window->GetTitle() << " [RELEASE BUILD Version: " << VERSION << "]"
               << "   |   Render: " << render << "   |   Updates: " << updates;
#endif
            glfwSetWindowTitle(m_window->GetWindowHandle(), ss.str().c_str());

            updates = 0;
            render = 0;
        }
    }

    Log::MDCII_LOG_DEBUG("[Game::GameLoop()] The game loop has ended.");
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::Game::CreateObjects()
{
    Log::MDCII_LOG_DEBUG("[Game::CreateSharedObjects()] Create objects.");

    m_soundDevice = std::make_shared<sound::SoundDevice>();
    is_sound_device_available = m_soundDevice->IsSoundDeviceAvailable();

    m_window = std::make_shared<ogl::Window>();
    m_camera = std::make_shared<camera::Camera>(m_window->width, m_window->height);
    m_originalResourcesManager = std::make_shared<file::OriginalResourcesManager>();
    m_mdciiResourcesManager = std::make_shared<file::MdciiResourcesManager>();
    m_stateStack = std::make_unique<state::StateStack>(std::make_unique<state::Context>(m_window, m_camera, m_originalResourcesManager, m_mdciiResourcesManager));
}

void mdcii::Game::Start() const
{
    Log::MDCII_LOG_DEBUG("[Game::Start()] Register all game states.");

    data::Text::Init();

    m_stateStack->RegisterState<MainMenuState>(state::StateId::MAIN_MENU);
    m_stateStack->RegisterState<IslandGeneratorState>(state::StateId::ISLAND_GENERATOR);
    m_stateStack->RegisterState<WorldGeneratorState>(state::StateId::WORLD_GENERATOR);
    m_stateStack->RegisterState<GameState>(state::StateId::NEW_GAME);
    m_stateStack->RegisterState<GameState>(state::StateId::LOADED_GAME);

    if (const auto startStateId{ magic_enum::enum_cast<state::StateId>(INI.Get<std::string>("game", "start_state")) }; startStateId.has_value())
    {
        switch (startStateId.value())
        {
        case state::StateId::MAIN_MENU:
            m_stateStack->PushState(state::StateId::MAIN_MENU);
            break;
        case state::StateId::ISLAND_GENERATOR:
            m_stateStack->PushState(state::StateId::ISLAND_GENERATOR);
            break;
        case state::StateId::WORLD_GENERATOR:
            m_stateStack->PushState(state::StateId::WORLD_GENERATOR);
            break;
        case state::StateId::NEW_GAME:
            m_stateStack->PushState(state::StateId::NEW_GAME);
            break;
        case state::StateId::LOADED_GAME:
            m_stateStack->PushState(state::StateId::LOADED_GAME);
            break;
        default:;
        }
    }
    else
    {
        throw MDCII_EXCEPTION("[Game::Start()] Invalid state name given.");
    }
}
