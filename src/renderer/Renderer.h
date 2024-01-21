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

#pragma once

#include "vendor/olc/olcPixelGameEngine.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * @brief Forward declaration class World.
     */
    class World;
}

namespace mdcii::world::tile
{
    /**
     * @brief Forward declaration struct TerrainTile.
     */
    struct TerrainTile;
}

namespace mdcii::resource
{
    /**
     * @brief Forward declaration enum class Asset.
     */
    enum class Asset;
}

namespace mdcii::renderer
{
    //-------------------------------------------------
    // Renderer
    //-------------------------------------------------

    // todo: -> AssetManager

    /**
     * @brief Represents a Renderer.
     */
    class Renderer
    {
    public:
        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        static void RenderAsset(
            resource::Asset t_asset,
            int t_startX,
            int t_startY,
            const world::World* t_world,
            const world::tile::TerrainTile* t_tile,
            bool t_minusElevation,
            const olc::Pixel& t_tint = olc::WHITE
        );

    protected:

    private:

    };
}
