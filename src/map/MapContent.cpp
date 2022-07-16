#include <fstream>
#include <magic_enum.hpp>
#include "MapContent.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "MdciiException.h"
#include "ecs/Components.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::map::MapContent::MapContent(const std::string& t_filePath, std::shared_ptr<data::Buildings> t_buildings)
    : buildings{ std::move(t_buildings) }
{
    Log::MDCII_LOG_DEBUG("[MapContent::MapContent()] Create MapContent.");

    CreateContent(t_filePath);
    PreCalcTiles();

    CreateEntitiesOfAllLayers();
    SortEntitiesOfAllLayers();
}

mdcii::map::MapContent::~MapContent() noexcept
{
    Log::MDCII_LOG_DEBUG("[MapContent::~MapContent()] Destruct MapContent.");
}

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

const mdcii::map::MapLayer& mdcii::map::MapContent::GetLayer(const LayerType t_layerType) const
{
    return *mapLayers.at(magic_enum::enum_integer(t_layerType));
}

mdcii::map::MapLayer& mdcii::map::MapContent::GetLayer(const LayerType t_layerType)
{
    return *mapLayers.at(magic_enum::enum_integer(t_layerType));
}

//-------------------------------------------------
// Sort
//-------------------------------------------------

void mdcii::map::MapContent::SortEntitiesOfAllLayers() const
{
    for (const auto& layer : mapLayers)
    {
        layer->SortEntities(rotation);
    }
}

//-------------------------------------------------
// Rotate
//-------------------------------------------------

void mdcii::map::MapContent::RotateLeft()
{
    --rotation;
    SortEntitiesOfAllLayers();
}

