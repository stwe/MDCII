// This file is part of the MDCII project.
//
// Copyright (c) 2022. stwe <https://github.com/stwe/MDCII>
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "MapLayer.h"
#include "MapContent.h"
#include "MdciiAssert.h"
#include "ecs/Components.h"
#include "ecs/EcsUtils.h"
#include "ogl/resource/ResourceManager.h"
#include "file/OriginalResourcesManager.h"
#include "state/State.h"

//-------------------------------------------------
// Json
//-------------------------------------------------

void mdcii::map::to_json(nlohmann::json& t_json, const MapTile& t_mapTile)
{
    t_json = nlohmann::json{
        { "id", t_mapTile.buildingId },
        { "rotation", rotation_to_int(t_mapTile.rotation) },
        { "x", t_mapTile.x },
        { "y", t_mapTile.y },
        { "connected", t_mapTile.connectedMapTiles }
    };
}

void mdcii::map::from_json(const nlohmann::json& t_json, MapTile& t_mapTile)
{
    if (t_json.count("id"))
    {
        t_json.at("id").get_to(t_mapTile.buildingId);
    }

    if (t_json.count("rotation"))
    {
        auto r{ 0 };
        t_json.at("rotation").get_to(r);
        t_mapTile.rotation = int_to_rotation(r);
    }

    if (t_json.count("x"))
    {
        t_json.at("x").get_to(t_mapTile.x);
    }

    if (t_json.count("y"))
    {
        t_json.at("y").get_to(t_mapTile.y);
    }

    if (t_json.count("connected"))
    {
        t_json.at("connected").get_to(t_mapTile.connectedMapTiles);
    }
}

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::map::MapLayer::MapLayer(MapContent* t_mapContent)
    : m_mapContent{ t_mapContent }
{
    Log::MDCII_LOG_DEBUG("[MapLayer::MapLayer()] Create MapLayer.");

    MDCII_ASSERT(m_mapContent, "[MapLayer::MapLayer()] Null pointer.")

    // store grid file names
    magic_enum::enum_for_each<Zoom>([&](const Zoom t_zoom)
    {
        const auto zoomStr{ to_lower_case(std::string(magic_enum::enum_name<Zoom>(t_zoom))) };
        const auto fileName{ "textures/" + zoomStr + "/red_" + zoomStr + ".png" };
        m_gridFileNames.at(magic_enum::enum_integer(t_zoom)) = fileName;
    });
}

mdcii::map::MapLayer::~MapLayer() noexcept
{
    Log::MDCII_LOG_DEBUG("[MapLayer::~MapLayer()] Destruct MapLayer.");
}

//-------------------------------------------------
// Layer type
//-------------------------------------------------

void mdcii::map::MapLayer::SetLayerTypeByString(const std::string& t_layerType)
{
    auto name{ t_layerType };
    std::transform(name.begin(), name.end(), name.begin(),
        [](const unsigned char t_c) { return std::toupper(t_c); });

    const auto newType{ magic_enum::enum_cast<LayerType>(name) };
    if (newType.has_value())
    {
        layerType = newType.value();
    }
    else
    {
        throw MDCII_EXCEPTION("[MapLayer::SetLayerTypeByString()] Invalid layer type " + t_layerType + " given.");
    }
}

//-------------------------------------------------
// Add/replace tile
//-------------------------------------------------

void mdcii::map::MapLayer::AddTileFromJson(const nlohmann::json& t_json)
{
    mapTiles.emplace_back(t_json.get<MapTile>());
}

void mdcii::map::MapLayer::ReplaceTile(const MapTile& t_mapTile)
{
    mapTiles.at(m_mapContent->GetMapIndex(t_mapTile.mapX, t_mapTile.mapY)) = t_mapTile;
}

//-------------------------------------------------
// Get tile
//-------------------------------------------------

const mdcii::map::MapTile& mdcii::map::MapLayer::GetTile(const int t_mapX, const int t_mapY) const
{
    return mapTiles.at(m_mapContent->GetMapIndex(t_mapX, t_mapY));
}

mdcii::map::MapTile& mdcii::map::MapLayer::GetTile(const int t_mapX, const int t_mapY)
{
    return mapTiles.at(m_mapContent->GetMapIndex(t_mapX, t_mapY));
}

//-------------------------------------------------
// Ecs
//-------------------------------------------------

void mdcii::map::MapLayer::CreateGridEntity(const int t_mapX, const int t_mapY)
{
    MDCII_ASSERT(layerType == LayerType::TERRAIN, "[MapLayer::CreateGridEntity()] Wrong layer type.")

    // skip tiles without building Id
    if (!GetTile(t_mapX, t_mapY).HasBuilding())
    {
        return;
    }

    // create an EnTT entity
    const auto entity{ ecs::EcsUtils::CreatePlainEntity() };

    // store entity handle
    GetTile(t_mapX, t_mapY).gridEntity = entity;

    // add GridComponent
    Game::ecs.emplace<ecs::GridComponent>(
        entity,
        GetTile(t_mapX, t_mapY),
        ogl::resource::ResourceManager::LoadTexture(m_gridFileNames.at(magic_enum::enum_integer(m_mapContent->zoom))).id
    );
}

void mdcii::map::MapLayer::CreateTerrainLayerEntity(const int t_mapX, const int t_mapY)
{
    MDCII_ASSERT(layerType == LayerType::TERRAIN, "[MapLayer::CreateTerrainLayerEntity()] Wrong layer type.")

    // skip tiles without building Id
    if (!GetTile(t_mapX, t_mapY).HasBuilding())
    {
        return;
    }

    // create an EnTT entity
    const auto entity{ ecs::EcsUtils::CreatePlainEntity() };

    // store entity handle
    GetTile(t_mapX, t_mapY).entity = entity;

    // add TerrainLayerTileComponent
    Game::ecs.emplace<ecs::TerrainLayerTileComponent>(
        entity,
        GetTile(t_mapX, t_mapY),
        m_mapContent->context->originalResourcesManager->GetBuildingById(GetTile(t_mapX, t_mapY).buildingId)
    );
}

void mdcii::map::MapLayer::AddBuildingsLayerComponent(const int t_mapX, const int t_mapY)
{
    MDCII_ASSERT(layerType == LayerType::BUILDINGS, "[MapLayer::AddBuildingsLayerComponent()] Wrong layer type.")

    const auto& terrainTile{ m_mapContent->GetLayer(LayerType::TERRAIN).GetTile(t_mapX, t_mapY) };

    // mandatory
    if (!terrainTile.HasBuilding())
    {
        return;
    }

    // skip tiles without building Id
    if (!GetTile(t_mapX, t_mapY).HasBuilding())
    {
        return;
    }

    // store entity handle
    GetTile(t_mapX, t_mapY).entity = terrainTile.entity;
    GetTile(t_mapX, t_mapY).gridEntity = terrainTile.gridEntity;

    // add BuildingsLayerTileComponent
    Game::ecs.emplace<ecs::BuildingsLayerTileComponent>(
        GetTile(t_mapX, t_mapY).entity,
        GetTile(t_mapX, t_mapY),
        m_mapContent->context->originalResourcesManager->GetBuildingById(GetTile(t_mapX, t_mapY).buildingId)
    );
}
