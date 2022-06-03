#pragma once

#include "data/TileAsset.h"
#include "file/BshFile.h"

//-------------------------------------------------
// ImGuiTileRenderer
//-------------------------------------------------

namespace mdcii::renderer
{
    /**
     * Static methods for rendering a tile for ImGui menus.
     */
    class ImGuiTileRenderer
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        ImGuiTileRenderer(const ImGuiTileRenderer& t_other) = delete;
        ImGuiTileRenderer(ImGuiTileRenderer&& t_other) noexcept = delete;
        ImGuiTileRenderer& operator=(const ImGuiTileRenderer& t_other) = delete;
        ImGuiTileRenderer& operator=(ImGuiTileRenderer&& t_other) noexcept = delete;

        //-------------------------------------------------
        // Render
        //-------------------------------------------------

        /**
         * Renders a single textured tile.
         *
         * @param t_tileAsset A tile asset. The gfx value is used for rendering.
         * @param t_bshFile A BshFile object holding multiple textures.
         */
        static void RenderTileGfxImGui(
            const data::TileAsset& t_tileAsset,
            const file::BshFile& t_bshFile
        );

        /**
         * Renders a single textured tile for preview.
         *
         * @param t_tileAsset A tile asset. The baugfx value is used for rendering.
         * @param t_bshFile A BshFile object holding multiple textures for preview.
         */
        static void RenderTileBauGfxImGui(
            const data::TileAsset& t_tileAsset,
            const file::BshFile& t_bshFile
        );

    protected:


    private:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        ImGuiTileRenderer() = default;
        ~ImGuiTileRenderer() noexcept = default;
    };
}
