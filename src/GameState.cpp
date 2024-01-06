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

#include "GameState.h"
#include "MdciiAssert.h"
#include "Game.h"
#include "Intl.h"
#include "world/World.h"
#include "resource/MdciiResourcesManager.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::GameState::GameState(Game* t_game)
    : State(t_game)
{
    MDCII_LOG_DEBUG("[GameState::GameState()] Create GameState.");
}

mdcii::GameState::~GameState() noexcept
{
    MDCII_LOG_DEBUG("[GameState::~GameState()] Destruct GameState.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

bool mdcii::GameState::OnUserCreate(void* t_data)
{
    MDCII_LOG_DEBUG("[GameState::OnUserCreate()] Init GameState.");

    auto* data{ reinterpret_cast<resource::MdciiFile*>(t_data) };
    MDCII_ASSERT(data, "[GameState::OnUserCreate()] Null pointer.")

    return CreateWorldFromFile(*data);
}

bool mdcii::GameState::OnUserUpdate(const float t_elapsedTime)
{
    // exit
    if (game->GetKey(olc::Key::ESCAPE).bHeld)
    {
        return false;
    }

    // world
    game->SetGameLayer();
    game->Clear(olc::BLACK);
    m_world->OnUserUpdate(t_elapsedTime);

    return RenderImGui();
}

//-------------------------------------------------
// Create world
//-------------------------------------------------

bool mdcii::GameState::CreateWorldFromFile(resource::MdciiFile& t_mdciiFile)
{
    MDCII_LOG_DEBUG("[GameState::CreateWorldFromFile()] Create a world from MDCII Json file {}.", t_mdciiFile.GetFileName());
    MDCII_ASSERT(!m_world, "[GameState::CreateWorldFromFile()] Invalid world pointer.")

    if (t_mdciiFile.SetJsonFromFile())
    {
        m_world = std::make_unique<world::World>(this, t_mdciiFile.GetJson());

        return true;
    }

    return false;
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

bool mdcii::GameState::RenderImGui() const
{
    ImGui::SetNextWindowPos(
        { ImGui::GetMainViewport()->Size.x - 250.0f, 0.0f },
        ImGuiCond_Appearing
    );
    constexpr ImGuiWindowFlags flags{ ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings };

    ImGui::Begin(_("Game Menu"), nullptr, flags);
    if (ImGui::Button(_("Exit Game")))
    {
        ImGui::End();
        return false;
    }

    m_world->RenderImGui();

    ImGui::End();

    return true;
}
