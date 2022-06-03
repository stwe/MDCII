#include <imgui.h>
#include "ImGuiTileRenderer.h"
#include "MdciiAssert.h"

void mdcii::renderer::ImGuiTileRenderer::RenderTileGfxImGui(
    const data::TileAsset& t_tileAssetProperties,
    const file::BshFile& t_bshFile
)
{
    if (const auto gfx{ t_tileAssetProperties.gfx }; gfx >= 0)
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

void mdcii::renderer::ImGuiTileRenderer::RenderTileBauGfxImGui(const data::TileAsset& t_tileAssetProperties, const file::BshFile& t_bshFile)
{
    if (const auto gfx{ t_tileAssetProperties.gfx }; gfx >= 0)
    {
        if (const auto baugfx{ t_tileAssetProperties.baugfx }; baugfx >= 0)
        {
            MDCII_ASSERT(static_cast<size_t>(baugfx) < t_bshFile.bshTextures.size(), "[ImGuiTileRenderer::RenderTileBauGfxImGui()] Invalid arguments given.")

            ImGui::Text("BauGfx: %d", baugfx);

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
