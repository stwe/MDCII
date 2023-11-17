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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#include "MdciiFile.h"
#include "MdciiAssert.h"
#include "MdciiUtils.h"
#include "world/Island.h"
#include "world/Layer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::resource::MdciiFile::MdciiFile(std::string t_fileName)
    : fileName{ std::move(t_fileName) }
{
    MDCII_LOG_DEBUG("[MdciiFile::MdciiFile()] Create MdciiFile.");

    MDCII_ASSERT(!fileName.empty(), "[MdciiFile::MdciiFile()] Invalid file name.")
}

mdcii::resource::MdciiFile::~MdciiFile() noexcept
{
    MDCII_LOG_DEBUG("[MdciiFile::~MdciiFile()] Destruct MdciiFile.");
}

//-------------------------------------------------
// Save && load
//-------------------------------------------------

bool mdcii::resource::MdciiFile::LoadJsonFromFile()
{
    MDCII_LOG_DEBUG("[MdciiFile::LoadJsonFromFile()] Start loading Json value from file {}.", fileName);

    if (std::filesystem::exists(fileName))
    {
        json = read_json_from_file(fileName);
        MDCII_LOG_DEBUG("[MdciiFile::LoadJsonFromFile()] The Json value was successfully read from file {}.", fileName);

        return true;
    }

    MDCII_LOG_WARN("[MdciiFile::LoadJsonFromFile()] File {} does not exist.", fileName);

    return false;
}

std::vector<std::unique_ptr<mdcii::world::Island>> mdcii::resource::MdciiFile::CreateWorldContent(int& t_worldWidth, int& t_worldHeight) const
{
    MDCII_LOG_DEBUG("[MdciiFile::CreateWorldContent()] Start creating the world from file {} ...", fileName);

    std::vector<std::unique_ptr<world::Island>> is;

    t_worldWidth = json.at("world").at("width").get<int32_t>();
    t_worldHeight = json.at("world").at("height").get<int32_t>();

    MDCII_LOG_DEBUG("[MdciiFile::CreateWorldContent()] The size of the world is ({}, {}).", t_worldWidth, t_worldHeight);

    for (const auto& [islandKeys, islandVars] : json["islands"].items())
    {
        auto island{ std::make_unique<world::Island>() };
        island->width = islandVars.at("width").get<int32_t>();
        island->height = islandVars.at("height").get<int32_t>();
        island->startX = islandVars.at("x").get<int32_t>();
        island->startY = islandVars.at("y").get<int32_t>();

        MDCII_LOG_DEBUG("[MdciiFile::CreateWorldContent()] The size of the island is ({}, {}).", island->width, island->height);
        MDCII_LOG_DEBUG("[MdciiFile::CreateWorldContent()] The island starts at position ({}, {}).", island->startX, island->startY);

        // the data for these three layers is read from a file
        InitLayerByType(island.get(), islandVars, world::LayerType::COAST);
        InitLayerByType(island.get(), islandVars, world::LayerType::TERRAIN);
        InitLayerByType(island.get(), islandVars, world::LayerType::BUILDINGS);

        // this layer will later be filled with the data from the other layers
        island->layers.at(magic_enum::enum_integer(world::LayerType::TERRAIN_BUILDINGS)) = std::make_unique<world::Layer>(world::LayerType::TERRAIN_BUILDINGS);

        is.push_back(std::move(island));
    }

    MDCII_LOG_DEBUG("[MdciiFile::CreateWorldContent()] The world was successfully created.");

    return is;
}

//-------------------------------------------------
// Layer && Tiles
//-------------------------------------------------

void mdcii::resource::MdciiFile::InitLayerByType(world::Island* t_island, const nlohmann::json& t_vars, const world::LayerType t_layerType)
{
    MDCII_LOG_DEBUG("[MdciiFile::InitLayerByType()] Create and init island layer {}.", magic_enum::enum_name(t_layerType));

    t_island->layers.at(magic_enum::enum_integer(t_layerType)) = std::make_unique<world::Layer>(t_layerType);

    for (auto layers = t_vars.at("layers").items(); const auto& [k, v] : layers)
    {
        for (const auto& [layerNameJson, layerTilesJson] : v.items())
        {
            if (layerNameJson == to_lower_case(std::string(magic_enum::enum_name(t_layerType))))
            {
                CreateLayerTiles(
                    t_island->layers.at(magic_enum::enum_integer(t_layerType)).get(),
                    layerTilesJson
                );

                MDCII_ASSERT(
                    t_island->layers.at(magic_enum::enum_integer(t_layerType))->tiles.size()
                    == t_island->width * t_island->height,
                    "[MdciiFile::CreateWorldContent()] Invalid number of tiles."
                )
            }
        }
    }
}

void mdcii::resource::MdciiFile::ExtractTileData(const nlohmann::json& t_source, world::Tile* t_tileTarget)
{
    if (t_source.count("id"))
    {
        t_tileTarget->buildingId = t_source.at("id");
    }

    if (t_source.count("rotation"))
    {
        t_tileTarget->rotation = t_source.at("rotation");
    }

    if (t_source.count("x"))
    {
        t_tileTarget->x = t_source.at("x");
    }

    if (t_source.count("y"))
    {
        t_tileTarget->y = t_source.at("y");
    }
}

void mdcii::resource::MdciiFile::CreateLayerTiles(world::Layer* t_layer, const nlohmann::json& t_layerTilesJson)
{
    MDCII_LOG_DEBUG("[MdciiFile::CreateLayerTiles()] Create layer tiles for layer of type {}.", magic_enum::enum_name(t_layer->layerType));

    for (const auto& [k, tileJson] : t_layerTilesJson.items())
    {
        world::Tile tile;
        ExtractTileData(tileJson, &tile);
        t_layer->tiles.push_back(std::move(tile));
    }
}
