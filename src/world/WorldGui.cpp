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
#include "state/State.h"
#include "data/Text.h"
#include "file/OriginalResourcesManager.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::WorldGui::WorldGui(World* t_world)
    : m_world{ t_world }
{
    Log::MDCII_LOG_DEBUG("[WorldGui::WorldGui()] Create WorldGui.");

    MDCII_ASSERT(t_world, "[WorldGui::WorldGui()] Null pointer.")

    data::Text::Init();
    InitBauhausZoom();
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
    std::string worldRotation{ magic_enum::enum_name(m_world->rotation) };
    ImGui::TextUnformatted(rotStr.append(": ").append(worldRotation).c_str());

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateMapRight").c_str()))
    {
        m_world->RotateWorld(ChangeRotation::RIGHT);
    }

    ImGui::SameLine();

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateMapLeft").c_str()))
    {
        m_world->RotateWorld(ChangeRotation::LEFT);
    }
}

void mdcii::world::WorldGui::ZoomGui() const
{
    std::string zoomStr{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "CurrentMapZoom") };
    std::string worldZoom{ magic_enum::enum_name(m_world->zoom) };
    ImGui::TextUnformatted(zoomStr.append(": ").append(worldZoom).c_str());

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "ZoomMapIn").c_str()))
    {
        m_world->ZoomWorld(ChangeZoom::ZOOM_IN);
    }

    ImGui::SameLine();

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "ZoomMapOut").c_str()))
    {
        m_world->ZoomWorld(ChangeZoom::ZOOM_OUT);
    }
}

void mdcii::world::WorldGui::ShowActionsGui() const
{
    magic_enum::enum_for_each<World::Action>([this](auto t_val) {
        constexpr World::Action action{ t_val };
        constexpr int i{ magic_enum::enum_integer(action) };

        if (m_world->actionButtons[i])
        {
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(7.0f, 0.6f, 0.6f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(7.0f, 0.7f, 0.7f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(7.0f, 0.8f, 0.8f)));

            ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), World::ACTION_NAMES[i].data()).c_str());
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && action != m_world->currentAction)
            {
                m_world->actionButtons[i] = !m_world->actionButtons[i];
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

void mdcii::world::WorldGui::AllWorkshopsGui()
{
    if (selectedWorkshop.HasBuilding())
    {
        WorkshopGui();
        ImGui::Separator();
    }

    if (ImGui::TreeNode(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Workshops").c_str()))
    {
        const auto& shops{ data::Text::GetBuildingsTexts(data::Text::Section::WORKSHOPS, Game::INI.Get<std::string>("locale", "lang")) };
        for (const auto& [k, v] : shops)
        {
            if (ImGui::TreeNode(v.c_str()))
            {
                const auto& building{ m_world->context->originalResourcesManager->GetBuildingById(std::stoi(k)) };
                const auto& bauhausBshTextures{ m_world->context->originalResourcesManager->GetBauhausBshByZoom(m_bauhausZoom) };

                if (ImGui::ImageButton(
                        reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(bauhausBshTextures.at(building.baugfx)->textureId)),
                        ImVec2(static_cast<float>(bauhausBshTextures.at(building.baugfx)->width), static_cast<float>(bauhausBshTextures.at(building.baugfx)->height)),
                        ImVec2(0.0f, 0.0f),
                        ImVec2(1.0f, 1.0f),
                        -1,
                        ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
                    ))
                {
                    Log::MDCII_LOG_DEBUG("[WorldGui::AllWorkshopsGui()] Select k: {}, v: {}", std::stoi(k), v);

                    selectedWorkshop.buildingId = std::stoi(k);
                    selectedWorkshop.rotation = Rotation::DEG0;
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}

void mdcii::world::WorldGui::SaveGameGui()
{
    const auto fileName{ Game::RESOURCES_REL_PATH + Game::INI.Get<std::string>("content", "save_map") };

    const auto str{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "SaveGame") + fileName };
    if (ImGui::Button(str.c_str()))
    {
        Log::MDCII_LOG_DEBUG("[WorldGui::SaveGameGui()] Start saving the game in file {}...", fileName);

        nlohmann::json j;
        nlohmann::json t = nlohmann::json::object();
        nlohmann::json b = nlohmann::json::object();
        std::ofstream file{ fileName };
        if (!file)
        {
            throw MDCII_EXCEPTION("[WorldGui::SaveGameGui()] Error while opening file " + fileName + ".");
        }

        j["width"] = m_world->width;
        j["height"] = m_world->height;
        j["layers"] = nlohmann::json::array();
        t["terrain"] = m_world->GetLayer(WorldLayerType::TERRAIN).tiles;
        b["buildings"] = m_world->GetLayer(WorldLayerType::BUILDINGS).tiles;
        j["layers"].push_back(t);
        j["layers"].push_back(b);

        file << j;

        Log::MDCII_LOG_DEBUG("[WorldGui::SaveGameGui()] The game has been successfully saved in file {}.", fileName);
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::WorldGui::InitBauhausZoom()
{
    // check for History Ed.
    if (m_world->context->originalResourcesManager->bauhausBshFiles.size() == 1)
    {
        Log::MDCII_LOG_DEBUG("[WorldGui::InitBauhausZoom()] History Ed.: Sets the thumbnail zoom for buildings to GFX.");

        m_bauhausZoom = Zoom::GFX;
        return;
    }

    // Nina has 3 Bauhaus.bsh files
    const auto zoomOptional{ magic_enum::enum_cast<Zoom>(Game::INI.Get<std::string>("main_menu", "thumbnails_zoom")) };
    if (zoomOptional.has_value())
    {
        const auto z{ zoomOptional.value() };
        m_bauhausZoom = z;
    }
    else
    {
        m_bauhausZoom = Zoom::GFX;
    }
}

void mdcii::world::WorldGui::WorkshopGui()
{
    if (!selectedWorkshop.HasBuilding())
    {
        return;
    }

    std::string rotStr{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "CurrentBuildingRotation") };
    std::string workshopRotation{ rotation_to_string(selectedWorkshop.rotation) };
    ImGui::TextUnformatted(rotStr.append(": ").append(workshopRotation).c_str());

    ImGui::Separator();

    const auto& building{ m_world->context->originalResourcesManager->GetBuildingById(selectedWorkshop.buildingId) };
    const auto& bauhausBshTextures{ m_world->context->originalResourcesManager->GetBauhausBshByZoom(m_bauhausZoom) };

    const auto textureWidth{ bauhausBshTextures.at(building.baugfx)->width };
    const auto textureHeight{ bauhausBshTextures.at(building.baugfx)->height };
    auto* const textureId{ reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(
        bauhausBshTextures.at(static_cast<size_t>(building.baugfx) + magic_enum::enum_integer(selectedWorkshop.rotation))->textureId
    )
    ) };

    if (ImGui::Button(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateBuildingRight").c_str()))
    {
        ++selectedWorkshop.rotation;
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
        --selectedWorkshop.rotation;
    }
}
