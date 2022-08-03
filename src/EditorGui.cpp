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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <imgui.h>
#include "EditorGui.h"
#include "Game.h"
#include "Log.h"
#include "map/Map.h"
#include "file/BshFile.h"
#include "data/Text.h"
#include "event/EventManager.h"
#include "map/MapContent.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::EditorGui::EditorGui(std::shared_ptr<map::Map> t_map, std::shared_ptr<data::Buildings> t_buildings)
    : m_map{ std::move(t_map) }
    , m_buildings{ std::move(t_buildings) }
{
    Log::MDCII_LOG_DEBUG("[EditorGui::EditorGui()] Create EditorGui.");
}

mdcii::EditorGui::~EditorGui() noexcept
{
    Log::MDCII_LOG_DEBUG("[EditorGui::~EditorGui()] Destruct EditorGui.");
}

//-------------------------------------------------
// Gui
//-------------------------------------------------

void mdcii::EditorGui::RotateMapGui() const
{
    std::string rotStr{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "CurrentMapRotation") };
    rotStr.append(std::string(": %s"));
    ImGui::Text(rotStr.c_str(), rotation_to_string(m_map->mapContent->rotation));

    ImGui::Separator();

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateMapRight").c_str()))
    {
        m_map->Rotate(map::ChangeRotation::RIGHT);
    }

    ImGui::SameLine();

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateMapLeft").c_str()))
    {
        m_map->Rotate(map::ChangeRotation::LEFT);
    }
}

void mdcii::EditorGui::AllWorkshopsGui() const
{
    if (ImGui::TreeNode(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Workshops").c_str()))
    {
        const auto& shops{ data::Text::GetBuildingsTexts(data::Text::Section::WORKSHOPS, Game::INI.Get<std::string>("locale", "lang")) };

        for (const auto& [k, v] : shops)
        {
            if (ImGui::TreeNode(v.c_str()))
            {
                const auto& building{ m_buildings->buildingsMap.at(std::stoi(k)) };

                const auto textureWidth{ m_map->bauhausBshFile->bshTextures.at(building.baugfx)->width };
                const auto textureHeight{ m_map->bauhausBshFile->bshTextures.at(building.baugfx)->height };
                const auto textureId{ reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(m_map->bauhausBshFile->bshTextures.at(building.baugfx)->textureId)) };

                if (ImGui::ImageButton(
                    textureId,
                    ImVec2(static_cast<float>(textureWidth), static_cast<float>(textureHeight)),
                    ImVec2(0.0f, 0.0f),
                    ImVec2(1.0f, 1.0f),
                    -1,
                    ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
                ))
                {
                    event::EventManager::eventDispatcher.dispatch(
                        event::MdciiEventType::BAUGFX_SELECTED,
                        event::BauGfxSelectedEvent({ std::stoi(k), map::Rotation::DEG0, v })
                    );
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}

void mdcii::EditorGui::WorkshopGui(event::SelectedBauGfx& t_selectedBauGfx) const
{
    if (!t_selectedBauGfx.HasBuilding())
    {
        return;
    }

    ImGui::TextUnformatted(t_selectedBauGfx.name.c_str());

    std::string rotStr{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "CurrentBuildingRotation") };
    rotStr.append(std::string(": %s"));

    ImGui::Text(rotStr.c_str(), rotation_to_string(t_selectedBauGfx.rotation));

    ImGui::Separator();

    const auto& building{ m_buildings->buildingsMap.at(t_selectedBauGfx.buildingId) };

    const auto textureWidth{ m_map->bauhausBshFile->bshTextures.at(building.baugfx)->width };
    const auto textureHeight{ m_map->bauhausBshFile->bshTextures.at(building.baugfx)->height };
    const auto textureId{ reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(
        m_map->bauhausBshFile->bshTextures.at(static_cast<size_t>(building.baugfx) + rotation_to_int(t_selectedBauGfx.rotation))->textureId)
    ) };

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateBuildingRight").c_str()))
    {
        ++t_selectedBauGfx.rotation;
    }

    ImGui::SameLine();

    ImGui::Image(
        textureId,
        ImVec2(static_cast<float>(textureWidth), static_cast<float>(textureHeight)),
        ImVec2(0.0f, 0.0f),
        ImVec2(1.0f, 1.0f),
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
    );

    ImGui::SameLine();

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateBuildingLeft").c_str()))
    {
        --t_selectedBauGfx.rotation;
    }
}
