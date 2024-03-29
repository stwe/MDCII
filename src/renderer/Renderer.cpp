// This file is part of the MDCII project.
//
// Copyright (c) 2024. stwe <https://github.com/stwe/MDCII>
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

#include "Renderer.h"
#include "MdciiRandom.h"
#include "world/Island.h"
#include "world/DeepWater.h"
#include "world/layer/Layer.h"
#include "resource/AssetManager.h"
#include "resource/TileAtlas.h"
#include "resource/AnimalsTileAtlas.h"
#include "camera/Camera.h"

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::renderer::Renderer::RenderAsset(
    const resource::Asset t_asset,
    const int t_startX,
    const int t_startY,
    const world::World* t_world,
    const world::tile::Tile* t_tile,
    const bool t_minusElevation,
    const olc::Pixel& t_tint
)
{
    olc::vf2d screenPosition{ t_world->ToScreen(t_tile->posX + t_startX, t_tile->posY + t_startY) };
    if (t_minusElevation)
    {
        screenPosition.y -= world::ELEVATIONS[magic_enum::enum_integer(t_world->camera->zoom)];
    }

    t_world->state->game->DrawDecal(
        screenPosition,
        t_world->state->game->assetManager->GetAsset(t_asset, t_world->camera->zoom)->Decal(),
        { 1.0f, 1.0f },
        t_tint
    );
}

void mdcii::renderer::Renderer::RenderWorldParts(const world::World* t_world, const float t_elapsedTime)
{
    // update animations
    UpdateAnimations(t_elapsedTime);

    // render deep water
    if (t_world->HasRenderLayerOption(world::RenderLayer::RENDER_DEEP_WATER_LAYER))
    {
        RenderDeepWater(t_world);
    }

    // render islands
    for (auto const& island : t_world->currentIslands)
    {
        using enum world::layer::LayerType;

        if (t_world->HasRenderLayerOption(world::RenderLayer::RENDER_MIXED_LAYER))
        {
            RenderIsland(t_world, island, MIXED);
        }
        else
        {
            if (t_world->HasRenderLayerOption(world::RenderLayer::RENDER_COAST_LAYER))
            {
                RenderIsland(t_world, island, COAST);
            }
            if (t_world->HasRenderLayerOption(world::RenderLayer::RENDER_TERRAIN_LAYER))
            {
                RenderIsland(t_world, island, TERRAIN);
            }
            if (t_world->HasRenderLayerOption(world::RenderLayer::RENDER_BUILDINGS_LAYER))
            {
                RenderIsland(t_world, island, BUILDINGS);
            }
        }
    }
}

