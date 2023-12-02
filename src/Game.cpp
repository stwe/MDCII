// This file is part of the MDCII project.
//
// Copyright (c) 2023. stwe <https://github.com/stwe/MDCII>
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
#include "Log.h"
#include "GameState.h"
#include "MainMenuState.h"
#include "Intl.h"
#include "resource/OriginalResourcesManager.h"
#include "resource/TileAtlas.h"
#include "resource/AssetManager.h"
#include "state/StateSystem.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::Game::Game()
{
    MDCII_LOG_DEBUG("[Game::Game()] Create Game.");

    setup_i18n(INI.Get<std::string>("locale", "lang"), RESOURCES_REL_PATH);

    sAppName = "MDCII";
}

mdcii::Game::~Game()
{
    MDCII_LOG_DEBUG("[Game::~Game()] Destruct Game.");
}

bool mdcii::Game::OnUserCreate()
{
    /*
    wave.LoadAudioWaveform(fmt::format("{}SampleC.wav", Game::RESOURCES_REL_PATH));
    engine.InitialiseAudio(44100, 2);
    */

    originalResourcesManager = std::make_unique<resource::OriginalResourcesManager>();
    tileAtlas = std::make_unique<resource::TileAtlas>();
    assetManager = std::make_unique<resource::AssetManager>();

    gameLayer = static_cast<int>(CreateLayer());
    EnableLayer(static_cast<uint8_t>(gameLayer), true);
    SetLayerCustomRenderFunction(0, std::bind_front(&Game::RenderImGui, this));

    stateSystem = std::make_unique<state::StateSystem>();
    stateSystem->AddState(state::StateId::MAIN_MENU, std::make_unique<MainMenuState>(this));
    stateSystem->AddState(state::StateId::NEW_GAME, std::make_unique<GameState>(this));
    stateSystem->ChangeState(state::StateId::NEW_GAME);

    return stateSystem->OnUserCreate();
}

bool mdcii::Game::OnUserUpdate(float t_elapsedTime)
{
    /*
    if (GetKey(olc::Key::K1).bPressed)
    {
        engine.PlayWaveform(&wave);
    }
    */

    return stateSystem->OnUserUpdate(t_elapsedTime);
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::Game::SetGameLayer()
{
    SetDrawTarget(static_cast<uint8_t>(gameLayer));
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::Game::RenderImGui()
{
    pgeImGui.ImGui_ImplPGE_Render();
}
