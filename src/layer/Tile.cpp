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
#include "Tile.h"
#include "Game.h"
#include "data/Text.h"
#include "GameLayer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::layer::Tile::Tile()
{
    layerType = LayerType::NOTHING;
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::layer::Tile::Reset()
{
    ResetBuildingInfo();

    islandXDeg0 = -1;
    islandYDeg0 = -1;
    worldXDeg0 = -1;
    worldYDeg0 = -1;
    screenPositions = {};
    indices = {};
    instanceIds = {};
}

void mdcii::layer::Tile::ResetBuildingInfo()
{
    buildingId = -1;
    rotation = world::Rotation::DEG0;
    x = 0;
    y = 0;
    gfxs = {};
    connectedTiles = {};
}

//-------------------------------------------------
// Render
//-------------------------------------------------

void mdcii::layer::Tile::RenderImGui() const
{
    magic_enum::enum_for_each< data::Section>([this] (const auto t_section) {
        constexpr data::Section section = t_section;
        const auto nameOptional{ data::Text::GetTextForBuildingId(
            section,
            buildingId, Game::INI.Get<std::string>("locale", "lang"))
        };
        if (nameOptional.has_value())
        {
            ImGui::Text("Building Name: %s", nameOptional.value().c_str());
        }
    });

    ImGui::Text("Tile Building-Id: %d", buildingId);
    ImGui::Text("Tile rotation: %s", magic_enum::enum_name(rotation).data());
    ImGui::Text("Layer type: %s", magic_enum::enum_name(layerType).data());
    ImGui::Text("Building x: %d", x);
    ImGui::Text("Building y: %d", y);
    ImGui::Text("Island x: %d", islandXDeg0);
    ImGui::Text("Island y: %d", islandYDeg0);
    ImGui::Text("World x: %d", worldXDeg0);
    ImGui::Text("World y: %d", worldYDeg0);

    if (ImGui::TreeNode("Indices"))
    {
        auto r{ world::Rotation::DEG0 };
        for (const auto index : indices)
        {
            if (ImGui::TreeNode(rotation_to_string(r)))
            {
                ImGui::Text("Index: %d", index);
                ImGui::TreePop();
            }

            ++r;
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Instances Ids"))
    {
        auto r{ world::Rotation::DEG0 };
        for (const auto id : instanceIds)
        {
            if (ImGui::TreeNode(rotation_to_string(r)))
            {
                ImGui::Text("Instance Id: %d", id);
                ImGui::TreePop();
            }

            ++r;
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Gfxs"))
    {
        auto r{ world::Rotation::DEG0 };
        for (const auto gfx : gfxs)
        {
            if (ImGui::TreeNode(magic_enum::enum_name(r).data()))
            {
                ImGui::Text("Gfx: %d", gfx);
                ImGui::TreePop();
            }

            ++r;
        }

        ImGui::TreePop();
    }

    if (connectedTiles.empty())
    {
        ImGui::Text("Connected tiles: none");
    }
    else
    {
        if (ImGui::TreeNode("Connected tiles"))
        {
            for (const auto tileIndex : connectedTiles)
            {
                ImGui::Text("Connected tile index: %d", tileIndex);
            }

            ImGui::TreePop();
        }
    }
}
