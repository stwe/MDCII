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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#include <glm/gtx/hash.hpp>
#include "TerrainLayer.h"
#include "state/State.h"
#include "world/Island.h"
#include "world/World.h"
#include "file/OriginalResourcesManager.h"
#include "ogl/buffer/Ssbo.h"
#include "renderer/RenderUtils.h"
#include "data/BuildingIds.h"

void mdcii::layer::to_json(nlohmann::json& t_json, const std::shared_ptr<Tile>& t_tile)
{
    if (t_tile)
    {
        t_json = nlohmann::json{
            { "id", t_tile->buildingId },
            { "rotation", magic_enum::enum_integer(t_tile->rotation) },
            { "x", t_tile->x },
            { "y", t_tile->y },
            { "connected", t_tile->connectedTiles }
        };
    }
    else
    {
        t_json = nullptr;
    }
}

void mdcii::layer::from_json(const nlohmann::json& t_json, Tile& t_tile)
{
    if (t_json.count("id"))
    {
        t_json.at("id").get_to(t_tile.buildingId);
    }

    if (t_json.count("rotation"))
    {
        auto r{ 0 };
        t_json.at("rotation").get_to(r);
        t_tile.rotation = world::int_to_rotation(r);
    }

    if (t_json.count("x"))
    {
        t_json.at("x").get_to(t_tile.x);
    }

    if (t_json.count("y"))
    {
        t_json.at("y").get_to(t_tile.y);
    }

    if (t_json.count("connected"))
    {
        t_json.at("connected").get_to(t_tile.connectedTiles);
    }
}

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::layer::TerrainLayer::TerrainLayer(
    std::shared_ptr<state::Context> t_context,
    world::World* t_world,
    world::Island* t_island,
    const LayerType t_layerType
)
    : GameLayer(std::move(t_context), t_world)
    , m_island{ t_island }
{
    Log::MDCII_LOG_DEBUG("[TerrainLayer::TerrainLayer()] Create TerrainLayer of type {}.", magic_enum::enum_name(t_layerType));
    layerType = t_layerType;

    MDCII_ASSERT(m_island, "[TerrainLayer::TerrainLayer()] Null pointer.")

    width = m_island->width;
    height = m_island->height;

    MDCII_ASSERT(width > 0, "[TerrainLayer::TerrainLayer()] Invalid width.")
    MDCII_ASSERT(height > 0, "[TerrainLayer::TerrainLayer()] Invalid height.")

    instancesToRender = width * height;
}

mdcii::layer::TerrainLayer::~TerrainLayer() noexcept
{
    Log::MDCII_LOG_DEBUG("[TerrainLayer::~TerrainLayer()] Destruct TerrainLayer.");
}

//-------------------------------------------------
// Tiles
//-------------------------------------------------

void mdcii::layer::TerrainLayer::CreateTilesFromJson(const nlohmann::json& t_json)
{
    Log::MDCII_LOG_DEBUG("[TerrainLayer::CreateTilesFromJson()] Create Tile objects from Json.");

    for (const auto& [k, v] : t_json.items())
    {
        AddTileFromJson(v);
    }

    MDCII_ASSERT(!tiles.empty(), "[TerrainLayer::CreateTilesFromJson()] Missing Tile objects.")
    MDCII_ASSERT(instancesToRender == static_cast<int32_t>(tiles.size()), "[TerrainLayer::CreateTilesFromJson()] Invalid map data.")
}

const mdcii::layer::Tile& mdcii::layer::TerrainLayer::GetTile(const int32_t t_x, const int32_t t_y) const
{
    return *tiles.at(GetMapIndex(t_x, t_y, world::Rotation::DEG0));
}

const mdcii::layer::Tile& mdcii::layer::TerrainLayer::GetTile(const glm::ivec2& t_position) const
{
    return GetTile(t_position.x, t_position.y);
}

mdcii::layer::Tile& mdcii::layer::TerrainLayer::GetTile(const glm::ivec2& t_position)
{
    return *tiles.at(GetMapIndex(t_position, world::Rotation::DEG0));
}

