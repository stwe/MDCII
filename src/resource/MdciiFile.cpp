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

mdcii::resource::MdciiFile::MdciiFile(std::string t_fileName)
    : m_fileName{ std::move(t_fileName) }
{
    MDCII_LOG_DEBUG("[MdciiFile::MdciiFile()] Create MdciiFile.");

    MDCII_ASSERT(!m_fileName.empty(), "[MdciiFile::MdciiFile()] Invalid file name.")
}

mdcii::resource::MdciiFile::~MdciiFile() noexcept
{
    MDCII_LOG_DEBUG("[MdciiFile::~MdciiFile()] Destruct MdciiFile.");
}

//-------------------------------------------------
// Content to Json
//-------------------------------------------------

bool mdcii::resource::MdciiFile::SetJsonFromFile()
{
    MDCII_LOG_DEBUG("[MdciiFile::SetJsonFromFile()] Start loading Json value from file {}.", m_fileName);

    MDCII_ASSERT(m_json.empty(), "[MdciiFile::SetJsonFromFile()] Invalid Json value.")

    if (std::filesystem::exists(m_fileName))
    {
        m_json = read_json_from_file(m_fileName);
        MDCII_LOG_DEBUG("[MdciiFile::SetJsonFromFile()] The Json value was successfully read from file {}.", m_fileName);

        return true;
    }

    MDCII_LOG_WARN("[MdciiFile::SetJsonFromFile()] File {} does not exist.", m_fileName);

    return false;
}

void mdcii::resource::MdciiFile::SetJsonFromIsland(
    const int t_width, const int t_height,
    const world::ClimateZone t_climateZone,
    const std::vector<world::Tile>& t_terrainTiles,
    const std::vector<world::Tile>& t_coastTiles
)
{
    MDCII_LOG_DEBUG("[MdciiFile::SetJsonFromIsland()] Set new values for the Json keys.");

    MDCII_ASSERT(m_json.empty(), "[MdciiFile::SetJsonFromIsland()] Invalid Json value.")

    // island
    m_json["width"] = t_width;
    m_json["height"] = t_height;
    m_json["x"] = -1;
    m_json["y"] = -1;
    m_json["climate"] = std::string(magic_enum::enum_name(t_climateZone));

    // layers
    m_json["layers"] = nlohmann::json::array();

    auto c = nlohmann::json::object();
    c["coast"] = t_terrainTiles;

    auto t = nlohmann::json::object();
    t["terrain"] = t_coastTiles;

    m_json["layers"].push_back(c);
    m_json["layers"].push_back(t);
}

//-------------------------------------------------
// Content from Json
//-------------------------------------------------

bool mdcii::resource::MdciiFile::CreateFileFromJson()
{
    InitFileName(std::string(ISLAND_RELATIVE_PATH), std::string(ISLAND_FILE_EXTENSION));

    MDCII_LOG_DEBUG("[MdciiFile::CreateFileFromJson()] Start saving Json value to file {}.", m_fileName);

    if (std::ofstream file; create_file(m_fileName, file))
    {
        file << m_json;
        MDCII_LOG_DEBUG("[MdciiFile::CreateFileFromJson()] The Json value has been successfully saved in file {}.", m_fileName);

        return true;
    }

    MDCII_LOG_WARN("[MdciiFile::CreateFileFromJson()] An error occurred while saving Json value to file {}.", m_fileName);

    return false;
}