void mdcii::map::MapContent::RotateRight()
{
    ++rotation;
    SortEntitiesOfAllLayers();
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

bool mdcii::map::MapContent::IsPositionInMap(const int t_mapX, const int t_mapY) const
{
    if (t_mapX >= 0 && t_mapX < width &&
        t_mapY >= 0 && t_mapY < height)
    {
        return true;
    }

    return false;
}

int mdcii::map::MapContent::GetMapIndex(const int t_mapX, const int t_mapY, const Rotation t_rotation) const
{
    MDCII_ASSERT(IsPositionInMap(t_mapX, t_mapY), "[MapContent::GetMapIndex()] Invalid map position given.")

    const auto position{ RotatePosition(t_mapX, t_mapY, t_rotation) };

    return position.y * width + position.x;
}

glm::vec2 mdcii::map::MapContent::MapToScreen(const int t_mapX, const int t_mapY, const Rotation t_rotation) const
{
    MDCII_ASSERT(IsPositionInMap(t_mapX, t_mapY), "[MapContent::MapToScreen()] Invalid map position given.")

    const auto position{ RotatePosition(t_mapX, t_mapY, t_rotation) };

    return {
        (position.x - position.y) * MapTile::TILE_WIDTH_HALF,
        (position.x + position.y) * MapTile::TILE_HEIGHT_HALF
    };
}

glm::ivec2 mdcii::map::MapContent::RotatePosition(const int t_mapX, const int t_mapY, const Rotation t_rotation) const
{
    auto x{ t_mapX };
    auto y{ t_mapY };

    switch (t_rotation)
    {
    case Rotation::DEG0:
        break;
    case Rotation::DEG90:
        x = width - t_mapY - 1;
        y = t_mapX;
        break;
    case Rotation::DEG180:
        x = width - t_mapX - 1;
        y = height - t_mapY - 1;
        break;
    case Rotation::DEG270:
        x = t_mapY;
        y = height - t_mapX - 1;
        break;
    }

    return { x, y };
}

//-------------------------------------------------
// Edit
//-------------------------------------------------

void mdcii::map::MapContent::AddBuilding(const int t_mapX, const int t_mapY, const event::SelectedBauGfx& t_selectedBauGfx)
{
    const auto& building{ buildings->buildingsMap.at(t_selectedBauGfx.buildingId) };

    // check all positions before
    for (auto y{ 0 }; y < building.size.h; ++y)
    {
        for (auto x{ 0 }; x < building.size.w; ++x)
        {
            if (!IsPositionInMap(t_mapX + x, t_mapY + y))
            {
                return;
            }

            /*
            if (GetLayer(LayerType::BUILDINGS).GetTile(t_mapX + x, t_mapY + y).HasBuilding())
            {
                return;
            }
            */
        }
    }

    // add
    for (auto y{ 0 }; y < building.size.h; ++y)
    {
        for (auto x{ 0 }; x < building.size.w; ++x)
        {
            const auto& terrainTile{ GetLayer(LayerType::TERRAIN).GetTile(t_mapX + x, t_mapY + y) };
            if (!terrainTile.HasBuilding())
            {
                return;
            }

            // create a new MapTile object
            MapTile mapTile;
            mapTile.buildingId = t_selectedBauGfx.buildingId;
            mapTile.orientation = t_selectedBauGfx.orientation;
            mapTile.x = x;
            mapTile.y = y;

            // add pre calcs
            PreCalcTile(mapTile, t_mapX + x, t_mapY + y);

            // replace the tile in the building layer with the newly created tile
            GetLayer(LayerType::BUILDINGS).ReplaceTile(t_mapX + x, t_mapY + y, mapTile);

            // remove existing building component, if already exists
            Game::ecs.remove<ecs::BuildingsLayerTileComponent>(terrainTile.entity);

            // add a (new) BuildingsLayerTileComponent to the terrain entity
            Game::ecs.emplace<ecs::BuildingsLayerTileComponent>(
                terrainTile.entity,
                mapTile,
                buildings->buildingsMap.at(mapTile.buildingId)
            );
        }
    }
}

void mdcii::map::MapContent::RemoveBuilding(const int t_mapX, const int t_mapY, const event::SelectedBauGfx& t_selectedBauGfx)
{
    const auto& building{ buildings->buildingsMap.at(t_selectedBauGfx.buildingId) };

    // check all positions before
    for (auto y{ 0 }; y < building.size.h; ++y)
    {
        for (auto x{ 0 }; x < building.size.w; ++x)
        {
            if (!IsPositionInMap(t_mapX + x, t_mapY + y))
            {
                return;
            }
        }
    }

    // remove
    for (auto y{ 0 }; y < building.size.h; ++y)
    {
        for (auto x{ 0 }; x < building.size.w; ++x)
        {
            const auto& terrainTile{ GetLayer(LayerType::TERRAIN).GetTile(t_mapX + x, t_mapY + y) };
            if (!terrainTile.HasBuilding())
            {
                return;
            }

            // replace the tile in the building layer with an empty newly created tile
            GetLayer(LayerType::BUILDINGS).ReplaceTile(t_mapX + x, t_mapY + y, {});

            // remove existing building component, if already exists
            Game::ecs.remove<ecs::BuildingsLayerTileComponent>(terrainTile.entity);
        }
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::map::MapContent::CreateContent(const std::string& t_filePath)
{
    Log::MDCII_LOG_DEBUG("[MapContent::CreateContent()] Start reading Json data...");

    nlohmann::json j = ReadJsonFromFile(Game::RESOURCES_PATH + t_filePath);

    for (const auto& [k, v] : j.items())
    {
        if (k == "width")
        {
            width = v.get<int>();

        }
        else if (k == "height")
        {
            height = v.get<int>();
        }
    }

    if (width <= 0 || height <= 0)
    {
        throw MDCII_EXCEPTION("[MapContent::CreateContent()] Invalid width or height given.");
    }

    for (const auto& [k, v] : j.items())
    {
        if (k == "layers")
        {
            for (const auto& [a, o] : v.items())
            {
                auto mapLayer{ std::make_unique<MapLayer>(this) };

                for (const auto& [layerName, layerTiles] : o.items())
                {
                    mapLayer->SetLayerTypeByString(layerName);
                    MDCII_ASSERT(mapLayer->layerType != LayerType::NONE, "[MapContent::CreateContent()] Invalid layer type.")

                    for (const auto& [i, tile] : layerTiles.items())
                    {
                        mapLayer->AddTileFromJson(tile);
                    }
                }

                mapLayers.emplace_back(std::move(mapLayer));
            }
        }
    }

    MDCII_ASSERT(!mapLayers.empty(), "[MapContent::CreateContent()] Missing layers.")
    MDCII_ASSERT(mapLayers.size() == 2, "[MapContent::CreateContent()] Invalid number of layers.")
    MDCII_ASSERT((static_cast<size_t>(width) * static_cast<size_t>(height)) == mapLayers.at(0)->mapTiles.size(), "[MapContent::CreateContent()] Invalid number of tiles.")
    MDCII_ASSERT((static_cast<size_t>(width) * static_cast<size_t>(height)) == mapLayers.at(1)->mapTiles.size(), "[MapContent::CreateContent()] Invalid number of tiles.")

    Log::MDCII_LOG_DEBUG("[MapContent::CreateContent()] Map width: {}", width);
    Log::MDCII_LOG_DEBUG("[MapContent::CreateContent()] Map height: {}", height);
    Log::MDCII_LOG_DEBUG("[MapContent::CreateContent()] Found layers in {}: {}", t_filePath, mapLayers.size());
    Log::MDCII_LOG_DEBUG("[MapContent::CreateContent()] Number of tiles per layer: {}", mapLayers.at(0)->mapTiles.size());

    Log::MDCII_LOG_DEBUG("[MapContent::CreateContent()] Json data read successfully.");
}

nlohmann::json mdcii::map::MapContent::ReadJsonFromFile(const std::string& t_filePath)
{
    nlohmann::json j;

    std::ifstream jsonFile;
    jsonFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        jsonFile.open(t_filePath);
        j = nlohmann::json::parse(jsonFile);
        jsonFile.close();
    }
    catch (const std::ifstream::failure&)
    {
        throw MDCII_EXCEPTION("[MapContent::ReadJsonFromFile()] Exception caught while loading file " + t_filePath + ".");
    }

    return j;
}

//-------------------------------------------------
// Ecs
//-------------------------------------------------

void mdcii::map::MapContent::CreateEntitiesOfAllLayers() const
{
    Log::MDCII_LOG_DEBUG("[MapContent::CreateEntitiesOfAllLayers()] Create entities with components for each layer.");

    for (const auto& layer : mapLayers)
    {
        for (auto y{ 0 }; y < height; ++y)
        {
            for (auto x{ 0 }; x < width; ++x)
            {
                if (layer->layerType == LayerType::TERRAIN)
                {
                    layer->CreateGridEntity(x, y);
                    layer->CreateTerrainLayerEntity(x, y);
                }

                if (layer->layerType == LayerType::BUILDINGS)
                {
                    layer->AddBuildingsLayerComponent(x, y);
                }
            }
        }
    }
}

//-------------------------------------------------
// Pre-calculations
//-------------------------------------------------

void mdcii::map::MapContent::PreCalcTiles() const
{
    Log::MDCII_LOG_DEBUG("[MapContent::PreCalcTiles()] Add some pre-calculations to each layer tile.");

    for (const auto& layer : mapLayers)
    {
        for (auto y{ 0 }; y < height; ++y)
        {
            for (auto x{ 0 }; x < width; ++x)
            {
                PreCalcTile(layer->GetTile(x, y), x, y);
            }
        }
    }
}

void mdcii::map::MapContent::PreCalcTile(MapTile& t_mapTile, const int t_mapX, const int t_mapY) const
{
    // set layer/map position
    t_mapTile.mapX = t_mapX;
    t_mapTile.mapY = t_mapY;

    // pre-calculate the position on the screen for each rotation
    t_mapTile.screenPositions[0] = MapToScreen(t_mapX, t_mapY, Rotation::DEG0);
    t_mapTile.screenPositions[1] = MapToScreen(t_mapX, t_mapY, Rotation::DEG90);
    t_mapTile.screenPositions[2] = MapToScreen(t_mapX, t_mapY, Rotation::DEG180);
    t_mapTile.screenPositions[3] = MapToScreen(t_mapX, t_mapY, Rotation::DEG270);

    // pre-calculate the index for each rotation for sorting
    t_mapTile.indices[0] = GetMapIndex(t_mapX, t_mapY, Rotation::DEG0);
    t_mapTile.indices[1] = GetMapIndex(t_mapX, t_mapY, Rotation::DEG90);
    t_mapTile.indices[2] = GetMapIndex(t_mapX, t_mapY, Rotation::DEG180);
    t_mapTile.indices[3] = GetMapIndex(t_mapX, t_mapY, Rotation::DEG270);

    // pre-calculate a gfx for each rotation
    if (t_mapTile.HasBuilding())
    {
        const auto building{ buildings->buildingsMap.at(t_mapTile.buildingId) };
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
