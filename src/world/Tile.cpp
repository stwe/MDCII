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

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::world::Tile::Reset()
{
    buildingId = -1;
    rotation = map::Rotation::DEG0;
    x = 0;
    y = 0;
    worldX = -1;
    worldY = -1;
}

//-------------------------------------------------
// Render
//-------------------------------------------------

void mdcii::world::Tile::RenderImGui() const
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 255, 0)));
    ImGui::Text("Tile");
    ImGui::PopStyleColor();

    ImGui::Separator();

    ImGui::Text("Building Id: %d", buildingId);
    ImGui::Text("Rotation name: %s", rotation_to_string(rotation));
    ImGui::Text("Rotation value: %d", rotation);
    ImGui::Text("X: %d", x);
    ImGui::Text("Y: %d", y);
    ImGui::Text("World x: %d", worldX);
    ImGui::Text("World y: %d", worldY);

    if (ImGui::TreeNode("Gfxs"))
    {
        auto r{ map::Rotation::DEG0 };
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
}