void mdcii::layer::TerrainLayer::ResetTilePointersAt(const std::array<int32_t, world::NR_OF_ROTATIONS>& t_instanceIds)
{
    const auto id0{ t_instanceIds.at(magic_enum::enum_integer(world::Rotation::DEG0)) };
    MDCII_ASSERT(tiles.at(id0), "[TerrainLayer::ResetTilePointersAt()] Null pointer.")
    tiles.at(id0).reset();

    magic_enum::enum_for_each<world::Rotation>([this, &t_instanceIds](const world::Rotation t_rotation) {
        const auto r{ magic_enum::enum_integer(t_rotation) };
        MDCII_ASSERT(sortedTiles.at(r).at(t_instanceIds.at(r)), "[TerrainLayer::ResetTilePointersAt()] Null pointer.")
        sortedTiles.at(r).at(t_instanceIds.at(r)).reset();
    });
}

void mdcii::layer::TerrainLayer::StoreTile(std::unique_ptr<Tile> t_tile)
{
    const auto id0{ t_tile->instanceIds.at(magic_enum::enum_integer(world::Rotation::DEG0)) };
    MDCII_ASSERT(!tiles.at(id0), "[TerrainLayer::StoreTile()] Invalid pointer.")
    tiles.at(id0) = std::move(t_tile);

    magic_enum::enum_for_each<world::Rotation>([this, &id0](const world::Rotation t_rotation) {
        const auto r{ magic_enum::enum_integer(t_rotation) };
        MDCII_ASSERT(!sortedTiles.at(r).at(tiles.at(id0)->instanceIds.at(r)), "[TerrainLayer::StoreTile()] Invalid pointer.")
        sortedTiles.at(r).at(tiles.at(id0)->instanceIds.at(r)) = tiles.at(id0);
    });
}

void mdcii::layer::TerrainLayer::PreCalcTile(Tile& t_tile) const
{
    // pre-calculate the position on the screen for each zoom and each rotation
    magic_enum::enum_for_each<world::Zoom>([this, &t_tile](const world::Zoom t_zoom) {
        std::array<glm::vec2, world::NR_OF_ROTATIONS> positions{};

        positions[0] = m_world->WorldToScreen(t_tile.worldXDeg0, t_tile.worldYDeg0, t_zoom, world::Rotation::DEG0);
        positions[1] = m_world->WorldToScreen(t_tile.worldXDeg0, t_tile.worldYDeg0, t_zoom, world::Rotation::DEG90);
        positions[2] = m_world->WorldToScreen(t_tile.worldXDeg0, t_tile.worldYDeg0, t_zoom, world::Rotation::DEG180);
        positions[3] = m_world->WorldToScreen(t_tile.worldXDeg0, t_tile.worldYDeg0, t_zoom, world::Rotation::DEG270);

        t_tile.screenPositions.at(magic_enum::enum_integer(t_zoom)) = positions;
    });

    // pre-calculate the index for each rotation for sorting
    t_tile.indices[0] = GetMapIndex(t_tile.islandXDeg0, t_tile.islandYDeg0, world::Rotation::DEG0);
    t_tile.indices[1] = GetMapIndex(t_tile.islandXDeg0, t_tile.islandYDeg0, world::Rotation::DEG90);
    t_tile.indices[2] = GetMapIndex(t_tile.islandXDeg0, t_tile.islandYDeg0, world::Rotation::DEG180);
    t_tile.indices[3] = GetMapIndex(t_tile.islandXDeg0, t_tile.islandYDeg0, world::Rotation::DEG270);

    // pre-calculate a gfx for each rotation
    if (t_tile.HasBuilding())
    {
        const auto building{ m_context->originalResourcesManager->GetBuildingById(t_tile.buildingId) };
        const auto gfx0{ building.gfx };

        t_tile.gfxs.push_back(gfx0);
        if (building.rotate > 0)
        {
            t_tile.gfxs.push_back(gfx0 + (1 * building.rotate));
            t_tile.gfxs.push_back(gfx0 + (2 * building.rotate));
            t_tile.gfxs.push_back(gfx0 + (3 * building.rotate));
        }
    }
}

