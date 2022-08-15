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
#include "Game.h"
#include "MapTile.h"
#include "data/Text.h"

//-------------------------------------------------
// Render
//-------------------------------------------------

void mdcii::map::MapTile::RenderImGui(const bool t_heading) const
{
    if (t_heading)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 255, 0)));
        ImGui::Text("MapTile");
        ImGui::PopStyleColor();

        ImGui::Separator();
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(4, 217, 255)));
    ImGui::Text("Name: %s", data::Text::GetTextForBuildingId(data::Text::Section::WORKSHOPS, buildingId, Game::INI.Get<std::string>("locale", "lang")).c_str());
    ImGui::PopStyleColor();

    ImGui::Text("Building Id: %d", buildingId);
    ImGui::Text("Rotation name: %s", rotation_to_string(rotation));
    ImGui::Text("Rotation value: %d", rotation);
    ImGui::Text("X: %d", x);
    ImGui::Text("Y: %d", y);
    ImGui::Text("Map x: %d", mapX);
    ImGui::Text("Map y: %d", mapY);

    if (connectedMapTiles.empty())
    {
        ImGui::Text("Connected tiles: none");
    }
    else
    {
        if (ImGui::TreeNode("Connected tiles"))
        {
            for (const auto tileIndex : connectedMapTiles)
            {
                ImGui::Text("Connected tile index: %d", tileIndex);
            }

            ImGui::TreePop();
        }
    }

    if (ImGui::TreeNode("Screen positions"))
    {
        auto r{ Rotation::DEG0 };
        for (const auto& position : screenPositions)
        {
            if (ImGui::TreeNode(rotation_to_string(r)))
            {
                ImGui::Text("Screen x: %f", static_cast<double>(position.x));
                ImGui::Text("Screen y: %f", static_cast<double>(position.y));
                ImGui::TreePop();
            }

            ++r;
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Indices"))
    {
        auto r{ Rotation::DEG0 };
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

    if (ImGui::TreeNode("Gfxs"))
    {
        auto r{ Rotation::DEG0 };
        for (const auto gfx : gfxs)
        {
            if (ImGui::TreeNode(rotation_to_string(r)))
            {
                ImGui::Text("Gfx: %d", gfx);
                ImGui::TreePop();
            }

            ++r;
        }

        ImGui::TreePop();
    }

    ImGui::Text("Entity handle: %d", entity);
    ImGui::Text("Debug grid entity handle: %d", gridEntity);
}