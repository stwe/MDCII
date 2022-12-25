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
#include "WorldLayer.h"
#include "state/State.h"
#include "file/OriginalResourcesManager.h"
#include "renderer/RenderUtils.h"
#include "world/World.h"
#include "world/Terrain.h"
#include "ogl/buffer/Ssbo.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::layer::WorldLayer::WorldLayer(std::shared_ptr<state::Context> t_context, world::World* t_world)
    : GameLayer(std::move(t_context), t_world)
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::WorldLayer()] Create WorldLayer.");

    width = m_world->width;
    height = m_world->height;
}

mdcii::layer::WorldLayer::~WorldLayer() noexcept
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::~WorldLayer()] Destruct WorldLayer.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::layer::WorldLayer::CreateTiles()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::CreateTiles()] Create and prepare Tile objects for rendering.");

    for (auto worldY{ 0 }; worldY < m_world->height; ++worldY)
    {
        for (auto worldX{ 0 }; worldX < m_world->width; ++worldX)
        {
            if (!m_world->terrain->IsPositionOnAnIsland(glm::ivec2(worldX, worldY)))
            {
                auto tile{ std::make_unique<Tile>() };

                tile->worldXDeg0 = worldX;
                tile->worldYDeg0 = worldY;
                tile->buildingId = WATER_BUILDING_ID;
                tile->rotation = world::Rotation::DEG0;

                // pre-calculate the position on the screen for each zoom and each rotation
                magic_enum::enum_for_each<world::Zoom>([this, &tile, &worldX, &worldY](const world::Zoom t_zoom) {
                    std::array<glm::vec2, world::NR_OF_ROTATIONS> positions{};

                    positions[0] = m_world->WorldToScreen(worldX, worldY, t_zoom, world::Rotation::DEG0);
                    positions[1] = m_world->WorldToScreen(worldX, worldY, t_zoom, world::Rotation::DEG90);
                    positions[2] = m_world->WorldToScreen(worldX, worldY, t_zoom, world::Rotation::DEG180);
                    positions[3] = m_world->WorldToScreen(worldX, worldY, t_zoom, world::Rotation::DEG270);

                    tile->screenPositions.at(magic_enum::enum_integer(t_zoom)) = positions;
                });

                // pre-calculate the index for each rotation for sorting
                tile->indices[0] = GetMapIndex(worldX, worldY, world::Rotation::DEG0);
                tile->indices[1] = GetMapIndex(worldX, worldY, world::Rotation::DEG90);
                tile->indices[2] = GetMapIndex(worldX, worldY, world::Rotation::DEG180);
                tile->indices[3] = GetMapIndex(worldX, worldY, world::Rotation::DEG270);

                tiles.emplace_back(std::move(tile));
            }
        }
    }

    MDCII_ASSERT(!tiles.empty(), "[WorldLayer::CreateTiles()] Missing Tile objects.")

    instancesToRender = static_cast<int32_t>(tiles.size());
}

void mdcii::layer::WorldLayer::SortTiles()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::SortTiles()] Sorting Tile objects by index.");

    MDCII_ASSERT(!tiles.empty(), "[WorldLayer::SortTiles()] Missing Tile objects.")

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

void mdcii::layer::WorldLayer::CreateModelMatricesContainer()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::CreateModelMatricesContainer()] Create model matrices container.");

    MDCII_ASSERT(modelMatrices.at(0).at(0).empty(), "[WorldLayer::CreateModelMatricesContainer()] Invalid model matrices container size.")
    MDCII_ASSERT(!sortedTiles.empty(), "[WorldLayer::CreateModelMatricesContainer()] Missing Tile objects.")

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

    MDCII_ASSERT(instanceIds.empty(), "[WorldLayer::CreateModelMatricesContainer()] Invalid Instance Ids map size.")

    // create a hashmap to fast find the instance ID for each position
    magic_enum::enum_for_each<world::Rotation>([this](const world::Rotation t_rotation) {
        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };
        for (const auto& tile : sortedTiles.at(rotationInt))
        {
            instanceIds.emplace(glm::ivec3(tile->worldXDeg0, tile->worldYDeg0, rotationInt), tile->instanceIds.at(rotationInt));
        }
    });
}