void mdcii::layer::TerrainLayer::PreCalcTile(Tile& t_tile, const int32_t t_x, const int32_t t_y) const
{
    // set island position of the tile for Deg0
    t_tile.islandXDeg0 = t_x;
    t_tile.islandYDeg0 = t_y;

    // set world position of the tile for Deg0
    t_tile.worldXDeg0 = m_island->startWorldX + t_tile.islandXDeg0;
    t_tile.worldYDeg0 = m_island->startWorldY + t_tile.islandYDeg0;

    PreCalcTile(t_tile);
}

int32_t mdcii::layer::TerrainLayer::CalcGfx(const Tile& t_tile, const world::Rotation t_rotation) const
{
    const auto& building{ m_context->originalResourcesManager->GetBuildingById(t_tile.buildingId) };
    auto buildingRotation{ t_tile.rotation };
    if (building.rotate > 0)
    {
        buildingRotation = buildingRotation + t_rotation;
    }
    auto gfx{ t_tile.gfxs[magic_enum::enum_integer(buildingRotation)] };

    if (building.size.w > 1 || building.size.h > 1)
    {
        // default: orientation 0
        auto rp{ glm::ivec2(t_tile.x, t_tile.y) };

        if (t_tile.rotation == world::Rotation::DEG270)
        {
            rp = rotate_position(
                t_tile.x, t_tile.y,
                building.size.w, building.size.h,
                world::Rotation::DEG90
            );
        }

        if (t_tile.rotation == world::Rotation::DEG180)
        {
            rp = rotate_position(
                t_tile.x, t_tile.y,
                building.size.w, building.size.h,
                world::Rotation::DEG180
            );
        }

        if (t_tile.rotation == world::Rotation::DEG90)
        {
            rp = rotate_position(
                t_tile.x, t_tile.y,
                building.size.w, building.size.h,
                world::Rotation::DEG270
            );
        }

        const auto offset{ rp.y * building.size.w + rp.x };
        gfx += offset;
    }

    return gfx;
}

