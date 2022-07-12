#include <magic_enum.hpp>
#include "MapLayer.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "MdciiException.h"
#include "ecs/Components.h"
#include "ogl/resource/ResourceManager.h"
#include "data/Buildings.h"

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

mdcii::map::MapLayer::MapLayer(const int t_width, const int t_height, std::shared_ptr<data::Buildings> t_buildings)
    : m_width{ t_width }
    , m_height{ t_height }
    , m_buildings{ std::move(t_buildings) }
{
    Log::MDCII_LOG_DEBUG("[MapLayer::MapLayer()] Create MapLayer.");

    MDCII_ASSERT(m_width, "[MapLayer::MapLayer()] Invalid width.")
    MDCII_ASSERT(m_height, "[MapLayer::MapLayer()] Invalid height.")
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
    mapTiles.at(GetMapIndex(t_mapX, t_mapY)) = t_mapTile;
}

//-------------------------------------------------
// Get tile
//-------------------------------------------------

const mdcii::map::MapTile& mdcii::map::MapLayer::GetTile(const int t_mapX, const int t_mapY) const
{
    return mapTiles.at(GetMapIndex(t_mapX, t_mapY));
}

mdcii::map::MapTile& mdcii::map::MapLayer::GetTile(const int t_mapX, const int t_mapY)
{
    return mapTiles.at(GetMapIndex(t_mapX, t_mapY));
}

//-------------------------------------------------
// Ecs
//-------------------------------------------------

void mdcii::map::MapLayer::CreateEntities()
{
    for (auto y{ 0 }; y < m_height; ++y)
    {
        for (auto x{ 0 }; x < m_width; ++x)
        {
            // create grid entities only once for terrain
            if (layerType == LayerType::TERRAIN)
            {
                CreateGridEntity(x, y);
            }

            CreateEntity(x, y);
        }
    }
}