void mdcii::layer::WorldLayer::CreateGfxNumbersContainer()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::CreateGfxNumbersContainer()] Create gfx numbers container.");

    MDCII_ASSERT(gfxNumbers.empty(), "[WorldLayer::CreateGfxNumbersContainer()] Invalid gfx numbers container size.")
    MDCII_ASSERT(instancesToRender > 0, "[WorldLayer::CreateGfxNumbersContainer()] Invalid number of instances.")
    MDCII_ASSERT(!sortedTiles.empty(), "[WorldLayer::CreateGfxNumbersContainer()] Missing Tile objects.")

    std::vector<glm::ivec4> gfxs(instancesToRender, glm::ivec4(-1));

    magic_enum::enum_for_each<world::Rotation>([this, &gfxs](const world::Rotation t_rotation) {
        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

        auto instance{ 0 };
        for (const auto& tile : sortedTiles.at(rotationInt))
        {
            if (tile->HasBuilding())
            {
                gfxs.at(instance)[rotationInt] = WATER_GFX;
            }

            instance++;
        }
    });

    gfxNumbers = gfxs;
}

void mdcii::layer::WorldLayer::CreateBuildingIdsContainer()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::CreateBuildingIdsContainer()] Create Building Ids container.");

    MDCII_ASSERT(buildingIds.empty(), "[WorldLayer::CreateBuildingIdsContainer()] Invalid Building Ids container size.")
    MDCII_ASSERT(instancesToRender > 0, "[WorldLayer::CreateBuildingIdsContainer()] Invalid number of instances.")
    MDCII_ASSERT(!sortedTiles.empty(), "[WorldLayer::CreateBuildingIdsContainer()] Missing Tile objects.")

    std::vector<glm::ivec4> ids(instancesToRender, glm::ivec4(-1));

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

void mdcii::layer::WorldLayer::StoreGfxNumbersInGpu()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::StoreGfxNumbersInGpu()] Store gfx numbers container in Gpu memory.");

    MDCII_ASSERT(!gfxNumbers.empty(), "[WorldLayer::StoreGfxNumbersInGpu()] Invalid gfx numbers container size.")
    MDCII_ASSERT(!gfxNumbersSsbo, "[WorldLayer::StoreGfxNumbersInGpu()] Invalid gfx numbers Ssbo pointer.")

    gfxNumbersSsbo = std::make_unique<ogl::buffer::Ssbo>("GfxNumbers_Ssbo");
    gfxNumbersSsbo->Bind();
    ogl::buffer::Ssbo::StoreData(static_cast<uint32_t>(gfxNumbers.size()) * sizeof(glm::ivec4), gfxNumbers.data());
    ogl::buffer::Ssbo::Unbind();
}

void mdcii::layer::WorldLayer::StoreBuildingIdsInGpu()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::StoreBuildingIdsInGpu()] Store Building Ids container in Gpu memory.");

    MDCII_ASSERT(!buildingIds.empty(), "[WorldLayer::StoreBuildingIdsInGpu()] Invalid Building Ids container size.")
    MDCII_ASSERT(!buildingIdsSsbo, "[WorldLayer::StoreBuildingIdsInGpu()] Invalid Building Ids Ssbo pointer.")

    buildingIdsSsbo = std::make_unique<ogl::buffer::Ssbo>("BuildingIds_Ssbo");
    buildingIdsSsbo->Bind();
    ogl::buffer::Ssbo::StoreData(static_cast<uint32_t>(buildingIds.size()) * sizeof(glm::ivec4), buildingIds.data());
    ogl::buffer::Ssbo::Unbind();
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

glm::mat4 mdcii::layer::WorldLayer::CreateModelMatrix(const layer::Tile& t_tile, const world::Zoom t_zoom, const world::Rotation t_rotation) const
{
    const auto& stadtfldBshTextures{ m_context->originalResourcesManager->GetStadtfldBshByZoom(t_zoom) };
    const auto w{ static_cast<float>(stadtfldBshTextures[WATER_GFX]->width) };
    const auto h{ static_cast<float>(stadtfldBshTextures[WATER_GFX]->height) };

    auto screenPosition{ t_tile.screenPositions.at(magic_enum::enum_integer(t_zoom)).at(magic_enum::enum_integer(t_rotation)) };
    screenPosition.y -= h - static_cast<float>(get_tile_height(t_zoom));

    return renderer::RenderUtils::GetModelMatrix(screenPosition, { w, h });
}
