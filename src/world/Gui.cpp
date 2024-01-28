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

mdcii::world::SelectBuilding mdcii::world::Gui::select_building{ nullptr, Rotation::DEG0 };

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::world::Gui::RenderAddBuildingsGui(const Game* t_game)
{
    MDCII_ASSERT(t_game, "[Gui::RenderAddBuildingsGui()] Null pointer.")

    static constexpr std::array<int, 16> workshopsBuildingIds{
        501, 503, 505, 507, 509, 511, 513, 515,
        517, 519, 521, 525, 527, 529, 531, 533
    };

    static std::array<std::string_view, 16> workshopNames{
        _("Wind mill"), _("Bakery"), _("Taylor"), _("Weaving hut"),
        _("Water mill"), _("Butcher"), _("Stonemason"), _("Tool smithy"),
        _("Swordsmith"), _("Rum distillery"), _("Ore refinery"), _("Tobacco products"),
        _("Cannon foundry"), _("Gunsmith"), _("Weaving mill"), _("Goldsmith")
    };

    const auto zoomOpt{ magic_enum::enum_cast<Zoom>(Game::INI.Get<std::string>("menu", "bauhaus_zoom")) };
    const auto& bshTextures{ t_game->originalResourcesManager->bauhausBshFiles[
        zoomOpt.value_or(Zoom::GFX)
    ]->bshTextures };

    if (select_building.building)
    {
        RotatableBuildingGui(select_building.building);
    }

    if (ImGui::TreeNode(_("Workshops")))
    {
        auto i{ 0 };
        for (const auto buildingId : workshopsBuildingIds)
        {
            if (ImGui::TreeNode(workshopNames.at(i).data()))
            {
                const auto& building{ t_game->originalResourcesManager->GetBuildingById(buildingId) };

                if (const auto& renderableBauhaus{ bshTextures.at(building.baugfx + magic_enum::enum_integer(select_building.rotation))->decal };
                    ImGui::ImageButton(
                        reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(renderableBauhaus->id)),  // texture-Id
                        ImVec2(static_cast<float>(renderableBauhaus->sprite->width),
                            static_cast<float>(renderableBauhaus->sprite->height)),                    // size
                        ImVec2(0.0f, 0.0f),            // uv0
                        ImVec2(1.0f, 1.0f),            // uv1
                        -1,                            // frame padding
                        ImVec4(0.0f, 0.0f, 0.0f, 1.0f) // bg color
                    ))
                {
                    MDCII_LOG_DEBUG("[Gui::RenderAddBuildingsGui()] Select Building {}", workshopNames.at(i).data());
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

        ImGui::Separator();
    }
}
