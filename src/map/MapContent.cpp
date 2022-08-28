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

#include <imgui.h>
#include "MapContent.h"
#include "MdciiAssert.h"
#include "camera/Camera.h"
#include "state/State.h"
#include "ecs/Components.h"
#include "ecs/EcsUtils.h"
#include "file/OriginalResourcesManager.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::map::MapContent::MapContent(const std::string& t_mapFilePath, std::shared_ptr<state::Context> t_context)
    : context{ std::move(t_context) }
{
    Log::MDCII_LOG_DEBUG("[MapContent::MapContent()] Create MapContent.");

    CreateContent(t_mapFilePath);
    PreCalcTiles();

    CreateEntitiesOfAllLayers();
    SortEntitiesOfAllLayers();

    InitBauhausZoom();
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
// Render
//-------------------------------------------------

void mdcii::map::MapContent::RenderImGui() const
{
    ImGui::Begin("MapContent Entities");

    ImGui::Separator();

    // Entities with the GridComponent

    if (ImGui::TreeNode(ecs::EcsUtils::EntityCounterLabel<const ecs::GridComponent>("Grid Entities").c_str()))
    {
        const auto view{ Game::ecs.view<const ecs::GridComponent>() };
        for (const auto entity : view)
        {
            const auto& gc{ view.get<const ecs::GridComponent>(entity) };
            auto x{ std::to_string(gc.mapTile.mapX) };
            auto y{ std::to_string(gc.mapTile.mapY) };

            if (ImGui::TreeNode(x.append(", ").append(y).c_str()))
            {
                ImGui::Separator();
                gc.mapTile.RenderImGui();
                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    ImGui::Separator();

    // Entities with the TerrainLayerTileComponent

    if (ImGui::TreeNode(ecs::EcsUtils::EntityCounterLabel<const ecs::TerrainLayerTileComponent>("Terrain Layer Entities").c_str()))
    {
        const auto view{ Game::ecs.view<const ecs::TerrainLayerTileComponent>() };
        for (const auto entity : view)
        {
            const auto& tc{ view.get<const ecs::TerrainLayerTileComponent>(entity) };
            auto x{ std::to_string(tc.mapTile.mapX) };
            auto y{ std::to_string(tc.mapTile.mapY) };

            if (ImGui::TreeNode(x.append(", ").append(y).c_str()))
            {
                ImGui::Separator();
                tc.mapTile.RenderImGui();
                tc.building.RenderImGui();

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    ImGui::Separator();

    // Entities with the BuildingsLayerTileComponent but not with the BuildingUpdatedComponent

    if (ImGui::TreeNode(ecs::EcsUtils::EntityCounterLabel<const ecs::BuildingsLayerTileComponent>("Buildings Layer Entities", entt::exclude<ecs::BuildingUpdatedComponent>).c_str()))
    {
        const auto view{ Game::ecs.view<const ecs::BuildingsLayerTileComponent>(entt::exclude<ecs::BuildingUpdatedComponent>) };
        for (const auto entity : view)
        {
            const auto& bc{ view.get<const ecs::BuildingsLayerTileComponent>(entity) };
            auto x{ std::to_string(bc.mapTile.mapX) };
            auto y{ std::to_string(bc.mapTile.mapY) };

            if (ImGui::TreeNode(x.append(", ").append(y).c_str()))
            {
                ImGui::Separator();
                bc.mapTile.RenderImGui();
                bc.building.RenderImGui();

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    ImGui::Separator();

    // Entities with the BuildingsLayerTileComponent and the BuildingUpdatedComponent

    auto c{ ecs::EcsUtils::EntityCounterLabel<const ecs::BuildingsLayerTileComponent, const ecs::BuildingUpdatedComponent>("Buildings Layer Entities to add") };
    ImGui::TextUnformatted(c.c_str());

    const auto view{ Game::ecs.view<ecs::TerrainLayerTileComponent, ecs::BuildingsLayerTileComponent, ecs::BuildingUpdatedComponent>() };
    for (const auto entity : view.use<ecs::TerrainLayerTileComponent>())
    {
        const auto& [t, b, u]{ view.get(entity) };
        ImGui::Text("mx: %d, my: %d, x: %d, y: %d, id: %d", b.mapTile.mapX, b.mapTile.mapY, b.mapTile.x, b.mapTile.y, b.building.id);
    }

    ImGui::Separator();

    ImGui::End();
}

//-------------------------------------------------
// Sort
//-------------------------------------------------

void mdcii::map::MapContent::SortEntitiesByRotation(const Rotation t_rotation)
{
    ecs::EcsUtils::SortEntities<ecs::TerrainLayerTileComponent>(t_rotation);
}

void mdcii::map::MapContent::SortEntitiesOfAllLayers() const
{
    SortEntitiesByRotation(rotation);
}

//-------------------------------------------------
// Rotate && Zoom
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

// todo: The camera needs to know the current zoom
void mdcii::map::MapContent::ZoomIn()
{
    ++zoom;
    context->camera->zoom = zoom;
}

void mdcii::map::MapContent::ZoomOut()
{
    --zoom;
    context->camera->zoom = zoom;
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

glm::vec2 mdcii::map::MapContent::MapToScreen(const int t_mapX, const int t_mapY, const Zoom t_zoom, const Rotation t_rotation) const
{
    MDCII_ASSERT(IsPositionInMap(t_mapX, t_mapY), "[MapContent::MapToScreen()] Invalid map position given.")

    const auto position{ RotatePosition(t_mapX, t_mapY, t_rotation) };

    return {
        (position.x - position.y) * get_tile_width_half(t_zoom),
        (position.x + position.y) * get_tile_height_half(t_zoom)
    };
}

glm::ivec2 mdcii::map::MapContent::RotatePosition(const int t_mapX, const int t_mapY, const Rotation t_rotation) const
{
    return rotate_position(t_mapX, t_mapY, width, height, t_rotation);
}

//-------------------------------------------------
// Edit
//-------------------------------------------------

void mdcii::map::MapContent::AddBuildingsLayerComponent(const int t_mapX, const int t_mapY, const event::SelectedBauGfx& t_selectedBauGfx)
{
    const auto& building{ context->originalResourcesManager->GetBuildingById(t_selectedBauGfx.buildingId) };

    // only add if the entire building fits on the map
    if (IsBuildingOutsideTheMap(t_mapX, t_mapY, building))
    {
        return;
    }

    // only add it if there is no other building on the position
    if (IsAlreadyBuildingOnLayer(t_mapX, t_mapY, building, LayerType::BUILDINGS))
    {
        return;
    }

    // don't build on the coast
    if (IsBuildingOnWaterOrCoast(t_mapX, t_mapY, building))
    {
        return;
    }

    // add
    std::vector<int> connectedMapTiles;
    std::vector<entt::entity> entities;
    for (auto y{ 0 }; y < building.size.h; ++y)
    {
        for (auto x{ 0 }; x < building.size.w; ++x)
        {
            // create a new MapTile object
            MapTile mapTile;
            mapTile.buildingId = t_selectedBauGfx.buildingId;
            mapTile.rotation = t_selectedBauGfx.rotation;
            mapTile.x = x;
            mapTile.y = y;

            // run pre-calcs
            PreCalcTile(mapTile, t_mapX + x, t_mapY + y);

            // get terrain layer map tile
            const auto& terrainTile{ GetLayer(LayerType::TERRAIN).GetTile(t_mapX + x, t_mapY + y) };

            // store entity handle
            mapTile.entity = terrainTile.entity;
            mapTile.gridEntity = terrainTile.gridEntity;

            // add/replace BuildingsLayerTileComponent
            Game::ecs.emplace_or_replace<ecs::BuildingsLayerTileComponent>(
                mapTile.entity,
                mapTile,
                context->originalResourcesManager->GetBuildingById(mapTile.buildingId)
            );

            // add/replace BuildingUpdatedComponent
            Game::ecs.emplace_or_replace<ecs::BuildingUpdatedComponent>(terrainTile.entity);

            // to store connected tiles
            if (building.size.h > 1 || building.size.w > 1)
            {
                entities.push_back(mapTile.entity);
                connectedMapTiles.push_back(GetMapIndex(t_mapX + x, t_mapY + y));
            }
        }
    }

    // store connected tiles
    for (const auto entity : entities)
    {
        auto& tileComponent{ Game::ecs.get<ecs::BuildingsLayerTileComponent>(entity) };
        tileComponent.mapTile.connectedMapTiles = connectedMapTiles;
    }
}

void mdcii::map::MapContent::RemoveBuildingsLayerComponent(const int t_mapX, const int t_mapY, const event::SelectedBauGfx& t_selectedBauGfx)
{
    const auto& building{ context->originalResourcesManager->GetBuildingById(t_selectedBauGfx.buildingId) };

    // nothing was built outside the map
    if (IsBuildingOutsideTheMap(t_mapX, t_mapY, building))
    {
        return;
    }

    // nothing was overbuilt
    if (IsAlreadyBuildingOnLayer(t_mapX, t_mapY, building, LayerType::BUILDINGS))
    {
        return;
    }

    // nothing was built on the coast
    if (IsBuildingOnWaterOrCoast(t_mapX, t_mapY, building))
    {
        return;
    }

    // remove
    for (auto y{ 0 }; y < building.size.h; ++y)
    {
        for (auto x{ 0 }; x < building.size.w; ++x)
        {
            // get terrain tile
            const auto& terrainTile{ GetLayer(LayerType::TERRAIN).GetTile(t_mapX + x, t_mapY + y) };

            // skip removing if the components do not exist
            if (!Game::ecs.all_of<ecs::BuildingsLayerTileComponent, ecs::BuildingUpdatedComponent>(terrainTile.entity))
            {
                continue;
            }

            // remove BuildingsLayerTileComponent from the terrain tile entity
            Game::ecs.remove<ecs::BuildingsLayerTileComponent>(terrainTile.entity);

            // remove BuildingUpdatedComponent from the terrain tile entity
            Game::ecs.remove<ecs::BuildingUpdatedComponent>(terrainTile.entity);
        }
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::map::MapContent::CreateContent(const std::string& t_filePath)
{
    Log::MDCII_LOG_DEBUG("[MapContent::CreateContent()] Start reading Json data...");

    nlohmann::json j = ReadJsonFromFile(Game::RESOURCES_REL_PATH + t_filePath);

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

void mdcii::map::MapContent::InitBauhausZoom()
{
    // check for History Ed.
    if (context->originalResourcesManager->bauhausBshFiles.size() == 1)
    {
        Log::MDCII_LOG_DEBUG("[MapContent::InitBauhausZoom()] History Ed.: Sets the thumbnail zoom for buildings to GFX.");

        bauhausZoom = Zoom::GFX;
        return;
    }

    // Nina has 3 Bauhaus.bsh files
    const auto zoomOptional{ magic_enum::enum_cast<Zoom>(Game::INI.Get<std::string>("main_menu", "thumbnails_zoom")) };
    if (zoomOptional.has_value())
    {
        bauhausZoom = zoomOptional.value();
    }
    else
    {
        bauhausZoom = Zoom::GFX;
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

bool mdcii::map::MapContent::IsBuildingOutsideTheMap(const int t_mapX, const int t_mapY, const data::Building& t_building) const
{
    for (auto y{ 0 }; y < t_building.size.h; ++y)
    {
        for (auto x{ 0 }; x < t_building.size.w; ++x)
        {
            if (!IsPositionInMap(t_mapX + x, t_mapY + y))
            {
                return true;
            }
        }
    }

    return false;
}

bool mdcii::map::MapContent::IsAlreadyBuildingOnLayer(const int t_mapX, const int t_mapY, const data::Building& t_building, const LayerType t_layerType) const
{
    for (auto y{ 0 }; y < t_building.size.h; ++y)
    {
        for (auto x{ 0 }; x < t_building.size.w; ++x)
        {
            const auto& tile{ GetLayer(t_layerType).GetTile(t_mapX + x, t_mapY + y) };
            if (tile.HasBuilding())
            {
                return true;
            }
        }
    }

    return false;
}

bool mdcii::map::MapContent::IsBuildingOnWaterOrCoast(const int t_mapX, const int t_mapY, const data::Building& t_building) const
{
    for (auto y{ 0 }; y < t_building.size.h; ++y)
    {
        for (auto x{ 0 }; x < t_building.size.w; ++x)
        {
            const auto& terrainTile{ GetLayer(LayerType::TERRAIN).GetTile(t_mapX + x, t_mapY + y) };
            const auto& terrainBuilding{ context->originalResourcesManager->GetBuildingById(terrainTile.buildingId) };
            if (terrainBuilding.posoffs == 0)
            {
                return true;
            }
        }
    }

    return false;
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

        layer->SortMapTiles();

        // todo: tmp code for instancing
        if (layer->layerType == LayerType::TERRAIN)
        {
            layer->CreateModelMatrices();
        }
    }
}

void mdcii::map::MapContent::PreCalcTile(MapTile& t_mapTile, const int t_mapX, const int t_mapY) const
{
    // set layer/map position
    t_mapTile.mapX = t_mapX;
    t_mapTile.mapY = t_mapY;

    // pre-calculate the position on the screen for each zoom and each rotation
    magic_enum::enum_for_each<Zoom>([&](const Zoom t_zoom) {
        std::array<glm::vec2, NR_OF_ROTATIONS> positions{};

        positions[0] = MapToScreen(t_mapX, t_mapY, t_zoom, Rotation::DEG0);
        positions[1] = MapToScreen(t_mapX, t_mapY, t_zoom, Rotation::DEG90);
        positions[2] = MapToScreen(t_mapX, t_mapY, t_zoom, Rotation::DEG180);
        positions[3] = MapToScreen(t_mapX, t_mapY, t_zoom, Rotation::DEG270);

        t_mapTile.screenPositions.at(magic_enum::enum_integer(t_zoom)) = positions;
    });

    // pre-calculate the index for each rotation for sorting
    t_mapTile.indices[0] = GetMapIndex(t_mapX, t_mapY, Rotation::DEG0);
    t_mapTile.indices[1] = GetMapIndex(t_mapX, t_mapY, Rotation::DEG90);
    t_mapTile.indices[2] = GetMapIndex(t_mapX, t_mapY, Rotation::DEG180);
    t_mapTile.indices[3] = GetMapIndex(t_mapX, t_mapY, Rotation::DEG270);

    // pre-calculate a gfx for each rotation
    if (t_mapTile.HasBuilding())
    {
        const auto building{ context->originalResourcesManager->GetBuildingById(t_mapTile.buildingId) };
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
