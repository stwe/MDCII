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

#include "MainMenuState.h"
#include "Log.h"
#include "Game.h"
#include "state/StateSystem.h"

mdcii::MainMenuState::MainMenuState(Game* t_game)
    : State(t_game)
{
    MDCII_LOG_DEBUG("[MainMenuState::MainMenuState()] Create MainMenuState.");
}

mdcii::MainMenuState::~MainMenuState() noexcept
{
    MDCII_LOG_DEBUG("[MainMenuState::~MainMenuState()] Destruct MainMenuState.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

bool mdcii::MainMenuState::OnUserCreate()
{
    MDCII_LOG_DEBUG("[MainMenuState::OnUserCreate()] Init MainMenuState.");

    return true;
}

bool mdcii::MainMenuState::OnUserUpdate(const float t_elapsedTime)
{
    game->SetGameLayer();

    const auto center{ ImGui::GetMainViewport()->GetCenter() };
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    constexpr ImGuiWindowFlags flags{ ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings };

    ImGui::Begin("MainMenu", nullptr, flags);
    if (ImGui::Button("New Game"))
    {
        game->stateSystem->ChangeState(state::StateId::NEW_GAME);
        ImGui::End();
        return game->stateSystem->OnUserCreate();
    }
    if (ImGui::Button("Exit"))
    {
        ImGui::End();
        return false;
    }

    ImGui::End();

    return true;
}
