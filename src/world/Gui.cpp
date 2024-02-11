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

#include "Gui.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "Intl.h"
#include "Rotation.h"
#include "resource/OriginalResourcesManager.h"
#include "resource/BshFile.h"
#include "world/World.h"

mdcii::world::SelectBuilding mdcii::world::Gui::select_building{ nullptr, Rotation::DEG0 };

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::world::Gui::RenderAddBuildingsGui(const Game* t_game)
{
    MDCII_ASSERT(t_game, "[Gui::RenderAddBuildingsGui()] Null pointer.")

    std::vector<std::string_view> craftBuildingNames{
        _("Wind mill"), _("Bakery"), _("Taylor"), _("Weaving hut"),
        _("Water mill"), _("Butcher"), _("Stonemason"), _("Tool smithy"),
        _("Swordsmith"), _("Rum distillery"), _("Ore refinery"), _("Tobacco products"),
        _("Cannon foundry"), _("Gunsmith"), _("Weaving mill"), _("Goldsmith")
    };

    std::vector<std::string_view> residentialBuildingNames{
        _("Pioneers"), _("Settlers"), _("Citizens"), _("Merchants"), _("Aristocrats"),
    };

    std::vector<std::string_view> publicBuildingNames{
        _("Church"), _("Tavern"), _("Market place"), _("Fire department"), _("Doctor"), _("University"), _("School")
    };

    std::vector<std::string_view> militaryBuildingNames{
        _("City Gate"), _("City Wall")
    };

    std::vector<std::string_view> coastalBuildingNames{
        _("Fishers hut"), _("Warehouse coast"), _("Warehouse 1"), _("Warehouse 2"), _("Warehouse 3"), _("Warehouse 4")
    };

    if (select_building.building)
    {
        RotatableBuildingGui(select_building.building);
    }

    RenderBuildingsByTitleGui(t_game, "Craft buildings", resource::CRAFT_BUILDING_IDS, craftBuildingNames);
    RenderBuildingsByTitleGui(t_game, "Residential buildings", resource::RESIDENTIAL_BUILDING_IDS, residentialBuildingNames);
    RenderBuildingsByTitleGui(t_game, "Public buildings", resource::PUBLIC_BUILDING_IDS, publicBuildingNames);
    RenderBuildingsByTitleGui(t_game, "Military buildings", resource::MILITARY_BUILDING_IDS, militaryBuildingNames);
    RenderBuildingsByTitleGui(t_game, "Coastal buildings", resource::COASTAL_BUILDING_IDS, coastalBuildingNames);
}

void mdcii::world::Gui::RotatableBuildingGui(const resource::Building* t_building)
{
    MDCII_ASSERT(t_building, "[Gui::RotatableBuildingGui()] Null pointer.")

    if (t_building && t_building->IsRotatable())
    {
        static bool r = false;
        static bool l = false;
        select_building.rotation == Rotation::DEG270 ? r = true : r = false;
        select_building.rotation == Rotation::DEG0 ? l = true : l = false;

        ImGui::BeginDisabled(r);
        if (ImGui::Button("->##BuildingRot"))
        {
            MDCII_LOG_DEBUG("[Gui::RotatableBuildingGui()] Rotate building right.");
            ++select_building.rotation;
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(66, 104, 188, 255));
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
            ImGui::SetTooltip("%s", _("Rotate building right"));
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();

        ImGui::SameLine();

        ImGui::BeginDisabled(l);
        if (ImGui::Button("<-##BuildingRot"))
        {
            MDCII_LOG_DEBUG("[Gui::RotatableBuildingGui()] Rotate building left.");
            --select_building.rotation;
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(66, 104, 188, 255));
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
            ImGui::SetTooltip("%s", _("Rotate building left"));
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }
        ImGui::EndDisabled();

        ImGui::Text("Building rotation %s", magic_enum::enum_name(select_building.rotation).data());

        ImGui::Separator();
    }
}

void mdcii::world::Gui::SaveGameGui(const World* t_world)
{
    const auto fileName{ Game::RESOURCES_REL_PATH + Game::INI.Get<std::string>("content", "save_game_map") };

    if (ImGui::Button(_("Save Game")))
    {
        MDCII_LOG_DEBUG("[Gui::SaveGameGui()] Start saving the game in file {}...", fileName);

        nlohmann::json json;

        std::ofstream file{ fileName };
        if (!file)
        {
            throw MDCII_EXCEPTION("[Gui::SaveGameGui()] Error while opening file " + fileName + ".");
        }

        json = *t_world;

        file << json;

        MDCII_LOG_DEBUG("[Gui::SaveGameGui()] The game has been successfully saved in file {}.", fileName);
    }
}

void mdcii::world::Gui::RenderBuildingsByTitleGui(
    const Game* t_game,
    const std::string& t_title,
    const std::vector<int>& t_buildingIds,
    const std::vector<std::string_view>& t_buildingNames
)
{
    const auto zoomOpt{ magic_enum::enum_cast<Zoom>(Game::INI.Get<std::string>("menu", "bauhaus_zoom")) };
    const auto& bshTextures{ t_game->originalResourcesManager->bauhausBshFiles[zoomOpt.value_or(Zoom::GFX)]->bshTextures };

    if (ImGui::TreeNode(_(t_title.c_str())))
    {
        auto i{ 0 };
        for (const auto buildingId : t_buildingIds)
        {
            if (ImGui::TreeNode(t_buildingNames.at(i).data()))
            {
                const auto& building{ t_game->originalResourcesManager->GetBuildingById(buildingId) };

                if (const auto& renderableBauhaus{ bshTextures.at(building.baugfx + magic_enum::enum_integer(select_building.rotation))->decal };
                    ImGui::ImageButton(
                        reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(renderableBauhaus->id)),  // texture-Id
                        ImVec2(static_cast<float>(renderableBauhaus->sprite->width),
                               static_cast<float>(renderableBauhaus->sprite->height)),  // size
                        ImVec2(0.0f, 0.0f),                                     // uv0
                        ImVec2(1.0f, 1.0f),                                     // uv1
                        -1,                                                   // frame padding
                        ImVec4(0.0f, 0.0f, 0.0f, 1.0f)                  // bg color
                    ))
                {
                    MDCII_LOG_DEBUG("[Gui::RenderBuildingsByTitleGui()] Select building {}", t_buildingNames.at(i).data());

                    select_building.building = &building;
                    select_building.rotation = Rotation::DEG0;
                }

                ImGui::TreePop();
            }

            i++;
        }

        ImGui::TreePop();
    }
}
