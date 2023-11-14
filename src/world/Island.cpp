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
#include "Island.h"
#include "Game.h"
#include "Layer.h"
#include "MdciiAssert.h"
#include "resource/OriginalResourcesManager.h"
#include "renderer/Renderer.h"
#include "world/Rotation.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::Island::Island()
{
    MDCII_LOG_DEBUG("[Island::Island()] Create Island.");
}

mdcii::world::Island::~Island() noexcept
{
    MDCII_LOG_DEBUG("[Island::~Island()] Destruct Island.");
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::Island::Init(const Game* t_game)
{
    MDCII_LOG_DEBUG("[Island::Init()] Initialize the island.");

    MDCII_ASSERT(t_game, "[Island::Init()] Null pointer.")
    MDCII_ASSERT(width, "[Island::Init()] Invalid width.")
    MDCII_ASSERT(height, "[Island::Init()] Invalid height.")

    for (auto i{ 0 }; i < 3; ++i)
    {
        InitLayerTiles(t_game, layers.at(i).get());
    }

    aabb = physics::Aabb(olc::vi2d(startX, startY), olc::vi2d(width, height));
}

void mdcii::world::Island::InitLayerTiles(const Game* t_game, Layer* t_layer) const
{
    MDCII_LOG_DEBUG("[Island::InitLayerTiles()] Start initializing layer tiles for {} ...", magic_enum::enum_name(t_layer->layerType));

    MDCII_ASSERT(!t_layer->tiles.empty(), "[Island::InitLayerTiles()] Invalid number of tiles.")

    for (auto h{ 0 }; h < height; ++h)
    {
        for (auto w{ 0 }; w < width; ++w)
        {
            auto& tile{ t_layer->tiles.at(h * width + w) };
            tile.posX = w;
            tile.posY = h;

            if (tile.HasBuilding())
            {
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

                for (auto& gfx : tile.gfxs)
                {
                    if (building.size.w > 1 || building.size.h > 1)
                    {
                        // default: orientation 0
                        auto rp{ olc::vi2d(tile.x, tile.y) };

                        if (tile.rotation == magic_enum::enum_integer(Rotation::DEG270))
                        {
                            rp = rotate_position(
                                tile.x, tile.y,
                                building.size.w, building.size.h,
                                world::Rotation::DEG90
                            );
                        }

                        if (tile.rotation == magic_enum::enum_integer(Rotation::DEG180))
                        {
                            rp = rotate_position(
                                tile.x, tile.y,
                                building.size.w, building.size.h,
                                world::Rotation::DEG180
                            );
                        }

                        if (tile.rotation == magic_enum::enum_integer(Rotation::DEG90))
                        {
                            rp = rotate_position(
                                tile.x, tile.y,
                                building.size.w, building.size.h,
                                world::Rotation::DEG270
                            );
                        }

                        const auto offset{ rp.y * building.size.w + rp.x };
                        gfx += offset;
                    }
                }

                // set posoffs
                tile.posoffs = building.posoffs;
            }

            tile.indices[0] = renderer::Renderer::GetMapIndex(tile.posX, tile
                .posY, width, height, world::Rotation::DEG0);
            tile.indices[1] = renderer::Renderer::GetMapIndex(tile.posX, tile
                .posY, width, height, world::Rotation::DEG90);
            tile.indices[2] = renderer::Renderer::GetMapIndex(tile.posX, tile
                .posY, width, height, world::Rotation::DEG180);
            tile.indices[3] = renderer::Renderer::GetMapIndex(tile.posX, tile
                .posY, width, height, world::Rotation::DEG270);
        }
    }

    // sort for rendering
    for (const auto rotation : magic_enum::enum_values<world::Rotation>())
    {
        const auto rotationInt{ magic_enum::enum_integer(rotation) };

        // sort tiles by index
        std::ranges::sort(t_layer->tiles, [&](const world::Tile& t_a, const world::Tile& t_b)
        {
            return t_a.indices[rotationInt] < t_b.indices[rotationInt];
        });

        // copy sorted tiles
        t_layer->sortedTiles.at(rotationInt) = t_layer->tiles;
    }

    // revert tiles sorting = sortedTiles DEG0
    t_layer->tiles = t_layer->sortedTiles.at(magic_enum::enum_integer(world::Rotation::DEG0));

    MDCII_LOG_DEBUG("[Island::InitLayerTiles()] The layer tiles were initialized successfully.");
}
