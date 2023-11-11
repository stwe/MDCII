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

#include <algorithm>
#include "DeepWater.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "resource/OriginalResourcesManager.h"
#include "renderer/Renderer.h"
#include "world/Rotation.h"

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::DeepWater::InitTiles(const Game* t_game)
{
    MDCII_ASSERT(t_game, "[DeepWater::InitTiles()] Null pointer.")
    MDCII_ASSERT(!tiles.empty(), "[DeepWater::InitTiles()] Invalid number of tiles.")
    MDCII_ASSERT(width, "[DeepWater::InitTiles()] Invalid width.")
    MDCII_ASSERT(height, "[DeepWater::InitTiles()] Invalid height.")

    // set world positions
    for (auto h{ 0 }; h < height; ++h)
    {
        for (auto w{ 0 }; w < width; ++w)
        {
            auto& tile{ tiles.at(h * width + w) };
            tile.posX = w;
            tile.posY = h;
        }
    }

    // remove tiles that don't have the buildingId 1201
    auto [begin, end]{ std::ranges::remove_if(tiles, [](const auto& t_tile) { return t_tile.buildingId != 1201; }) };
    tiles.erase(begin, end);

    for (auto& tile : tiles)
    {
        MDCII_ASSERT(tile.HasBuilding(), "[DeepWater::InitTiles()] Invalid building Id")

        // pre-calculate a gfx for each rotation
        const auto& building{ t_game->originalResourcesManager->GetBuildingById(tile.buildingId) };
        const auto gfx0{ building.gfx };

        tile.gfxs.push_back(gfx0);
        if (building.rotate > 0)
        {
            tile.gfxs.push_back(gfx0 + (1 * building.rotate));
            tile.gfxs.push_back(gfx0 + (2 * building.rotate));
            tile.gfxs.push_back(gfx0 + (3 * building.rotate));
        }

        // set posoffs
        tile.posoffs = building.posoffs;

        tile.indices[0] = renderer::Renderer::GetMapIndex(tile.posX, tile
            .posY, width, height, world::Rotation::DEG0);
        tile.indices[1] = renderer::Renderer::GetMapIndex(tile.posX, tile
            .posY, width, height, world::Rotation::DEG90);
        tile.indices[2] = renderer::Renderer::GetMapIndex(tile.posX, tile
            .posY, width, height, world::Rotation::DEG180);
        tile.indices[3] = renderer::Renderer::GetMapIndex(tile.posX, tile
            .posY, width, height, world::Rotation::DEG270);
    }

    // sort for rendering
    for (const auto rotation : magic_enum::enum_values<world::Rotation>())
    {
        const auto rotationInt{ magic_enum::enum_integer(rotation) };

        // sort tiles by index
        std::ranges::sort(tiles, [&](const world::Tile& t_a, const world::Tile& t_b)
        {
            return t_a.indices[rotationInt] < t_b.indices[rotationInt];
        });

        // copy sorted tiles
        sortedTiles.at(rotationInt) = tiles;
    }

    // revert tiles sorting = sortedTiles DEG0
    tiles = sortedTiles.at(magic_enum::enum_integer(world::Rotation::DEG0));
}
