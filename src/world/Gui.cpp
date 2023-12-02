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

#include <array>
#include <string_view>
#include "Gui.h"
#include "Game.h"
#include "Log.h"
#include "Intl.h"
#include "resource/OriginalResourcesManager.h"
#include "resource/BshFile.h"

//-------------------------------------------------
// ImGui
//-------------------------------------------------

std::optional<int> mdcii::world::Gui::RenderAddBuildingsGui(const Game* t_game)
{
    static constexpr std::array<int, 16> workshops{ 501, 503, 505, 507, 509, 511, 513, 515,
                                   517, 519, 521, 525, 527, 529, 531, 533 };

    static std::array<std::string_view, 16> names{
        _("Wind mill"), _("Bakery"), _("Taylor"), _("Weaving hut"),
        _("Water mill"), _("Butcher"), _("Stonemason"), _("Tool smithy"),
        _("Swordsmith"), _("Rum distillery"), _("Ore refinery"), _("Tobacco products"),
        _("Cannon foundry"), _("Gunsmith"), _("Weaving mill"), _("Goldsmith")
    };

    auto zoomOpt{ magic_enum::enum_cast<Zoom>(Game::INI.Get<std::string>("menu", "bauhaus_zoom")) };
    const auto& bshTextures{ t_game->originalResourcesManager->bauhausBshFiles.at(zoomOpt.value_or(Zoom::GFX))->bshTextures };

    auto result{ -1 };

    if (ImGui::TreeNode(_("Workshops")))
    {
        auto i{ 0 };
        for (const auto buildingId : workshops)
        {
            if (ImGui::TreeNode(names.at(i).data()))
            {
                const auto& building{ t_game->originalResourcesManager->GetBuildingById(buildingId) };
                const auto& renderableBauhaus{ bshTextures.at(building.baugfx)->decal };

                if (ImGui::ImageButton(
                    reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(renderableBauhaus->id)),                                        // textId
                    ImVec2(static_cast<float>(renderableBauhaus->sprite->width), static_cast<float>(renderableBauhaus->sprite->height)), // size
                    ImVec2(0.0f, 0.0f),            // uv0
                    ImVec2(1.0f, 1.0f),            // uv1
                    -1,                            // frame padding
                    ImVec4(0.0f, 0.0f, 0.0f, 1.0f) // bg color
                ))
                {
                    MDCII_LOG_DEBUG("[Gui::RenderAddBuildingsGui()] Select {}", names.at(i).data());

                    //selectedBuildingTile.buildingId = std::stoi(k);
                    //selectedBuildingTile.rotation = Rotation::DEG0;

                    result = buildingId;
                }

                ImGui::TreePop();
            }

            i++;
        }

        ImGui::TreePop();
    }

    return (result > 0) ? std::optional<int>(result) : std::nullopt;
}
