// This file is part of the MDCII project.
//
// Copyright (c) 2024. stwe <https://github.com/stwe/MDCII>
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

#include "WorldGenerator.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "World.h"
#include "Island.h"
#include "MdciiUtils.h"
#include "MousePicker.h"
#include "state/State.h"
#include "resource/MdciiResourcesManager.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::WorldGenerator::WorldGenerator(state::State* t_state)
    : m_state{ t_state }
{
    MDCII_LOG_DEBUG("[WorldGenerator::WorldGenerator()] Create WorldGenerator.");

    MDCII_ASSERT(m_state, "[WorldGenerator::WorldGenerator()] Null pointer.")
}

mdcii::world::WorldGenerator::~WorldGenerator() noexcept
{
    MDCII_LOG_DEBUG("[WorldGenerator::~WorldGenerator()] Destruct WorldGenerator.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::WorldGenerator::OnUserUpdate(const float t_elapsedTime)
{
    world->OnUserUpdate(t_elapsedTime);

    // FindVisibleIslands();
    if (m_currentIsland)
    {
        m_currentIsland->startX = world->mousePicker->selected.x;
        m_currentIsland->startY = world->mousePicker->selected.y;
    }

    if (world->state->game->GetMouse(0).bPressed && m_currentIsland)
    {
        m_currentIsland = nullptr;
    }

    if (m_state->game->mdciiResourcesManager->islandFiles.empty())
    {
        ImGui::Text("Missing Files");
    }
    else
    {
        m_island_file_index = render_mdcii_file_chooser(m_state->game->mdciiResourcesManager->islandFiles);
    }

    if (m_island_file_index >= 0)
    {
        auto& fileName{ m_state->game->mdciiResourcesManager->islandFiles.at(m_island_file_index) };
        if (fileName.SetJsonFromFile())
        {
            auto island{ std::make_unique<Island>(world.get(), fileName.GetJson()) };
            island->startX = 0;
            island->startY = 0;

            world->islands.push_back(std::move(island));
            m_currentIsland = world->islands.back().get();

            MDCII_LOG_DEBUG("[WorldGenerator::OnUserUpdate()] Add island {}.", m_state->game->mdciiResourcesManager->islandFiles.at(m_island_file_index).GetFileName());
        }
    }

    SaveWorldImGui();
}

void mdcii::world::WorldGenerator::SaveWorldImGui() const
{
    static bool error{ false };
    static bool saved{ false };
    static std::string fileName;
    save_file_button("Save World", &fileName);

    if (!fileName.empty())
    {
        if (ImGui::Button("Save"))
        {
            resource::MdciiFile mdciiFile{ fileName };
            mdciiFile.GetJson() = *world;

            if (mdciiFile.CreateFileFromJson(resource::MdciiFileType::MAP))
            {
                saved = true;
                fileName.clear();
            }
            else
            {
                error = true;
                saved = false;
                fileName.clear();
            }
        }
    }

    if (error)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
        ImGui::Text("Save Error");
        ImGui::PopStyleColor();
    }

    if (saved)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        ImGui::Text("Save Success");
        ImGui::PopStyleColor();
    }
}
