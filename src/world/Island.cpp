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

#include "Island.h"
#include "Layer.h"
#include "MdciiAssert.h"
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

    mixedLayer->SortTiles();
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

    t_layer->SortTiles();

    MDCII_LOG_DEBUG("[Island::InitLayerTiles()] The layer tiles were initialized successfully.");
}

void mdcii::world::Island::InitTileDetails(Layer* t_layer, const int t_w, const int t_h) const
{
    auto& tile{ t_layer->tiles.at(t_h * width + t_w) };
    tile.posX = t_w;
    tile.posY = t_h;

    if (tile.HasBuilding())
    {
        tile.CalculateGfx();
    }

    tile.indices[0] = renderer::Renderer::GetMapIndex(tile.posX, tile.posY, width, height, Rotation::DEG0);
    tile.indices[1] = renderer::Renderer::GetMapIndex(tile.posX, tile.posY, width, height, Rotation::DEG90);
    tile.indices[2] = renderer::Renderer::GetMapIndex(tile.posX, tile.posY, width, height, Rotation::DEG180);
    tile.indices[3] = renderer::Renderer::GetMapIndex(tile.posX, tile.posY, width, height, Rotation::DEG270);
}
