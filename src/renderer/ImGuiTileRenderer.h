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

#pragma once

#include "data/Buildings.h"
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
         * @param t_building A tile building. The gfx value is used for rendering.
         * @param t_bshFile A BshFile object holding multiple textures.
         */
        static void RenderTileGfxImGui(
            const data::Building& t_building,
            const file::BshFile& t_bshFile
        );

        /**
         * Renders a single textured tile for preview.
         *
         * @param t_building A tile building. The baugfx value is used for rendering.
         * @param t_bshFile A BshFile object holding multiple textures for preview.
         */
        static void RenderTileBauGfxImGui(
            const data::Building& t_building,
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
