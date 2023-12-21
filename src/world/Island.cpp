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
#include "World.h"
#include "GameState.h"
#include "MousePicker.h"
#include "MdciiAssert.h"
#include "renderer/Renderer.h"
#include "world/Rotation.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::Island::Island(World* t_world, const int t_width, const int t_height, const int t_x, const int t_y)
    : width{ t_width }
    , height{ t_height }
    , startX{ t_x }
    , startY{ t_y }
    , m_world{ t_world }
    , m_aabb{ physics::Aabb(olc::vi2d(startX, startY), olc::vi2d(width, height)) }
{
    MDCII_LOG_DEBUG("[Island::Island()] Create Island.");

    MDCII_ASSERT(width, "[Island::Island()] Invalid width.")
    MDCII_ASSERT(height, "[Island::Island()] Invalid height.")
    MDCII_ASSERT(m_world, "[Island::Island()] Null pointer.")
}

mdcii::world::Island::~Island() noexcept
{
    MDCII_LOG_DEBUG("[Island::~Island()] Destruct Island.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

mdcii::world::Layer* mdcii::world::Island::GetLayer(const LayerType t_layerType)
{
    return layers[magic_enum::enum_integer(t_layerType)].get();
}

const mdcii::world::Layer* mdcii::world::Island::GetLayer(const LayerType t_layerType) const
{
    return layers[magic_enum::enum_integer(t_layerType)].get();
}

//-------------------------------------------------
// Position
//-------------------------------------------------

bool mdcii::world::Island::IsWorldPositionOverIsland(const olc::vi2d& t_position) const
{
    return IsWorldPositionInAabb(t_position);
}

std::optional<olc::vi2d> mdcii::world::Island::IsMouseOverIsland() const
{
    if (IsWorldPositionInAabb(m_world->gameState->mousePicker->selected))
    {
        return olc::vi2d(
            m_world->gameState->mousePicker->selected.x - startX,
            m_world->gameState->mousePicker->selected.y - startY
        );
    }

    return std::nullopt;
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::Island::Init()
{
    MDCII_LOG_DEBUG("[Island::Init()] Initializes the island.");

    InitLayers();
    InitMixedLayer();
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

bool mdcii::world::Island::IsWorldPositionInAabb(const olc::vi2d& t_position) const
{
    return physics::Aabb::PointVsAabb(t_position, m_aabb);
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

    mixedLayer->SortTilesForRendering();
}

void mdcii::world::Island::PopulateMixedLayer(
    const Layer* t_coastLayer,
    const Layer* t_terrainLayer,
    const Layer* t_buildingsLayer,
    Layer* t_mixedLayer,
    const int t_x,
    const int t_y
) const
{
    const auto index{ t_y * width + t_x };

    if (ShouldReplaceTile(t_coastLayer, t_buildingsLayer, index))
    {
        t_mixedLayer->tiles.at(index) = t_coastLayer->tiles.at(index);
    }

    if (ShouldReplaceTile(t_terrainLayer, t_buildingsLayer, index))
    {
        t_mixedLayer->tiles.at(index) = t_terrainLayer->tiles.at(index);
    }
}

//-------------------------------------------------
// Layer tiles
//-------------------------------------------------

bool mdcii::world::Island::ShouldReplaceTile(const Layer* t_layer, const Layer* t_buildingsLayer, int t_index)
{
    return t_layer->tiles.at(t_index).HasBuilding() && !t_buildingsLayer->tiles.at(t_index).HasBuilding();
}

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

    t_layer->SortTilesForRendering();

    MDCII_LOG_DEBUG("[Island::InitLayerTiles()] The layer tiles were initialized successfully.");
}

void mdcii::world::Island::InitTileDetails(Layer* t_layer, const int t_x, const int t_y) const
{
    auto& tile{ t_layer->tiles.at(t_y * width + t_x) };
    tile.posX = t_x;
    tile.posY = t_y;

    if (tile.HasBuilding())
    {
        tile.CalculateGfx();
    }

    tile.indices[0] = t_layer->GetMapIndex(tile.posX, tile.posY, Rotation::DEG0);
    tile.indices[1] = t_layer->GetMapIndex(tile.posX, tile.posY, Rotation::DEG90);
    tile.indices[2] = t_layer->GetMapIndex(tile.posX, tile.posY, Rotation::DEG180);
    tile.indices[3] = t_layer->GetMapIndex(tile.posX, tile.posY, Rotation::DEG270);
}

//-------------------------------------------------
// Serializing Island into Json
//-------------------------------------------------

/*
void mdcii::world::to_json(nlohmann::json& t_json, const mdcii::world::Island& t_island)
{
    t_json["width"] = t_island.width;
    t_json["height"] = t_island.height;
    t_json["x"] = t_island.startX;
    t_json["y"] = t_island.startY;
    t_json["climate"] = std::string(magic_enum::enum_name(t_island.climateZone));

    // layers
    t_json["layers"] = nlohmann::json::array();

    auto c = nlohmann::json::object();
    c["coast"] = t_island.layers.at(0)->tiles;

    auto t = nlohmann::json::object();
    t["terrain"] = t_island.layers.at(1)->tiles;

    auto b = nlohmann::json::object();
    b["buildings"] = t_island.layers.at(2)->tiles;

    t_json["layers"].push_back(c);
    t_json["layers"].push_back(t);
    t_json["layers"].push_back(b);
}
*/