void mdcii::renderer::Renderer::RenderWorld(const world::World* t_world, const float t_elapsedTime)
{
    // update animations
    UpdateAnimations(t_elapsedTime);

    // render deep water
    RenderDeepWater(t_world);

    // render islands
    for (auto const& island : t_world->currentIslands)
    {
        for (auto& terrainTile : island->GetTerrainLayer(world::layer::LayerType::MIXED)->currentTiles)
        {
            // render terrain tile
            RenderTerrainTile(t_world, island, &terrainTile);

            // render an existing figure on top of the terrain tile
            RenderFigureOnTopOfTerrainTile(t_world, island, &terrainTile);
        }
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::renderer::Renderer::RenderIsland(const world::World* t_world, world::Island* t_island, const world::layer::LayerType t_layerType)
{
    for (auto& terrainTile : t_island->GetTerrainLayer(t_layerType)->currentTiles)
    {
        RenderTerrainTile(t_world, t_island, &terrainTile);

        if (t_world->HasRenderLayerOption(world::RenderLayer::RENDER_FIGURES_LAYER))
        {
            RenderFigureOnTopOfTerrainTile(t_world, t_island, &terrainTile);
        }
    }
}

void mdcii::renderer::Renderer::UpdateAnimations(const float t_elapsedTime)
{
    resource::TileAtlas::CalcAnimationFrame(t_elapsedTime);
    resource::AnimalsTileAtlas::CalcAnimationFrame(t_elapsedTime);
}

void mdcii::renderer::Renderer::RenderDeepWater(const world::World* t_world)
{
    for (auto& waterTile : t_world->deepWater->layer->currentTiles)
    {
        waterTile.UpdateFrame(resource::TileAtlas::frame_values);
        t_world->tileAtlas->RenderTile(0, 0, &waterTile, olc::WHITE);
        if (t_world->renderDeepWaterGrid)
        {
            RenderAsset(resource::Asset::BLUE_ISO, 0, 0, t_world, &waterTile, false);
        }
    }
}

void mdcii::renderer::Renderer::RenderFigureOnTopOfTerrainTile(
    const world::World* t_world,
    world::Island* t_island,
    const world::tile::TerrainTile* t_terrainTile
)
{
    // todo TEMP Code

    auto const& terrainTile{ t_island->GetTerrainLayer(world::layer::LayerType::TERRAIN)->GetSortedTile(t_terrainTile->posX, t_terrainTile->posY, t_world->camera->rotation) };
    auto const& buildingsTile{ t_island->GetTerrainLayer(world::layer::LayerType::BUILDINGS)->GetSortedTile(t_terrainTile->posX, t_terrainTile->posY, t_world->camera->rotation) };
    auto& figureTile{ t_island->GetFiguresLayer()->GetSortedTile(t_terrainTile->posX, t_terrainTile->posY, t_world->camera->rotation) };

    if (figureTile.HasFigure())
    {
        RenderFigureTile(t_world, t_island, &figureTile);
    }
    else if (t_island->GetFiguresLayer()->CountFigures(resource::FigureId::DEER_ID) < 3)
    {
        if (terrainTile.HasBuildingAboveWaterAndCoast() && !buildingsTile.HasBuilding())
        {
            if (mdcii_uniform_random_int<int>(1, 1000) == 1000)
            {
                MDCII_LOG_DEBUG("[Renderer::RenderFigureOnTopOfTerrainTile()] Create new deer at ({}, {}).", t_terrainTile->posX, t_terrainTile->posX);

                auto& figure{ t_world->state->game->originalResourcesManager->GetFigureById(resource::FigureId::DEER_ID) };
                figureTile.figure = &figure;
                figureTile.rotation = mdcii_uniform_random_int<int>(1, 4);
                figureTile.currentAnimation = 1;

                // todo: write back to sortedTiles and tiles

                // for count
                t_island->GetFiguresLayer()->tiles.at(t_island->GetFiguresLayer()->GetSortedIndex(t_terrainTile->posX, t_terrainTile->posY, world::Rotation::DEG0)) = figureTile;
            }
        }
    }
}

void mdcii::renderer::Renderer::RenderTerrainTile(
    const world::World* t_world,
    const world::Island* t_island,
    world::tile::TerrainTile* t_terrainTile,
    const olc::Pixel& t_tint
)
{
    if (t_terrainTile->HasBuilding())
    {
        t_terrainTile->UpdateFrame(resource::TileAtlas::frame_values);
        t_world->tileAtlas->RenderTile(t_island->startX, t_island->startY, t_terrainTile, t_tint);
        if (t_world->renderIslandsGrid)
        {
            RenderAsset(resource::Asset::GREEN_ISO, t_island->startX, t_island->startY, t_world, t_terrainTile, true);
        }
    }
}

void mdcii::renderer::Renderer::RenderFigureTile(
    const world::World* t_world,
    const world::Island* t_island,
    world::tile::FigureTile* t_figureTile
)
{
    if (t_figureTile->HasFigure())
    {
        t_figureTile->UpdateFrame(resource::AnimalsTileAtlas::frame_values);
        t_world->animalsTileAtlas->RenderTile(t_island->startX, t_island->startY, t_figureTile, olc::WHITE);
    }
}
