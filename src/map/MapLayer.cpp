#include <magic_enum.hpp>
#include "MapLayer.h"
#include "MapContent.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "MdciiException.h"
#include "ecs/Components.h"
#include "ogl/resource/ResourceManager.h"

//-------------------------------------------------
// Types, Types conversions && Operators
//-------------------------------------------------

void mdcii::map::to_json(nlohmann::json& t_json, const MapTile& t_mapTile)
{
    t_json = nlohmann::json{
        { "id", t_mapTile.buildingId },
        { "orientation", t_mapTile.orientation },
        { "x", t_mapTile.x },
        { "y", t_mapTile.y }
    };
}

void mdcii::map::from_json(const nlohmann::json& t_json, MapTile& t_mapTile)
{
    if (t_json.count("id"))
    {
        t_json.at("id").get_to(t_mapTile.buildingId);
    }

    if (t_json.count("orientation"))
    {
        t_json.at("orientation").get_to(t_mapTile.orientation);
    }

    if (t_json.count("x"))
    {
        t_json.at("x").get_to(t_mapTile.x);
    }

    if (t_json.count("y"))
    {
        t_json.at("y").get_to(t_mapTile.y);
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

void mdcii::map::MapLayer::AddTile(const MapTile& t_mapTile)
{
    mapTiles.emplace_back(t_mapTile);
}

void mdcii::map::MapLayer::ReplaceTile(const int t_mapX, const int t_mapY, const MapTile& t_mapTile)
{
    mapTiles.at(m_mapContent->GetMapIndex(t_mapX, t_mapY)) = t_mapTile;
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
    const auto entity{ CreatePlainEntity() };

    // store entity handle
    GetTile(t_mapX, t_mapY).gridEntity = entity;

    // add GridComponent
    Game::ecs.emplace<ecs::GridComponent>(
        entity,
        GetTile(t_mapX, t_mapY),
        ogl::resource::ResourceManager::LoadTexture("textures/red.png").id
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
    const auto entity{ CreatePlainEntity() };

    // store entity handle
    GetTile(t_mapX, t_mapY).entity = entity;

    // add TerrainLayerTileComponent
    Game::ecs.emplace<ecs::TerrainLayerTileComponent>(
        entity,
        GetTile(t_mapX, t_mapY),
        m_mapContent->buildings->buildingsMap.at(GetTile(t_mapX, t_mapY).buildingId)
    );
}

void mdcii::map::MapLayer::AddBuildingsLayerComponent(const int t_mapX, const int t_mapY)
{
    MDCII_ASSERT(layerType == LayerType::BUILDINGS, "[MapLayer::AddBuildingsLayerComponent()] Wrong layer type.")

    const auto& terrainTile{ m_mapContent->GetLayer(LayerType::TERRAIN).GetTile(t_mapX, t_mapY) };
    if (!terrainTile.HasBuilding())
    {
        return;
    }

    // skip tiles without building Id
    if (!GetTile(t_mapX, t_mapY).HasBuilding())
    {
        return;
    }

    // add BuildingsLayerTileComponent
    Game::ecs.emplace<ecs::BuildingsLayerTileComponent>(
        terrainTile.entity,
        GetTile(t_mapX, t_mapY),
        m_mapContent->buildings->buildingsMap.at(GetTile(t_mapX, t_mapY).buildingId)
    );
}

void mdcii::map::MapLayer::SortEntities(const Rotation t_rotation)
{
    auto i{ magic_enum::enum_integer(t_rotation) };

    Game::ecs.sort<ecs::TerrainLayerTileComponent>([i](const ecs::TerrainLayerTileComponent& t_lhs, const ecs::TerrainLayerTileComponent& t_rhs)
    {
        return t_lhs.mapTile.indices[i] < t_rhs.mapTile.indices[i];
    });
}

//-------------------------------------------------
// Ecs
//-------------------------------------------------

entt::entity mdcii::map::MapLayer::CreatePlainEntity()
{
    return Game::ecs.create();
}