void mdcii::resource::MdciiFile::CreateWorldContentFromJson(world::World* t_world) const
{
    MDCII_LOG_DEBUG("[MdciiFile::CreateWorldContent()] Start creating the world from Json value ...");

    std::vector<std::unique_ptr<world::Island>> is;

    t_world->worldWidth = m_json.at("world").at("width").get<int32_t>();
    t_world->worldHeight = m_json.at("world").at("height").get<int32_t>();

    if (t_world->worldWidth <= 0 || t_world->worldHeight <= 0)
    {
        throw MDCII_EXCEPTION("[MdciiFile::CreateWorldContent()] Invalid world size.");
    }

    MDCII_LOG_DEBUG("[MdciiFile::CreateWorldContent()] The size of the world is ({}, {}).", t_world->worldWidth, t_world->worldHeight);

    for (const auto& [islandKeys, islandVars] : m_json["islands"].items())
    {
        auto island{ std::make_unique<world::Island>(
            t_world,
            islandVars.at("width").get<int32_t>(),
            islandVars.at("height").get<int32_t>(),
            islandVars.at("x").get<int32_t>(),
            islandVars.at("y").get<int32_t>()
        ) };

        MDCII_LOG_DEBUG("[MdciiFile::CreateWorldContent()] The size of the island is ({}, {}).", island->width, island->height);
        MDCII_LOG_DEBUG("[MdciiFile::CreateWorldContent()] The island starts at position ({}, {}).", island->startX, island->startY);

        // the data for these three island layers is read from a file
        InitLayerByType(island.get(), islandVars, world::LayerType::COAST);
        InitLayerByType(island.get(), islandVars, world::LayerType::TERRAIN);
        InitLayerByType(island.get(), islandVars, world::LayerType::BUILDINGS);

        // this island layer will later be filled with the data from the other layers
        island->layers.at(magic_enum::enum_integer(world::LayerType::MIXED)) = std::make_unique<world::Layer>(world::LayerType::MIXED, island->width, island->height);
        island->Init();

        is.push_back(std::move(island));
    }

    MDCII_LOG_DEBUG("[MdciiFile::CreateWorldContent()] The world was successfully created from Json.");

    t_world->islands = std::move(is);
}

//-------------------------------------------------
// Layer && Tiles
//-------------------------------------------------

void mdcii::resource::MdciiFile::InitLayerByType(world::Island* t_island, const nlohmann::json& t_vars, const world::LayerType t_layerType)
{
    MDCII_LOG_DEBUG("[MdciiFile::InitLayerByType()] Create and init island layer {}.", magic_enum::enum_name(t_layerType));

    t_island->layers.at(magic_enum::enum_integer(t_layerType)) = std::make_unique<world::Layer>(t_layerType, t_island->width, t_island->height);

    for (auto layers = t_vars.at("layers").items(); const auto& [k, v] : layers)
    {
        for (const auto& [layerNameJson, layerTilesJson] : v.items())
        {
            if (layerNameJson == to_lower_case(std::string(magic_enum::enum_name(t_layerType))))
            {
                CreateLayerTiles(t_island->m_world, t_island->layers.at(magic_enum::enum_integer(t_layerType)).get(), layerTilesJson);

                MDCII_ASSERT(t_island->layers.at(magic_enum::enum_integer(t_layerType))->tiles.size()
                             == t_island->width * static_cast<size_t>(t_island->height),
                             "[MdciiFile::CreateWorldContent()] Invalid number of tiles.")
            }
        }
    }
}

void mdcii::resource::MdciiFile::ExtractTileData(const world::World* t_world, const nlohmann::json& t_source, world::Tile* t_tileTarget)
{
    if (t_source.count("id") && t_source.at("id") >= 0)
    {
        if (t_world)
        {
            const auto& building{ t_world->gameState->game->originalResourcesManager->GetBuildingById(t_source.at("id")) };
            t_tileTarget->building = &building;
        }
        else
        {
            throw MDCII_EXCEPTION("[MdciiFile::ExtractTileData()] Null pointer.");
        }
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

void mdcii::resource::MdciiFile::CreateLayerTiles(const world::World* t_world, world::Layer* t_layer, const nlohmann::json& t_layerTilesJson)
{
    MDCII_LOG_DEBUG("[MdciiFile::CreateLayerTiles()] Create layer tiles for layer of type {}.", magic_enum::enum_name(t_layer->layerType));

    for (const auto& [k, tileJson] : t_layerTilesJson.items())
    {
        world::Tile tile;
        ExtractTileData(t_world, tileJson, &tile);
        t_layer->tiles.push_back(tile);
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::resource::MdciiFile::InitFileName(const std::string& t_relPath, const std::string& t_fileExtension)
{
    if (m_fileName.find(fmt::format("{}{}", Game::RESOURCES_REL_PATH, t_relPath)) == std::string::npos)
    {
        m_fileName = fmt::format("{}{}{}", Game::RESOURCES_REL_PATH, t_relPath, m_fileName.append(t_fileExtension));

        MDCII_LOG_DEBUG("[MdciiFile::InitFileName()] The full path to the file was initialized to {}.", m_fileName);
    }
}
