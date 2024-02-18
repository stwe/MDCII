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

#include "Island.h"
#include "MousePicker.h"
#include "MdciiUtils.h"
#include "renderer/Renderer.h"
#include "resource/AssetManager.h"

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

mdcii::world::layer::TerrainLayer<mdcii::world::tile::TerrainTile>* mdcii::world::Island::GetTerrainLayer(const layer::LayerType t_layerType)
{
    MDCII_ASSERT(m_terrainLayers.count(t_layerType), "[Island::GetTerrainLayer()] Missing layer.")

    return m_terrainLayers[t_layerType].get();
}

const mdcii::world::layer::TerrainLayer<mdcii::world::tile::TerrainTile>* mdcii::world::Island::GetTerrainLayer(const layer::LayerType t_layerType) const
{
    const auto it{ m_terrainLayers.find(t_layerType) };

    MDCII_ASSERT(it != m_terrainLayers.end(), "[Island::GetTerrainLayer()] Missing layer.")

    return it->second.get();
}

mdcii::world::layer::FiguresLayer<mdcii::world::tile::FigureTile>* mdcii::world::Island::GetFiguresLayer()
{
    return m_figuresLayer.get();
}

const mdcii::world::layer::FiguresLayer<mdcii::world::tile::FigureTile>* mdcii::world::Island::GetFiguresLayer() const
{
    return m_figuresLayer.get();
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
// Add building
//-------------------------------------------------

std::optional<std::vector<mdcii::world::tile::TerrainTile>> mdcii::world::Island::AddBuilding(
    const resource::Building* t_building,
    const Rotation t_rotation,
    const olc::vi2d& t_mouseOverIsland,
    const int t_xOffset, const int t_yOffset
)
{
    std::vector<tile::TerrainTile> newTiles;

    for (auto y{ 0 }; y < t_building->size.h; ++y)
    {
        for (auto x{ 0 }; x < t_building->size.w; ++x)
        {
            const auto buildingRotation{ t_rotation - world->camera->rotation };

            auto rp{ world::rotate_position(x, y, t_building->size.w, t_building->size.h, buildingRotation) };
            if (buildingRotation == Rotation::DEG90 || buildingRotation == Rotation::DEG270)
            {
                rp = world::rotate_position(x, y,t_building->size.h, t_building->size.w, buildingRotation);
            }

            auto posX{ t_mouseOverIsland.x + rp.x };
            auto posY{ t_mouseOverIsland.y + rp.y };

            if (IsWorldPositionOverIsland({ startX + posX, startY + posY }))
            {
                const auto &terrainTileToCheck{ GetTerrainLayer(layer::LayerType::TERRAIN)->GetSortedTile(posX, posY, world->camera->rotation) };
                const auto &buildingTileToCheck{ GetTerrainLayer(layer::LayerType::BUILDINGS)->GetSortedTile(posX, posY, world->camera->rotation) };

                if (terrainTileToCheck.HasBuildingAboveWaterAndCoast() && !buildingTileToCheck.HasBuilding())
                {
                    newTiles.emplace_back(
                        t_building,
                        magic_enum::enum_integer(buildingRotation),
                        rp.x, rp.y,
                        posX, posY
                    );
                }
            }
        }
    }

    if (newTiles.size() == t_building->size.h * static_cast<std::size_t>(t_building->size.w))
    {
        for (const auto& tile : newTiles)
        {
            renderer::Renderer::RenderAsset(resource::Asset::GREEN_ISO, startX, startY, world, &tile, true);
        }

        return newTiles;
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
    SetLayerDataByType(t_json, layer::LayerType::COAST);
    SetLayerDataByType(t_json, layer::LayerType::TERRAIN);
    SetLayerDataByType(t_json, layer::LayerType::BUILDINGS);

    // todo: refactor
    // figures layer
    m_figuresLayer = std::make_unique<layer::FiguresLayer<tile::FigureTile>>(world, width, height);
    for (auto layerJson = t_json.at("layers").items(); const auto& [k, v] : layerJson)
    {
        for (const auto& [layerNameJson, layerTilesJson] : v.items())
        {
            if (layerNameJson == to_lower_case(std::string(magic_enum::enum_name(layer::LayerType::FIGURES))))
            {
                m_figuresLayer->CreateTiles(layerTilesJson);
            }
        }
    }

    // this mixed layer will later be filled with the data from the other layers
    m_terrainLayers.emplace(layer::LayerType::MIXED, std::make_unique<layer::TerrainLayer<tile::TerrainTile>>(world, layer::LayerType::MIXED, width, height));
}

void mdcii::world::Island::SetLayerDataByType(const nlohmann::json& t_json, const layer::LayerType t_layerType)
{
    MDCII_LOG_DEBUG("[Island::SetLayerDataByType()] Sets the island's layer data of type {}.", magic_enum::enum_name(t_layerType));

    m_terrainLayers.emplace(t_layerType, std::make_unique<layer::TerrainLayer<tile::TerrainTile>>(world, t_layerType, width, height));

    for (auto layerJson = t_json.at("layers").items(); const auto& [k, v] : layerJson)
    {
        for (const auto& [layerNameJson, layerTilesJson] : v.items())
        {
            if (layerNameJson == to_lower_case(std::string(magic_enum::enum_name(t_layerType))))
            {
                GetTerrainLayer(t_layerType)->CreateTiles(layerTilesJson);
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

    GetTerrainLayer(layer::LayerType::COAST)->InitTiles();
    GetTerrainLayer(layer::LayerType::TERRAIN)->InitTiles();
    GetTerrainLayer(layer::LayerType::BUILDINGS)->InitTiles();

    m_figuresLayer->InitTiles();
}

void mdcii::world::Island::InitMixedLayerData()
{
    MDCII_LOG_DEBUG("[Island::InitMixedLayerData()] Initializing the MIXED layer tile data.");

    GetTerrainLayer(layer::LayerType::MIXED)->tiles = GetTerrainLayer(layer::LayerType::BUILDINGS)->tiles;

    for (auto h{ 0 }; h < height; ++h)
    {
        for (auto w{ 0 }; w < width; ++w)
        {
            PopulateMixedLayer(w, h);
        }
    }

    GetTerrainLayer(layer::LayerType::MIXED)->SortTilesForRendering();
}

void mdcii::world::Island::PopulateMixedLayer(const int t_x, const int t_y)
{
    const auto index{ t_y * width + t_x };

    MDCII_ASSERT(index >= 0 && index < width * height, "[Island::PopulateMixedLayer()] Invalid index.")

    if (ShouldReplaceTile(GetTerrainLayer(layer::LayerType::COAST), index))
    {
        GetTerrainLayer(layer::LayerType::MIXED)->tiles.at(index) = GetTerrainLayer(layer::LayerType::COAST)->tiles.at(index);
    }

    if (ShouldReplaceTile(GetTerrainLayer(layer::LayerType::TERRAIN), index))
    {
        GetTerrainLayer(layer::LayerType::MIXED)->tiles.at(index) = GetTerrainLayer(layer::LayerType::TERRAIN)->tiles.at(index);
    }
}

bool mdcii::world::Island::ShouldReplaceTile(const layer::TerrainLayer<tile::TerrainTile>* t_layer, const int t_index)
{
    return t_layer->tiles.at(t_index).HasBuilding() && !GetTerrainLayer(layer::LayerType::BUILDINGS)->tiles.at(t_index).HasBuilding();
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
    c["coast"] = t_island.GetTerrainLayer(layer::LayerType::COAST)->tiles;

    auto t = nlohmann::json::object();
    t["terrain"] = t_island.GetTerrainLayer(layer::LayerType::TERRAIN)->tiles;

    auto b = nlohmann::json::object();
    b["buildings"] = t_island.GetTerrainLayer(layer::LayerType::BUILDINGS)->tiles;

    auto f = nlohmann::json::object();
    f["figures"] = t_island.GetFiguresLayer()->tiles;

    t_json["layers"].push_back(c);
    t_json["layers"].push_back(t);
    t_json["layers"].push_back(b);
    t_json["layers"].push_back(f);
}
