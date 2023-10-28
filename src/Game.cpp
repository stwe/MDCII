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
#include "resource/MdciiFile.h"
#include "resource/OriginalResourcesManager.h"
#include "resource/TileAtlas.h"
#include "world/Island.h"
#include "renderer/Renderer.h"
#include "state/StateSystem.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::Game::Game()
{
    MDCII_LOG_DEBUG("[Game::Game()] Create Game.");

    sAppName = "MDCII";
}

mdcii::Game::~Game()
{
    MDCII_LOG_DEBUG("[Game::~Game()] Destruct Game.");
}

bool mdcii::Game::OnUserCreate()
{
    /*
    origResMng = std::make_unique<resource::OriginalResourcesManager>();
    tileAtlas = std::make_unique<resource::TileAtlas>();

    if (resource::MdciiFile file{ "resources/sav/Example.sav" }; file.LoadJsonFromFile())
    {
        islands = file.CreateIslands();
    }

    for (auto const& island : islands)
    {
        island->InitTiles(this);
    }

    // Create and enable a new Layer which will be used for the game.
    gameLayer = CreateLayer();
    EnableLayer(static_cast<uint8_t>(gameLayer), true);

    // Set a custom render function on layer 0.  Since DrawUI is a member of
    // our class, we need to use std::bind
    // If the pge_imgui was constructed with _register_handler = true, this line is not needed
    SetLayerCustomRenderFunction(0, std::bind(&Game::DrawImGui, this));

    m_renderer = std::make_unique<renderer::Renderer>();
    */

    m_stateSystem = std::make_unique<state::StateSystem>();
    m_stateSystem->AddState(state::StateId::NEW_GAME, new GameState());
    m_stateSystem->InitState(state::StateId::NEW_GAME);

    return true;
}

bool mdcii::Game::OnUserUpdate(float t_elapsedTime)
{
    /*
    // zoom
    if (GetMouseWheel() > 0)
    {
        --zoom;
        MDCII_LOG_DEBUG("Zoom-- {}", magic_enum::enum_name(zoom));
    }
    if (GetMouseWheel() < 0)
    {
        ++zoom;
        MDCII_LOG_DEBUG("Zoom++ {}", magic_enum::enum_name(zoom));
    }

    // rotation
    if (GetKey(olc::Key::PGUP).bPressed)
    {
        ++rotation;
        MDCII_LOG_DEBUG("World rotation++ {}", magic_enum::enum_name(rotation));
    }
    if (GetKey(olc::Key::PGDN).bPressed)
    {
        --rotation;
        MDCII_LOG_DEBUG("World rotation-- {}", magic_enum::enum_name(rotation));
    }

    // camera
    olc::vf2d vVel = { 0.0f, 0.0f };
    if (GetKey(olc::Key::W).bHeld)
    {
        vVel += { 0, -1 };
    }
    if (GetKey(olc::Key::S).bHeld)
    {
        vVel += { 0, +1 };
    }
    if (GetKey(olc::Key::A).bHeld)
    {
        vVel += { -1, 0 };
    }
    if (GetKey(olc::Key::D).bHeld)
    {
        vVel += { +1, 0 };
    }

    // exit
    if (GetKey(olc::Key::ESCAPE).bHeld)
    {
        return false;
    }

    // update camera
    for (auto const& island : islands)
    {
        island->x += vVel.x * t_elapsedTime * 32.0f;
        island->y += vVel.y * t_elapsedTime * 32.0f;
    }

    // render islands
    // Change the Draw Target to not be Layer 0.
    SetDrawTarget(static_cast<uint8_t>(gameLayer));
    Clear(olc::BLACK);
    m_renderer->Render(this);
    m_stateStack->OnUserUpdate(t_elapsedTime);

    ImGui::ShowDemoWindow();
    */

    m_stateSystem->Input(this);
    m_stateSystem->Render(this, t_elapsedTime);

    return true;
}

void mdcii::Game::DrawImGui()
{
    //pgeImgui.ImGui_ImplPGE_Render();
}
