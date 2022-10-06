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
#include "WorldGui.h"
#include "World.h"
#include "MdciiAssert.h"
#include "Game.h"
#include "data/Text.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::WorldGui::WorldGui(World* t_world)
    : m_world{ t_world }
{
    Log::MDCII_LOG_DEBUG("[WorldGui::WorldGui()] Create WorldGui.");

    data::Text::Init();

    MDCII_ASSERT(t_world, "[WorldGui::WorldGui()] Null pointer.")
}

mdcii::world::WorldGui::~WorldGui() noexcept
{
    Log::MDCII_LOG_DEBUG("[WorldGui::~WorldGui()] Destruct WorldGui.");
}

//-------------------------------------------------
// GUIs
//-------------------------------------------------

void mdcii::world::WorldGui::RotateGui() const
{
    std::string rotStr{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "CurrentMapRotation") };
    rotStr.append(std::string(": %s"));
    ImGui::Text(rotStr.c_str(), rotation_to_string(m_world->rotation));

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateMapRight").c_str()))
    {
        m_world->Rotate(map::ChangeRotation::RIGHT);
    }

    ImGui::SameLine();

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateMapLeft").c_str()))
    {
        m_world->Rotate(map::ChangeRotation::LEFT);
    }
}

void mdcii::world::WorldGui::ZoomGui() const
{
    std::string zoomStr{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "CurrentMapZoom") };
    zoomStr.append(std::string(": %s"));
    ImGui::Text(zoomStr.c_str(), magic_enum::enum_name(m_world->zoom).data());

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "ZoomMapIn").c_str()))
    {
        m_world->Zoom(map::ChangeZoom::ZOOM_IN);
    }

    ImGui::SameLine();

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "ZoomMapOut").c_str()))
    {
        m_world->Zoom(map::ChangeZoom::ZOOM_OUT);
    }
}

void mdcii::world::WorldGui::ShowActionsGui()
{
    magic_enum::enum_for_each<World::Action>([&](auto t_val) {
        constexpr World::Action action{ t_val };
        constexpr int i{ magic_enum::enum_integer(action) };

        if (m_world->actionButtons[i])
        {
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(7.0f, 0.6f, 0.6f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(7.0f, 0.7f, 0.7f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(7.0f, 0.8f, 0.8f)));

            ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), World::ACTION_NAMES[i].data()).c_str());
            if (ImGui::IsItemClicked(0))
            {
                if (action != m_world->currentAction)
                {
                    m_world->actionButtons[i] = !m_world->actionButtons[i];
                }
            }

            ImGui::PopStyleColor(3);
            ImGui::PopID();

            ImGui::SameLine();
        }
        else
        {
            if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), World::ACTION_NAMES[i].data()).c_str()))
            {
                std::fill(m_world->actionButtons.begin(), m_world->actionButtons.end(), false);
                m_world->actionButtons[i] = true;
                m_world->currentAction = action;

                Log::MDCII_LOG_DEBUG("[WorldGui::ShowActionsGui()] Change to action: {}", magic_enum::enum_name(m_world->currentAction));
            }

            ImGui::SameLine();
        }
    });

    ImGui::NewLine();
}
