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
#include "Terrain.h"
#include "Island.h"
#include "MdciiAssert.h"
#include "Game.h"
#include "state/State.h"
#include "data/Text.h"
#include "file/OriginalResourcesManager.h"
#include "layer/TerrainLayer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::WorldGui::WorldGui(World* t_world)
    : m_world{ t_world }
{
    Log::MDCII_LOG_DEBUG("[WorldGui::WorldGui()] Create WorldGui.");

    MDCII_ASSERT(m_world, "[WorldGui::WorldGui()] Null pointer.")

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

    static bool r = false;
    static bool l = false;
    m_world->rotation == Rotation::DEG270 ? r = true : r = false;
    m_world->rotation == Rotation::DEG0 ? l = true : l = false;

    ImGui::BeginDisabled(r);
    if (ImGui::Button("->##MapRot"))
    {
        m_world->RotateWorld(ChangeRotation::RIGHT);
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(66, 104, 188, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::SetTooltip("%s", data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateMapRight").c_str());
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    ImGui::EndDisabled();

    ImGui::SameLine();

    ImGui::BeginDisabled(l);
    if (ImGui::Button("<-##MapRot"))
    {
        m_world->RotateWorld(ChangeRotation::LEFT);
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(66, 104, 188, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::SetTooltip("%s", data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateMapLeft").c_str());
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    ImGui::EndDisabled();
}

void mdcii::world::WorldGui::ZoomGui() const
{
    std::string zoomStr{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "CurrentMapZoom") };
    std::string worldZoom{ magic_enum::enum_name(m_world->zoom) };
    ImGui::TextUnformatted(zoomStr.append(": ").append(worldZoom).c_str());

    static bool i = false;
    static bool o = false;
    m_world->zoom == Zoom::GFX ? i = true : i = false;
    m_world->zoom == Zoom::SGFX ? o = true : o = false;

    ImGui::BeginDisabled(i);
    if (ImGui::Button("+##MapZoom"))
    {
        m_world->ZoomWorld(ChangeZoom::ZOOM_IN);
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(66, 104, 188, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::SetTooltip("%s", data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "ZoomMapIn").c_str());
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    ImGui::EndDisabled();

    ImGui::SameLine();

    ImGui::BeginDisabled(o);
    if (ImGui::Button("-##MapZoom"))
    {
        m_world->ZoomWorld(ChangeZoom::ZOOM_OUT);
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(66, 104, 188, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::SetTooltip("%s", data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "ZoomMapOut").c_str());
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    ImGui::EndDisabled();
}

void mdcii::world::WorldGui::ShowActionsGui()
{
    magic_enum::enum_for_each<World::Action>([this](const auto t_val) {
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

                // reset current selected island && selected tile
                if (m_world->terrain->currentSelectedIsland && m_world->terrain->currentSelectedIsland->currentSelectedTile)
                {
                    m_world->terrain->currentSelectedIsland->currentSelectedTile = nullptr;
                    m_world->terrain->currentSelectedIsland = nullptr;
                }

                // reset selected building
                if (action != world::World::Action::BUILD && selectedBuildingTile.HasBuilding())
                {
                    selectedBuildingTile.Reset();
                }

                m_world->actionButtons[i] = true;
                m_world->currentAction = action;

                Log::MDCII_LOG_DEBUG("[WorldGui::ShowActionsGui()] Change to action: {}", magic_enum::enum_name(m_world->currentAction));
            }

            ImGui::SameLine();
        }
    });

    ImGui::NewLine();
}

void mdcii::world::WorldGui::ShowBuildingsGui()
{
    if (selectedBuildingTile.HasBuilding())
    {
        const auto it{ data::NON_ROTATABLE_BUILDING_IDS.find(selectedBuildingTile.buildingId) };
        it != data::NON_ROTATABLE_BUILDING_IDS.end() ? NonRotatableBuildingGui() : RotatableBuildingGui();
    }

    BuildingsSectionGui(data::Section::HOUSES);
    BuildingsSectionGui(data::Section::PUBLIC);
    BuildingsSectionGui(data::Section::FARMS);
    BuildingsSectionGui(data::Section::WORKSHOPS);
    BuildingsSectionGui(data::Section::WATER);
    BuildingsSectionGui(data::Section::MILITARY);
}

void mdcii::world::WorldGui::SaveGameGui()
{
    const auto fileName{ Game::RESOURCES_REL_PATH + Game::INI.Get<std::string>("content", "save_game_map") };

    const auto str{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "SaveGame") + fileName };
    if (ImGui::Button(str.c_str()))
    {
        Log::MDCII_LOG_DEBUG("[WorldGui::SaveGameGui()] Start saving the game in file {}...", fileName);

        nlohmann::json json;

        std::ofstream file{ fileName };
        if (!file)
        {
            throw MDCII_EXCEPTION("[WorldGui::SaveGameGui()] Error while opening file " + fileName + ".");
        }

        // version
        json["version"] = Game::VERSION;

        // world
        json["world"] = *m_world;

        // islands
        auto islandsJson = nlohmann::json::array();
        for (const auto& island : m_world->terrain->islands)
        {
            // island
            auto islandJson = nlohmann::json::object();
            islandJson = *island;
            islandJson["layers"] = nlohmann::json::array();

            // coast
            auto c = nlohmann::json::object();
            c["coast"] = island->coastLayer->tiles;

            // terrain
            auto t = nlohmann::json::object();
            t["terrain"] = island->terrainLayer->tiles;

            // buildings
            auto b = nlohmann::json::object();
            b["buildings"] = island->buildingsLayer->tiles;

            islandJson["layers"].push_back(c);
            islandJson["layers"].push_back(t);
            islandJson["layers"].push_back(b);

            islandsJson.push_back(islandJson);
        }

        json["islands"] = islandsJson;

        file << json;

        Log::MDCII_LOG_DEBUG("[WorldGui::SaveGameGui()] The game has been successfully saved in file {}.", fileName);
    }
}

//-------------------------------------------------
// Buildings
//-------------------------------------------------

void mdcii::world::WorldGui::NonRotatableBuildingGui() const
{
    const auto& building{ m_world->context->originalResourcesManager->GetBuildingById(selectedBuildingTile.buildingId) };
    const auto& bauhausBshTextures{ m_world->context->originalResourcesManager->GetBauhausBshByZoom(m_bauhausZoom) };

    const auto textureWidth{ bauhausBshTextures.at(building.baugfx)->width };
    const auto textureHeight{ bauhausBshTextures.at(building.baugfx)->height };
    auto* const textureId{ reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(
        bauhausBshTextures.at(static_cast<size_t>(building.baugfx) + magic_enum::enum_integer(selectedBuildingTile.rotation))->textureId
    )
    ) };

    ImGui::Image(
        textureId,
        ImVec2(static_cast<float>(textureWidth), static_cast<float>(textureHeight)),
        ImVec2(0.0f, 0.0f),
        ImVec2(1.0f, 1.0f),
        ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
    );

    ImGui::Separator();
}

void mdcii::world::WorldGui::RotatableBuildingGui()
{
    std::string rotStr{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "CurrentBuildingRotation") };
    std::string workshopRotation{ rotation_to_string(selectedBuildingTile.rotation) };
    ImGui::TextUnformatted(rotStr.append(": ").append(workshopRotation).c_str());

    ImGui::Separator();

    const auto& building{ m_world->context->originalResourcesManager->GetBuildingById(selectedBuildingTile.buildingId) };
    const auto& bauhausBshTextures{ m_world->context->originalResourcesManager->GetBauhausBshByZoom(m_bauhausZoom) };

    const auto textureWidth{ bauhausBshTextures.at(building.baugfx)->width };
    const auto textureHeight{ bauhausBshTextures.at(building.baugfx)->height };
    auto* const textureId{ reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(
        bauhausBshTextures.at(static_cast<size_t>(building.baugfx) + magic_enum::enum_integer(selectedBuildingTile.rotation))->textureId
    )
    ) };

    static bool r = false;
    static bool l = false;
    selectedBuildingTile.rotation == Rotation::DEG270 ? r = true : r = false;
    selectedBuildingTile.rotation == Rotation::DEG0 ? l = true : l = false;

    ImGui::BeginDisabled(r);
    if (ImGui::Button("->##BuildingRot"))
    {
        ++selectedBuildingTile.rotation;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(66, 104, 188, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::SetTooltip("%s", data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateBuildingRight").c_str());
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    ImGui::EndDisabled();

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

    ImGui::BeginDisabled(l);
    if (ImGui::Button("<-##BuildingRot"))
    {
        --selectedBuildingTile.rotation;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(66, 104, 188, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::SetTooltip("%s", data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "RotateBuildingLeft").c_str());
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    ImGui::EndDisabled();

    ImGui::Separator();
}

void mdcii::world::WorldGui::BuildingsSectionGui(const data::Section t_section)
{
    // Section toString
    std::string name{ magic_enum::enum_name(t_section) };
    std::string lc{ to_lower_case(name) };
    lc[0] = static_cast<char>(toupper(lc[0]));

    if (ImGui::TreeNode(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), lc).c_str()))
    {
        const auto& buildings{ data::Text::GetBuildingsTexts(t_section, Game::INI.Get<std::string>("locale", "lang")) };
        for (const auto& [k, v] : buildings)
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
                    Log::MDCII_LOG_DEBUG("[WorldGui::BuildingsSectionGui()] Select k: {}, v: {}", std::stoi(k), v);

                    selectedBuildingTile.buildingId = std::stoi(k);
                    selectedBuildingTile.rotation = Rotation::DEG0;
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
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
