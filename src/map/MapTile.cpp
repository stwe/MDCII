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
#include <magic_enum.hpp>
#include "Game.h"
#include "MapLayer.h"
#include "data/Text.h"

//-------------------------------------------------
// Render
//-------------------------------------------------

void mdcii::map::MapTile::RenderImGui() const
{
    ImGui::Separator();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 255, 0)));
    ImGui::Text("MapTile");
    ImGui::PopStyleColor();

    ImGui::Separator();

    ImGui::Text("Name: %s", data::Text::GetTextForBuildingId(data::Text::Section::WORKSHOPS, buildingId, Game::INI.Get<std::string>("locale", "lang")).c_str());

    ImGui::Text("Building Id: %d", buildingId);
    ImGui::Text("Orientation name: %s", ShowCurrentOrientation());
    ImGui::Text("Orientation value: %d", orientation);
    ImGui::Text("X: %d", x);
    ImGui::Text("Y: %d", y);
    ImGui::Text("Map x: %d", mapX);
    ImGui::Text("Map y: %d", mapY);

    if (ImGui::TreeNode("Screen positions"))
    {
        auto r{ Rotation::DEG0 };
        for (const auto& position : screenPositions)
        {
            if (ImGui::TreeNode(magic_enum::enum_name(r).data()))
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
            if (ImGui::TreeNode(magic_enum::enum_name(r).data()))
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
            if (ImGui::TreeNode(magic_enum::enum_name(r).data()))
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

//-------------------------------------------------
// Orientation
//-------------------------------------------------

const char* mdcii::map::MapTile::ShowCurrentOrientation() const
{
    const auto rotation{ magic_enum::enum_cast<Rotation>(orientation) };
    if (rotation.has_value())
    {
        return magic_enum::enum_name(rotation.value()).data();
    }

    return nullptr;
}
