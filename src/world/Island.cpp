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
#include "MousePicker.h"
#include "MdciiAssert.h"
#include "MdciiUtils.h"
#include "Game.h"
#include "Rotation.h"
#include "resource/OriginalResourcesManager.h"
#include "state/State.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::Island::Island(const World* t_world, const nlohmann::json& t_json)
    : world{ t_world }
{
    MDCII_LOG_DEBUG("[Island::Island()] Create Island.");

    MDCII_ASSERT(world, "[Island::Island()] Null pointer.")
    MDCII_ASSERT(!t_json.empty(), "[Island::Island()] Invalid Json value.")

    SetIslandData(t_json);
    SetLayerData(t_json);

    InitLayerData();
    InitMixedLayerData();
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
    if (IsWorldPositionInAabb(world->mousePicker->selected))
    {
        return olc::vi2d(
            world->mousePicker->selected.x - startX,
            world->mousePicker->selected.y - startY
        );
    }

    return std::nullopt;
}

//-------------------------------------------------
// Set layer data from Json
//-------------------------------------------------

void mdcii::world::Island::SetIslandData(const nlohmann::json& t_json)
{
    MDCII_LOG_DEBUG("[Island::SetIslandData()] Sets the island's size, position and climate zone.");

    width = t_json.at("width").get<int>();
    height = t_json.at("height").get<int>();
    startX = t_json.at("x").get<int>();
    startY = t_json.at("y").get<int>();

    // todo: magic enum issue?

    /*
    auto z = t_json.at("climate").get<std::string>();
    if (auto zone = magic_enum::enum_cast<ClimateZone>(z); zone.has_value())
    {
        climateZone = zone.value();

        MDCII_LOG_DEBUG("[Island::SetSizeAndPosition()] The zone of the island is {}.", t_json.at("climate").get<std::string>());
    }
    */

    if (const auto z = t_json.at("climate").get<std::string>(); z == "NORTH")
    {
        climateZone = ClimateZone::NORTH;
    }
    else
    {
        climateZone = ClimateZone::SOUTH;
    }

    m_aabb = physics::Aabb(olc::vi2d(startX, startY), olc::vi2d(width, height));

    MDCII_LOG_DEBUG("[Island::SetIslandData()] The size of the island is ({}, {}).", width, height);
    MDCII_LOG_DEBUG("[Island::SetIslandData()] The island starts at position ({}, {}).", startX, startY);
}

void mdcii::world::Island::SetLayerData(const nlohmann::json& t_json)
{
    MDCII_LOG_DEBUG("[Island::SetLayerData()] Sets the island's layer data.");

    // the data for these three island layers is read from a file
    SetLayerDataByType(t_json, LayerType::COAST);
    SetLayerDataByType(t_json, LayerType::TERRAIN);
    SetLayerDataByType(t_json, LayerType::BUILDINGS);

    // this mixed layer will later be filled with the data from the other layers
    layers.at(magic_enum::enum_integer(LayerType::MIXED)) = std::make_unique<Layer>(world, LayerType::MIXED, width, height);
}

void mdcii::world::Island::SetLayerDataByType(const nlohmann::json& t_json, LayerType t_layerType)
{
    MDCII_LOG_DEBUG("[Island::SetLayerDataByType()] Sets the island's layer data of type {}.", magic_enum::enum_name(t_layerType));

    layers.at(magic_enum::enum_integer(t_layerType)) = std::make_unique<Layer>(world, t_layerType, width, height);

    for (auto layerJson = t_json.at("layers").items(); const auto& [k, v] : layerJson)
    {
        for (const auto& [layerNameJson, layerTilesJson] : v.items())
        {
            if (layerNameJson == to_lower_case(std::string(magic_enum::enum_name(t_layerType))))
            {
                GetLayer(t_layerType)->CreateLayerTiles(layerTilesJson);
            }
        }
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

bool mdcii::world::Island::IsWorldPositionInAabb(const olc::vi2d& t_position) const
{
    return physics::Aabb::PointVsAabb(t_position, m_aabb);
}

//-------------------------------------------------
// Init layer data
//-------------------------------------------------

void mdcii::world::Island::InitLayerData()
{
    MDCII_LOG_DEBUG("[Island::InitLayerData()] Initialises the tile data of the COAST, TERRAIN and BUILDINGS layer.");

    InitLayerTiles(GetLayer(LayerType::COAST));
    InitLayerTiles(GetLayer(LayerType::TERRAIN));
    InitLayerTiles(GetLayer(LayerType::BUILDINGS));
}

void mdcii::world::Island::InitMixedLayerData()
{
    MDCII_LOG_DEBUG("[Island::InitMixedLayerData()] Initializing the MIXED layer tile data.");

    GetLayer(LayerType::MIXED)->tiles = GetLayer(LayerType::BUILDINGS)->tiles;

    for (auto h{ 0 }; h < height; ++h)
    {
        for (auto w{ 0 }; w < width; ++w)
        {
            PopulateMixedLayer(w, h);
        }
    }

    GetLayer(LayerType::MIXED)->SortTilesForRendering();
}

void mdcii::world::Island::PopulateMixedLayer(const int t_x, const int t_y)
{
    const auto index{ t_y * width + t_x };

    MDCII_ASSERT(index >= 0 && index < width * height, "[Island::PopulateMixedLayer()] Invalid index.")

    if (ShouldReplaceTile(GetLayer(LayerType::COAST), index))
    {
        GetLayer(LayerType::MIXED)->tiles.at(index) = GetLayer(LayerType::COAST)->tiles.at(index);
    }

    if (ShouldReplaceTile(GetLayer(LayerType::TERRAIN), index))
    {
        GetLayer(LayerType::MIXED)->tiles.at(index) = GetLayer(LayerType::TERRAIN)->tiles.at(index);
    }
}

//-------------------------------------------------
// Layer tiles
//-------------------------------------------------

bool mdcii::world::Island::ShouldReplaceTile(const Layer* t_layer, const int t_index)
{
    return t_layer->tiles.at(t_index).HasBuilding() && !GetLayer(LayerType::BUILDINGS)->tiles.at(t_index).HasBuilding();
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

void mdcii::world::to_json(nlohmann::json& t_json, const Island& t_island)
{
    t_json["width"] = t_island.width;
    t_json["height"] = t_island.height;
    t_json["x"] = t_island.startX;
    t_json["y"] = t_island.startY;
    t_json["climate"] = t_island.climateZone == ClimateZone::NORTH ? "NORTH" : "SOUTH"; // todo: magic enum issue?

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
