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
#include "MdciiException.h"
#include "MdciiUtils.h"
#include "GameState.h"
#include "Game.h"
#include "world/World.h"
#include "world/Island.h"
#include "world/Layer.h"
#include "resource/OriginalResourcesManager.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::resource::MdciiFile::MdciiFile(world::World* t_world, std::string t_fileName)
    : m_world{ t_world }
    , m_fileName{ std::move(t_fileName) }
{
    MDCII_LOG_DEBUG("[MdciiFile::MdciiFile()] Create MdciiFile.");

    MDCII_ASSERT(m_world, "[MdciiFile::MdciiFile()] Null pointer.")
    MDCII_ASSERT(!m_fileName.empty(), "[MdciiFile::MdciiFile()] Invalid file name.")
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
    MDCII_LOG_DEBUG("[MdciiFile::LoadJsonFromFile()] Start loading Json value from file {}.", m_fileName);

    if (std::filesystem::exists(m_fileName))
    {
        m_json = read_json_from_file(m_fileName);
        MDCII_LOG_DEBUG("[MdciiFile::LoadJsonFromFile()] The Json value was successfully read from file {}.", m_fileName);

        return true;
    }

    MDCII_LOG_WARN("[MdciiFile::LoadJsonFromFile()] File {} does not exist.", m_fileName);

    return false;
}

std::vector<std::unique_ptr<mdcii::world::Island>> mdcii::resource::MdciiFile::CreateWorldContent() const
{
    MDCII_LOG_DEBUG("[MdciiFile::CreateWorldContent()] Start creating the world from file {} ...", m_fileName);

    std::vector<std::unique_ptr<world::Island>> is;

    m_world->worldWidth = m_json.at("world").at("width").get<int32_t>();
    m_world->worldHeight = m_json.at("world").at("height").get<int32_t>();

    if (m_world->worldWidth <= 0 || m_world->worldHeight <= 0)
    {
        throw MDCII_EXCEPTION("[MdciiFile::CreateWorldContent()] Invalid world size.");
    }

    MDCII_LOG_DEBUG("[MdciiFile::CreateWorldContent()] The size of the world is ({}, {}).", m_world->worldWidth, m_world->worldHeight);

    for (const auto& [islandKeys, islandVars] : m_json["islands"].items())
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
        island->layers.at(magic_enum::enum_integer(world::LayerType::MIXED)) = std::make_unique<world::Layer>(world::LayerType::MIXED);

        is.push_back(std::move(island));
    }

    MDCII_LOG_DEBUG("[MdciiFile::CreateWorldContent()] The world was successfully created.");

    return is;
}

//-------------------------------------------------
// Layer && Tiles
//-------------------------------------------------

void mdcii::resource::MdciiFile::InitLayerByType(world::Island* t_island, const nlohmann::json& t_vars, const world::LayerType t_layerType) const
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

void mdcii::resource::MdciiFile::ExtractTileData(const nlohmann::json& t_source, world::Tile* t_tileTarget) const
{
    if (t_source.count("id") && t_source.at("id") >= 0)
    {
        const auto& building{ m_world->gameState->game->originalResourcesManager->GetBuildingById(t_source.at("id")) };
        t_tileTarget->building = &building;
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

void mdcii::resource::MdciiFile::CreateLayerTiles(world::Layer* t_layer, const nlohmann::json& t_layerTilesJson) const
{
    MDCII_LOG_DEBUG("[MdciiFile::CreateLayerTiles()] Create layer tiles for layer of type {}.", magic_enum::enum_name(t_layer->layerType));

    for (const auto& [k, tileJson] : t_layerTilesJson.items())
    {
        world::Tile tile;
        ExtractTileData(tileJson, &tile);
        t_layer->tiles.push_back(std::move(tile));
    }
}