void mdcii::map::MapLayer::SortEntities(const Rotation t_rotation)
{
    auto i{ magic_enum::enum_integer(t_rotation) };

    Game::ecs.sort<ecs::TileComponent>([i](const ecs::TileComponent& t_lhs, const ecs::TileComponent& t_rhs)
    {
        return t_lhs.mapTile.indices[i] < t_rhs.mapTile.indices[i];
    });
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

glm::vec2 mdcii::map::MapLayer::LayerToScreen(const int t_mapX, const int t_mapY, const Rotation t_rotation) const
{
    MDCII_ASSERT(t_mapX >= 0, "[MapLayer::LayerToScreen()] Invalid x position given.")
    MDCII_ASSERT(t_mapY >= 0, "[MapLayer::LayerToScreen()] Invalid y position given.")

    const auto position{ RotatePosition(t_mapX, t_mapY, t_rotation) };

    return {
        (position.x - position.y) * MapTile::TILE_WIDTH_HALF,
        (position.x + position.y) * MapTile::TILE_HEIGHT_HALF
    };
}

bool mdcii::map::MapLayer::IsPositionInLayer(const glm::ivec2& t_position) const
{
    if (t_position.x >= 0 && t_position.x < m_width &&
        t_position.y >= 0 && t_position.y < m_height)
    {
        return true;
    }

    return false;
}

glm::ivec2 mdcii::map::MapLayer::RotatePosition(const int t_mapX, const int t_mapY, const Rotation t_rotation) const
{
    auto x{ t_mapX };
    auto y{ t_mapY };

    switch (t_rotation)
    {
    case Rotation::DEG0:
        break;
    case Rotation::DEG90:
        x = m_width - t_mapY - 1;
        y = t_mapX;
        break;
    case Rotation::DEG180:
        x = m_width - t_mapX - 1;
        y = m_height - t_mapY - 1;
        break;
    case Rotation::DEG270:
        x = t_mapY;
        y = m_height - t_mapX - 1;
        break;
    }

    return { x, y };
}

int mdcii::map::MapLayer::GetMapIndex(const int t_mapX, const int t_mapY, const Rotation t_rotation) const
{
    MDCII_ASSERT(t_mapX >= 0, "[MapLayer::GetMapIndex()] Invalid x position given.")
    MDCII_ASSERT(t_mapY >= 0, "[MapLayer::GetMapIndex()] Invalid y position given.")

    const auto position{ RotatePosition(t_mapX, t_mapY, t_rotation) };

    return position.y * m_width + position.x;
}

//-------------------------------------------------
// Precalculations
//-------------------------------------------------

void mdcii::map::MapLayer::PreCalcTiles()
{
    for (auto y{ 0 }; y < m_height; ++y)
    {
        for (auto x{ 0 }; x < m_width; ++x)
        {
            PreCalcTile(mapTiles.at(GetMapIndex(x, y)), x, y);
        }
    }
}

//-------------------------------------------------
// Ecs
//-------------------------------------------------

entt::entity mdcii::map::MapLayer::CreatePlainEntity()
{
    return Game::ecs.create();
}

void mdcii::map::MapLayer::CreateGridEntity(const int t_mapX, const int t_mapY)
{
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

void mdcii::map::MapLayer::CreateEntity(const int t_mapX, const int t_mapY)
{
    // skip tiles without building Id
    if (!GetTile(t_mapX, t_mapY).HasBuilding())
    {
        return;
    }

    // create an EnTT entity
    const auto entity{ CreatePlainEntity() };

    // store entity handle
    GetTile(t_mapX, t_mapY).entity = entity;

    // add TileComponent
    Game::ecs.emplace<ecs::TileComponent>(
        entity,
        GetTile(t_mapX, t_mapY),
        m_buildings->buildingsMap.at(GetTile(t_mapX, t_mapY).buildingId)
    );

    // add terrain or building component
    if (layerType == LayerType::TERRAIN)
    {
        Game::ecs.emplace<ecs::TerrainLayerComponent>(entity, 0);
    }

    if (layerType == LayerType::BUILDINGS)
    {
        Game::ecs.emplace<ecs::BuildingsLayerComponent>(entity, 1);
    }
}

//-------------------------------------------------
// Precalculations
//-------------------------------------------------

void mdcii::map::MapLayer::PreCalcTile(MapTile& t_mapTile, const int t_mapX, const int t_mapY) const
{
    // set layer/map position
    t_mapTile.mapX = t_mapX;
    t_mapTile.mapY = t_mapY;

    // pre-calculate the position on the screen for each rotation
    t_mapTile.screenPositions[0] = LayerToScreen(t_mapX, t_mapY, Rotation::DEG0);
    t_mapTile.screenPositions[1] = LayerToScreen(t_mapX, t_mapY, Rotation::DEG90);
    t_mapTile.screenPositions[2] = LayerToScreen(t_mapX, t_mapY, Rotation::DEG180);
    t_mapTile.screenPositions[3] = LayerToScreen(t_mapX, t_mapY, Rotation::DEG270);

    // pre-calculate the index for each rotation for sorting
    t_mapTile.indices[0] = GetMapIndex(t_mapX, t_mapY, Rotation::DEG0);
    t_mapTile.indices[1] = GetMapIndex(t_mapX, t_mapY, Rotation::DEG90);
    t_mapTile.indices[2] = GetMapIndex(t_mapX, t_mapY, Rotation::DEG180);
    t_mapTile.indices[3] = GetMapIndex(t_mapX, t_mapY, Rotation::DEG270);

    // pre-calculate a gfx for each rotation
    if (t_mapTile.HasBuilding())
    {
        const auto building{ m_buildings->buildingsMap.at(t_mapTile.buildingId) };
        const auto gfx0{ building.gfx };

        t_mapTile.gfxs.push_back(gfx0);
        if (building.rotate > 0)
        {
            t_mapTile.gfxs.push_back(gfx0 + (1 * building.rotate));
            t_mapTile.gfxs.push_back(gfx0 + (2 * building.rotate));
            t_mapTile.gfxs.push_back(gfx0 + (3 * building.rotate));
        }
    }
}