glm::mat4 mdcii::layer::TerrainLayer::CreateModelMatrix(const Tile& t_tile, const world::Zoom t_zoom, const world::Rotation t_rotation) const
{
    // to definitely create a screen position
    int32_t gfx{ GRASS_GFX };
    auto posoffs{ m_context->originalResourcesManager->GetBuildingById(data::GRASS_BUILDING_ID).posoffs };

    // override gfx && posoffs from above
    if (t_tile.HasBuilding())
    {
        // todo: Daten aus dem Tile (pointer) von sortedTiles nehmen?
        gfx = CalcGfx(t_tile, t_rotation);
        posoffs = m_context->originalResourcesManager->GetBuildingById(t_tile.buildingId).posoffs;
    }

    // get width && height
    const auto& stadtfldBshTextures{ m_context->originalResourcesManager->GetStadtfldBshByZoom(t_zoom) };
    const auto w{ static_cast<float>(stadtfldBshTextures[gfx]->width) };
    const auto h{ static_cast<float>(stadtfldBshTextures[gfx]->height) };

    // get elevation
    auto elevation{ 0.0f };
    if (posoffs > 0)
    {
        elevation = static_cast<float>(get_elevation(t_zoom));
    }

    // screen position
    auto screenPosition{ t_tile.screenPositions.at(magic_enum::enum_integer(t_zoom)).at(magic_enum::enum_integer(t_rotation)) };
    screenPosition.y -= h - static_cast<float>(get_tile_height(t_zoom));
    screenPosition.y -= elevation;

    // calc model matrix
    auto mat{ renderer::RenderUtils::GetModelMatrix(screenPosition, { w, h }) };

    // update min/max screen positions
    if (layerType == LayerType::COAST)
    {
        const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

        if (mat[3].x > m_island->max.at(zoomInt).at(rotationInt).x)
        {
            m_island->max.at(zoomInt).at(rotationInt).x = mat[3].x;
        }
        if (mat[3].y > m_island->max.at(zoomInt).at(rotationInt).y)
        {
            m_island->max.at(zoomInt).at(rotationInt).y = mat[3].y;
        }

        if (mat[3].x < m_island->min.at(zoomInt).at(rotationInt).x)
        {
            m_island->min.at(zoomInt).at(rotationInt).x = mat[3].x;
        }
        if (mat[3].y < m_island->min.at(zoomInt).at(rotationInt).y)
        {
            m_island->min.at(zoomInt).at(rotationInt).y = mat[3].y;
        }
    }

    return mat;
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::layer::TerrainLayer::CreateTiles()
{
    Log::MDCII_LOG_DEBUG("[TerrainLayer::CreateTiles()] Prepare Tile objects for rendering.");

    MDCII_ASSERT(width > 0, "[TerrainLayer::CreateTiles()] Invalid width.")
    MDCII_ASSERT(height > 0, "[TerrainLayer::CreateTiles()] Invalid height.")
    MDCII_ASSERT(!tiles.empty(), "[TerrainLayer::CreateTiles()] Missing Tile objects.")

    for (auto y{ 0 }; y < height; ++y)
    {
        for (auto x{ 0 }; x < width; ++x)
        {
            PreCalcTile(*tiles.at(GetMapIndex(x, y, world::Rotation::DEG0)), x, y);
        }
    }
}

void mdcii::layer::TerrainLayer::SortTiles()
{
    Log::MDCII_LOG_DEBUG("[TerrainLayer::SortTiles()] Sorting Tile objects by index.");

    MDCII_ASSERT(!tiles.empty(), "[TerrainLayer::SortTiles()] Missing Tile objects.")

    magic_enum::enum_for_each<world::Rotation>([this](const world::Rotation t_rotation) {
        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

        // sort tiles by index
        std::sort(tiles.begin(), tiles.end(), [&](const std::shared_ptr<Tile>& t_a, const std::shared_ptr<Tile>& t_b) {
            return t_a->indices[rotationInt] < t_b->indices[rotationInt];
        });

        // copy sorted tiles
        sortedTiles.at(rotationInt) = tiles;
    });

    // revert tiles sorting = sortedTiles DEG0
    tiles = sortedTiles.at(magic_enum::enum_integer(world::Rotation::DEG0));
}

void mdcii::layer::TerrainLayer::CreateModelMatricesContainer()
{
    Log::MDCII_LOG_DEBUG("[TerrainLayer::CreateModelMatricesContainer()] Create model matrices container.");

    MDCII_ASSERT(modelMatrices.at(0).at(0).empty(), "[TerrainLayer::CreateModelMatricesContainer()] Invalid model matrices container size.")
    MDCII_ASSERT(!sortedTiles.empty(), "[TerrainLayer::CreateModelMatricesContainer()] Missing Tile objects.")

    magic_enum::enum_for_each<world::Zoom>([this](const world::Zoom t_zoom) {

        Model_Matrices_For_Each_Rotation matricesForRotations;
        magic_enum::enum_for_each<world::Rotation>([this, &t_zoom, &matricesForRotations](const world::Rotation t_rotation) {
            const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

            std::vector<glm::mat4> matrices;
            int32_t instance{ 0 };
            for (const auto& tile : sortedTiles.at(rotationInt))
            {
                matrices.emplace_back(CreateModelMatrix(*tile, t_zoom, t_rotation));
                tile->instanceIds.at(rotationInt) = instance;

                instance++;
            }

            matricesForRotations.at(rotationInt) = matrices;
        });

        modelMatrices.at(magic_enum::enum_integer(t_zoom)) = matricesForRotations;
    });

    MDCII_ASSERT(instanceIds.empty(), "[TerrainLayer::CreateModelMatricesContainer()] Invalid Instance Ids map size.")

    // create a hashmap to fast find the instance ID for each position
    magic_enum::enum_for_each<world::Rotation>([this](const world::Rotation t_rotation) {
        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };
        for (const auto& tile : sortedTiles.at(rotationInt))
        {
            instanceIds.emplace(glm::ivec3(tile->worldXDeg0, tile->worldYDeg0, rotationInt), tile->instanceIds.at(rotationInt));
        }
    });
}

