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
#include "ImGuiTileRenderer.h"
#include "MdciiAssert.h"

void mdcii::renderer::ImGuiTileRenderer::RenderTileGfxImGui(
    const data::Building& t_building,
    const file::BshFile& t_bshFile
)
{
    if (const auto gfx{ t_building.gfx }; gfx >= 0)
    {
        MDCII_ASSERT(static_cast<size_t>(gfx) < t_bshFile.bshTextures.size(), "[ImGuiTileRenderer::RenderTileGfxImGui()] Invalid arguments given.")

        ImGui::Text("Gfx: %d", gfx);

        const auto textureWidth{ t_bshFile.bshTextures.at(gfx)->width };
        MDCII_ASSERT(textureWidth, "[ImGuiTileRenderer::RenderTileGfxImGui()] Invalid texture width.")

        const auto textureHeight{ t_bshFile.bshTextures.at(gfx)->height };
        MDCII_ASSERT(textureHeight, "[ImGuiTileRenderer::RenderTileGfxImGui()] Invalid texture height.")

        const auto textureId{ reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(t_bshFile.bshTextures.at(gfx)->textureId)) };

        ImGui::Image(
            textureId,
            ImVec2(static_cast<float>(textureWidth), static_cast<float>(textureHeight)),
            ImVec2(0.0f, 0.0f),
            ImVec2(1.0f, 1.0f),
            ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
            ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
        );
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
        ImGui::Text("Invalid Gfx: %d", gfx);
        ImGui::PopStyleColor();
    }
}

void mdcii::renderer::ImGuiTileRenderer::RenderTileBauGfxImGui(
    const data::Building& t_building,
    const file::BshFile& t_bshFile
)
{
    if (const auto gfx{ t_building.gfx }; gfx >= 0)
    {
        if (const auto baugfx{ t_building.baugfx }; baugfx >= 0)
        {
            MDCII_ASSERT(static_cast<size_t>(baugfx) < t_bshFile.bshTextures.size(), "[ImGuiTileRenderer::RenderTileBauGfxImGui()] Invalid arguments given.")

            //ImGui::Text("BauGfx: %d", baugfx);

            const auto textureWidth{ t_bshFile.bshTextures.at(baugfx)->width };
            MDCII_ASSERT(textureWidth, "[ImGuiTileRenderer::RenderTileBauGfxImGui()] Invalid texture width.")

            const auto textureHeight{ t_bshFile.bshTextures.at(baugfx)->height };
            MDCII_ASSERT(textureHeight, "[ImGuiTileRenderer::RenderTileBauGfxImGui()] Invalid texture height.")

            const auto textureId{ reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(t_bshFile.bshTextures.at(baugfx)->textureId)) };

            ImGui::Image(
                textureId,
                ImVec2(static_cast<float>(textureWidth), static_cast<float>(textureHeight)),
                ImVec2(0.0f, 0.0f),
                ImVec2(1.0f, 1.0f),
                ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
            );
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
            ImGui::Text("Invalid BauGfx: %d", baugfx);
            ImGui::PopStyleColor();
        }
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
        ImGui::Text("Invalid Gfx: %d", gfx);
        ImGui::PopStyleColor();
    }
}
