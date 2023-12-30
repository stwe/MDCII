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
#include "MdciiUtils.h"
#include "state/StateSystem.h"
#include "resource/MdciiResourcesManager.h"

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

bool mdcii::MainMenuState::OnUserCreate(void* t_data)
{
    MDCII_LOG_DEBUG("[MainMenuState::OnUserCreate()] Init MainMenuState.");

    return true;
}

bool mdcii::MainMenuState::OnUserUpdate(const float t_elapsedTime)
{
    const auto center{ ImGui::GetMainViewport()->GetCenter() };
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    constexpr ImGuiWindowFlags flags{ ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings };

    ImGui::Begin("MainMenu", nullptr, flags);

    static auto c{ -1 };

    // Start a new game

    if (ImGui::Button("New Game"))
    {
        ImGui::OpenPopup("Select Map");
        ImGui::End();

        return true;
    }

    if (ImGui::BeginPopupModal("Select Map"))
    {
        c = render_mdcii_file_chooser(game->mdciiResourcesManager->mapFiles);
        if (c >= 0)
        {
            ImGui::EndPopup();
            ImGui::End();

            auto& mdciiFile{ game->mdciiResourcesManager->mapFiles.at(c) };
            return game->stateSystem->ChangeState(state::StateId::NEW_GAME, &mdciiFile);
        }

        if (ImGui::Button("Close"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // Load saved game

    if (ImGui::Button("Load Game"))
    {
        ImGui::OpenPopup("Select Saved Game");
        ImGui::End();

        return true;
    }

    if (ImGui::BeginPopupModal("Select Saved Game"))
    {
        c = render_mdcii_file_chooser(game->mdciiResourcesManager->saveGameFiles);
        if (c >= 0)
        {
            ImGui::EndPopup();
            ImGui::End();

            auto& mdciiFile{ game->mdciiResourcesManager->saveGameFiles.at(c) };
            return game->stateSystem->ChangeState(state::StateId::LOAD_GAME, &mdciiFile);
        }

        if (ImGui::Button("Close"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // Start the island generator

    if (ImGui::Button("Island Generator"))
    {
        ImGui::End();

        return game->stateSystem->ChangeState(state::StateId::ISLAND_GENERATOR, nullptr);
    }

    // Start the world generator

    if (ImGui::Button("World Generator"))
    {
        ImGui::OpenPopup("World Size");
        ImGui::End();

        return true;
    }

    if (ImGui::BeginPopupModal("World Size"))
    {
        static olc::vi2d size{ 96,96 };
        ImGui::SliderInt("World width", &size.x, 96, 128);
        ImGui::SliderInt("World height", &size.y, 96, 128);

        if (ImGui::Button("Generate World"))
        {

            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            ImGui::End();

            return game->stateSystem->ChangeState(state::StateId::WORLD_GENERATOR, &size);
        }

        if (ImGui::Button("Close"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
        ImGui::End();

        return true;
    }

    // Exit game

    if (ImGui::Button("Exit"))
    {
        ImGui::End();

        return false;
    }

    ImGui::End();

    return true;
}
