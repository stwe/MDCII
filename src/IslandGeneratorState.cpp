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

#include "IslandGeneratorState.h"
#include "Log.h"
#include "Game.h"
#include "world/IslandGenerator.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::IslandGeneratorState::IslandGeneratorState(Game* t_game)
    : State(t_game)
{
    MDCII_LOG_DEBUG("[IslandGeneratorState::IslandGeneratorState()] Create IslandGeneratorState.");
}

mdcii::IslandGeneratorState::~IslandGeneratorState() noexcept
{
    MDCII_LOG_DEBUG("[IslandGeneratorState::~IslandGeneratorState()] Destruct IslandGeneratorState.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

bool mdcii::IslandGeneratorState::OnUserCreate()
{
    MDCII_LOG_DEBUG("[IslandGeneratorState::OnUserCreate()] Init IslandGeneratorState.");

    m_islandGenerator = std::make_unique<world::IslandGenerator>();

    return true;
}

bool mdcii::IslandGeneratorState::OnUserUpdate(const float t_elapsedTime)
{
    ImGui::Begin("Menu");

    if (ImGui::Button("Exit Island Generator"))
    {
        ImGui::End();

        return false;
    }

    ImGui::Separator();

    m_islandGenerator->RenderImGui();

    ImGui::End();

    return true;
}
