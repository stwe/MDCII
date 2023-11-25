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
#include "Layer.h"
#include "MdciiAssert.h"
#include "renderer/Renderer.h"
#include "world/Rotation.h"
#include "resource/Buildings.h"

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

void mdcii::world::Island::Init()
{
    MDCII_LOG_DEBUG("[Island::Init()] Initializes the island.");

    MDCII_ASSERT(width, "[Island::Init()] Invalid width.")
    MDCII_ASSERT(height, "[Island::Init()] Invalid height.")

    InitLayers();
    InitMixedLayer();

    aabb = physics::Aabb(olc::vi2d(startX, startY), olc::vi2d(width, height));
}

//-------------------------------------------------
// Layer
//-------------------------------------------------

void mdcii::world::Island::InitLayers()
{
    MDCII_LOG_DEBUG("[Island::InitLayers()] Initializing the COAST, TERRAIN and BUILDINGS layer of the island.");

    for (auto i{ 0 }; i < NR_OF_LAYERS - 1; ++i)
    {
        InitLayerTiles(layers.at(i).get());
    }
}

void mdcii::world::Island::InitMixedLayer()
{
    MDCII_LOG_DEBUG("[Island::InitMixedLayer()] Initializing the MIXED layer of the island.");

    const auto* buildingsLayer{ layers.at(magic_enum::enum_integer(LayerType::BUILDINGS)).get() };
    const auto* terrainLayer{ layers.at(magic_enum::enum_integer(LayerType::TERRAIN)).get() };
    const auto* coastLayer{ layers.at(magic_enum::enum_integer(LayerType::COAST)).get() };
    auto* mixedLayer{ layers.at(magic_enum::enum_integer(LayerType::MIXED)).get() };
    mixedLayer->tiles = buildingsLayer->tiles;

    for (auto h{ 0 }; h < height; ++h)
    {
        for (auto w{ 0 }; w < width; ++w)
        {
            PopulateMixedLayer(coastLayer, terrainLayer, buildingsLayer, mixedLayer, w, h);
        }
    }

    SortTiles(mixedLayer);
}

void mdcii::world::Island::PopulateMixedLayer(
    const Layer* t_coastLayer,
    const Layer* t_terrainLayer,
    const Layer* t_buildingsLayer,
    Layer* t_mixedLayer,
    const int t_w,
    const int t_h
) const
{
    const auto index{ t_h * width + t_w };

    if (ShouldReplaceTile(t_coastLayer, t_buildingsLayer, index))
    {
        t_mixedLayer->tiles.at(index) = t_coastLayer->tiles.at(index);
    }

    if (ShouldReplaceTile(t_terrainLayer, t_buildingsLayer, index))
    {
        t_mixedLayer->tiles.at(index) = t_terrainLayer->tiles.at(index);
    }
}

bool mdcii::world::Island::ShouldReplaceTile(const Layer* t_layer, const Layer* t_buildingsLayer, int t_index)
{
    return t_layer->tiles.at(t_index).HasBuilding() && !t_buildingsLayer->tiles.at(t_index).HasBuilding();
}

//-------------------------------------------------
// Layer tiles
//-------------------------------------------------

void mdcii::world::Island::InitLayerTiles(Layer* t_layer) const
{
    MDCII_LOG_DEBUG("[Island::InitLayerTiles()] Start initializing layer tiles for {} ...", magic_enum::enum_name(t_layer->layerType));

    MDCII_ASSERT(!t_layer->tiles.empty(), "[Island::InitLayerTiles()] Invalid number of tiles.")

    for (auto h{ 0 }; h < height; ++h)
    {
        for (auto w{ 0 }; w < width; ++w)
        {
            InitTileDetails(t_layer, w, h);
        }
    }

    SortTiles(t_layer);

    MDCII_LOG_DEBUG("[Island::InitLayerTiles()] The layer tiles were initialized successfully.");
}

void mdcii::world::Island::InitTileDetails(Layer* t_layer, const int t_w, const int t_h) const
{
    auto& tile{ t_layer->tiles.at(t_h * width + t_w) };
    tile.posX = t_w;
    tile.posY = t_h;

    if (tile.HasBuilding())
    {
        InitBuildingTileDetails(&tile);
    }

    tile.indices[0] = renderer::Renderer::GetMapIndex(tile.posX, tile.posY, width, height, Rotation::DEG0);
    tile.indices[1] = renderer::Renderer::GetMapIndex(tile.posX, tile.posY, width, height, Rotation::DEG90);
    tile.indices[2] = renderer::Renderer::GetMapIndex(tile.posX, tile.posY, width, height, Rotation::DEG180);
    tile.indices[3] = renderer::Renderer::GetMapIndex(tile.posX, tile.posY, width, height, Rotation::DEG270);
}

void mdcii::world::Island::InitBuildingTileDetails(Tile* t_tile) const
{
    MDCII_ASSERT(t_tile->HasBuilding(), "[Island::InitBuildingTileDetails()] Null pointer.")

    // pre-calculate a gfx for each rotation
    const auto gfx0{ t_tile->building->gfx };

    t_tile->gfxs.push_back(gfx0);
    if (t_tile->building->rotate > 0)
    {
        t_tile->gfxs.push_back(gfx0 + (1 * t_tile->building->rotate));
        t_tile->gfxs.push_back(gfx0 + (2 * t_tile->building->rotate));
        t_tile->gfxs.push_back(gfx0 + (3 * t_tile->building->rotate));
    }

    for (auto& gfx : t_tile->gfxs)
    {
        if (t_tile->building->size.w > 1 || t_tile->building->size.h > 1)
        {
            AdjustGfxForBigBuildings(t_tile, gfx);
        }
    }
}

void mdcii::world::Island::AdjustGfxForBigBuildings(const Tile* t_tile, int& t_gfx) const
{
    MDCII_ASSERT(t_tile->HasBuilding(), "[Island::AdjustGfxForBigBuildings()] Null pointer.")

    // default: orientation 0
    auto rp{ olc::vi2d(t_tile->x, t_tile->y) };

    if (t_tile->rotation == magic_enum::enum_integer(Rotation::DEG270))
    {
        rp = rotate_position(
            t_tile->x, t_tile->y,
            t_tile->building->size.w, t_tile->building->size.h,
            Rotation::DEG90
        );
    }

    if (t_tile->rotation == magic_enum::enum_integer(Rotation::DEG180))
    {
        rp = rotate_position(
            t_tile->x, t_tile->y,
            t_tile->building->size.w, t_tile->building->size.h,
            Rotation::DEG180
        );
    }

    if (t_tile->rotation == magic_enum::enum_integer(Rotation::DEG90))
    {
        rp = rotate_position(
            t_tile->x, t_tile->y,
            t_tile->building->size.w, t_tile->building->size.h,
            Rotation::DEG270
        );
    }

    const auto offset{ rp.y * t_tile->building->size.w + rp.x };
    t_gfx += offset;
}

void mdcii::world::Island::SortTiles(Layer* t_layer) const
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