void mdcii::layer::TerrainLayer::CreateGfxNumbersContainer()
{
    Log::MDCII_LOG_DEBUG("[TerrainLayer::CreateGfxNumbersContainer()] Create gfx numbers container.");

    MDCII_ASSERT(gfxNumbers.empty(), "[TerrainLayer::CreateGfxNumbersContainer()] Invalid gfx numbers container size.")
    MDCII_ASSERT(instancesToRender > 0, "[TerrainLayer::CreateGfxNumbersContainer()] Invalid number of instances.")
    MDCII_ASSERT(!sortedTiles.empty(), "[TerrainLayer::CreateGfxNumbersContainer()] Missing Tile objects.")

    std::vector gfxs(instancesToRender, glm::ivec4(-1));

    magic_enum::enum_for_each<world::Rotation>([this, &gfxs](const world::Rotation t_rotation) {
        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

        auto instance{ 0 };
        for (const auto& tile : sortedTiles.at(rotationInt))
        {
            if (tile->HasBuilding())
            {
                gfxs.at(instance)[rotationInt] = CalcGfx(*tile, t_rotation);
            }

            instance++;
        }
    });

    gfxNumbers = gfxs;
}

void mdcii::layer::TerrainLayer::CreateBuildingIdsContainer()
{
    Log::MDCII_LOG_DEBUG("[TerrainLayer::CreateBuildingIdsContainer()] Create Building Ids container.");

    MDCII_ASSERT(buildingIds.empty(), "[TerrainLayer::CreateBuildingIdsContainer()] Invalid Building Ids container size.")
    MDCII_ASSERT(instancesToRender > 0, "[TerrainLayer::CreateBuildingIdsContainer()] Invalid number of instances.")
    MDCII_ASSERT(!sortedTiles.empty(), "[TerrainLayer::CreateBuildingIdsContainer()] Missing Tile objects.")

    std::vector ids(instancesToRender, glm::ivec4(-1));

    magic_enum::enum_for_each<world::Rotation>([this, &ids](const world::Rotation t_rotation) {
        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

        auto instance{ 0 };
        for (const auto& tile : sortedTiles.at(rotationInt))
        {
            if (tile->HasBuilding())
            {
                ids.at(instance)[rotationInt] = tile->buildingId;
            }

            instance++;
        }
    });

    buildingIds = ids;
}

void mdcii::layer::TerrainLayer::StoreGfxNumbersInGpu()
{
    Log::MDCII_LOG_DEBUG("[TerrainLayer::StoreGfxNumbersInGpu()] Store gfx numbers container in Gpu memory.");

    MDCII_ASSERT(!gfxNumbers.empty(), "[TerrainLayer::StoreGfxNumbersInGpu()] Invalid gfx numbers container size.")
    MDCII_ASSERT(!gfxNumbersSsbo, "[TerrainLayer::StoreGfxNumbersInGpu()] Invalid gfx numbers Ssbo pointer.")

    gfxNumbersSsbo = std::make_unique<ogl::buffer::Ssbo>("GfxNumbers_Ssbo");
    gfxNumbersSsbo->Bind();
    ogl::buffer::Ssbo::StoreData(static_cast<uint32_t>(gfxNumbers.size()) * sizeof(glm::ivec4), gfxNumbers.data());
    ogl::buffer::Ssbo::Unbind();
}

void mdcii::layer::TerrainLayer::StoreBuildingIdsInGpu()
{
    Log::MDCII_LOG_DEBUG("[TerrainLayer::StoreBuildingIdsInGpu()] Store Building Ids container in Gpu memory.");

    MDCII_ASSERT(!buildingIds.empty(), "[TerrainLayer::StoreBuildingIdsInGpu()] Invalid Building Ids container size.")
    MDCII_ASSERT(!buildingIdsSsbo, "[TerrainLayer::StoreBuildingIdsInGpu()] Invalid Building Ids Ssbo pointer.")

    buildingIdsSsbo = std::make_unique<ogl::buffer::Ssbo>("BuildingIds_Ssbo");
    buildingIdsSsbo->Bind();
    ogl::buffer::Ssbo::StoreData(static_cast<uint32_t>(buildingIds.size()) * sizeof(glm::ivec4), buildingIds.data());
    ogl::buffer::Ssbo::Unbind();
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::layer::TerrainLayer::AddTileFromJson(const nlohmann::json& t_json)
{
    tiles.emplace_back(std::make_unique<Tile>(t_json.get<Tile>()));
    tiles.back()->layerType = layerType;
}
