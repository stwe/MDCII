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
            InitTileDetails(t_game, t_layer, w, h);
        }
    }

    SortTiles(t_layer);

    // revert tiles sorting = sortedTiles DEG0
    t_layer->tiles = t_layer->sortedTiles.at(magic_enum::enum_integer(Rotation::DEG0));

    MDCII_LOG_DEBUG("[Island::InitLayerTiles()] The layer tiles were initialized successfully.");
}

//-------------------------------------------------
// Init layer tiles
//-------------------------------------------------

void mdcii::world::Island::InitTileDetails(const Game* t_game, Layer* t_layer, const int t_w, const int t_h) const
{
    auto& tile{ t_layer->tiles.at(t_h * width + t_w) };
    tile.posX = t_w;
    tile.posY = t_h;

    if (tile.HasBuilding())
    {
        InitBuildingTileDetails(t_game, &tile);
    }

    tile.indices[0] = renderer::Renderer::GetMapIndex(tile.posX, tile
        .posY, width, height, Rotation::DEG0);
    tile.indices[1] = renderer::Renderer::GetMapIndex(tile.posX, tile
        .posY, width, height, Rotation::DEG90);
    tile.indices[2] = renderer::Renderer::GetMapIndex(tile.posX, tile
        .posY, width, height, Rotation::DEG180);
    tile.indices[3] = renderer::Renderer::GetMapIndex(tile.posX, tile
        .posY, width, height, Rotation::DEG270);
}

void mdcii::world::Island::InitBuildingTileDetails(const Game* t_game, Tile* t_tile)
{
    // pre-calculate a gfx for each rotation
    const auto& building{ t_game->originalResourcesManager->GetBuildingById(t_tile->buildingId) };
    const auto gfx0{ building.gfx };

    t_tile->gfxs.push_back(gfx0);
    if (building.rotate > 0)
    {
        t_tile->gfxs.push_back(gfx0 + (1 * building.rotate));
        t_tile->gfxs.push_back(gfx0 + (2 * building.rotate));
        t_tile->gfxs.push_back(gfx0 + (3 * building.rotate));
    }

    for (auto& gfx : t_tile->gfxs)
    {
        if (building.size.w > 1 || building.size.h > 1)
        {
            AdjustGfxForBigBuildings(&building, t_tile, gfx);
        }
    }

    // set posoffs
    t_tile->posoffs = building.posoffs;
}

void mdcii::world::Island::AdjustGfxForBigBuildings(const resource::Building* t_building, const Tile* t_tile, int& t_gfx)
{
    // default: orientation 0
    auto rp{ olc::vi2d(t_tile->x, t_tile->y) };

    if (t_tile->rotation == magic_enum::enum_integer(Rotation::DEG270))
    {
        rp = rotate_position(
            t_tile->x, t_tile->y,
            t_building->size.w, t_building->size.h,
            Rotation::DEG90
        );
    }

    if (t_tile->rotation == magic_enum::enum_integer(Rotation::DEG180))
    {
        rp = rotate_position(
            t_tile->x, t_tile->y,
            t_building->size.w, t_building->size.h,
            Rotation::DEG180
        );
    }

    if (t_tile->rotation == magic_enum::enum_integer(Rotation::DEG90))
    {
        rp = rotate_position(
            t_tile->x, t_tile->y,
            t_building->size.w, t_building->size.h,
            Rotation::DEG270
        );
    }

    const auto offset{ rp.y * t_building->size.w + rp.x };
    t_gfx += offset;
}

void mdcii::world::Island::SortTiles(Layer* t_layer)
{
    for (const auto rotation : magic_enum::enum_values<Rotation>())
    {
        const auto rotationInt{ magic_enum::enum_integer(rotation) };

        // sort tiles by index
        std::ranges::sort(t_layer->tiles, [&](const Tile& t_a, const Tile& t_b)
        {
            return t_a.indices[rotationInt] < t_b.indices[rotationInt];
        });

        // copy sorted tiles
        t_layer->sortedTiles.at(rotationInt) = t_layer->tiles;
    }
}
