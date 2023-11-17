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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#include "World.h"
#include "Game.h"
#include "Island.h"
#include "Layer.h"
#include "DeepWater.h"
#include "MdciiAssert.h"
#include "GameState.h"
#include "resource/MdciiFile.h"
#include "renderer/Renderer.h"
#include "camera/Camera.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::World::World(GameState* t_gameState, const std::string& t_fileName)
    : gameState{ t_gameState }
{
    MDCII_LOG_DEBUG("[World::World()] Create World.");

    MDCII_ASSERT(gameState, "[World::World()] Null pointer.")

    Init(t_fileName);
}

mdcii::world::World::~World() noexcept
{
    MDCII_LOG_DEBUG("[World::~World()] Destruct World.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::World::OnUserUpdate(const float t_elapsedTime)
{
    // first access
    if (m_flag)
    {
        MDCII_LOG_DEBUG("[World::OnUserUpdate()] First world update.");

        MDCII_ASSERT(deepWater->layer->currentTiles.empty(), "[World::OnUserUpdate()] Invalid number of tiles.")

        FindVisibleIslands();
        FindVisibleDeepWaterTiles();

        m_flag = false;
    }

    // on key pressed
    if (camera->OnUserUpdate(t_elapsedTime) && !m_flag)
    {
        FindVisibleIslands();
        FindVisibleDeepWaterTiles();
    }

    renderer->RenderDeepWater();
    renderer->RenderIslands();
}

//-------------------------------------------------
// World / screen positions
//-------------------------------------------------

olc::vi2d mdcii::world::World::ToScreen(const int t_x, const int t_y) const
{
    const auto position{ rotate_position(t_x, t_y, worldWidth, worldHeight, camera->rotation) };

    return olc::vi2d{
        camera->screenPosition.x + (position.x - position.y) * get_tile_width_half(camera->zoom),
        camera->screenPosition.y + (position.x + position.y) * get_tile_height_half(camera->zoom)
    };
}

bool mdcii::world::World::IsWorldPositionOnAnyIsland(const int t_x, const int t_y) const
{
    return std::ranges::any_of(islands, [t_x, t_y](auto const& t_island)
    {
        return physics::Aabb::PointVsAabb({ t_x, t_y }, t_island->aabb);
    });
}

bool mdcii::world::World::IsWorldPositionOutsideScreen(const int t_x, const int t_y) const
{
    const auto screenPosition{ ToScreen(t_x, t_y) };
    return screenPosition.x > Game::INI.Get<int>("window", "width") + get_tile_width(camera->zoom) ||
           screenPosition.x < -get_tile_width_half(camera->zoom) ||
           screenPosition.y > Game::INI.Get<int>("window", "height") + get_tile_height(camera->zoom) ||
           screenPosition.y < -get_tile_height_half(camera->zoom);
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::World::Init(const std::string& t_fileName)
{
    MDCII_LOG_DEBUG("[World::Init()] Start initializing the world ...");

    if (resource::MdciiFile file{ t_fileName }; file.LoadJsonFromFile())
    {
        islands = file.CreateWorldContent(worldWidth, worldHeight);
    }

    for (auto const& island : islands)
    {
        island->Init(gameState->game);
    }

    deepWater = CreateDeepWaterArea();
    renderer = std::make_unique<renderer::Renderer>(this);
    camera = std::make_unique<camera::Camera>(this);

    MDCII_LOG_DEBUG("[World::Init()] The world were initialized successfully.");
}

std::unique_ptr<mdcii::world::DeepWater> mdcii::world::World::CreateDeepWaterArea() const
{
    MDCII_LOG_DEBUG("[World::CreateDeepWaterArea()] Initialize the deep water area.");

    auto deepWaterArea{ std::make_unique<DeepWater>() };
    deepWaterArea->width = worldWidth;
    deepWaterArea->height = worldHeight;
    deepWaterArea->layer = std::make_unique<Layer>(LayerType::DEEP_WATER);
    deepWaterArea->layer->tiles.resize(worldWidth * worldHeight);

    for (auto y{ 0 }; y < worldHeight; ++y)
    {
        for (auto x{ 0 }; x < worldWidth; ++x)
        {
            if (!IsWorldPositionOnAnyIsland(x, y))
            {
                deepWaterArea->layer->tiles.at(y * worldWidth + x).buildingId = DEEP_WATER_BUILDING_ID;
            }
        }
    }

    deepWaterArea->Init(gameState->game);

    return deepWaterArea;
}

//-------------------------------------------------
// Render helper
//-------------------------------------------------

// todo: it makes sense to sort out the islands beforehand that are completely invisible
void mdcii::world::World::FindVisibleIslands()
{
    std::vector<Island*>().swap(currentIslands);

    // todo
    for (const auto& island : islands)
    {
        /*
        for (auto i{ 0 }; i < Island::NR_OF_LAYERS - 1; ++i)
        {
            std::vector<Tile>().swap(island->layers[i]->currentTiles);
            island->layers[i]->currentTiles = island->layers[i]->sortedTiles.at(magic_enum::enum_integer(camera->rotation));

            auto [begin, end]{ std::ranges::remove_if(island->layers[i]->currentTiles, [&](const Tile& t_tile)
            {
                return IsWorldPositionOutsideScreen(t_tile.posX + island->startX, t_tile.posY + island->startY);
            }) };

            island->layers[i]->currentTiles.erase(begin, end);
        }
        */

        const auto mixedLayerInt{ magic_enum::enum_integer(LayerType::TERRAIN_BUILDINGS) };

        std::vector<Tile>().swap(island->layers[mixedLayerInt]->currentTiles);
        island->layers[mixedLayerInt]->currentTiles = island->layers[mixedLayerInt]->sortedTiles.at(magic_enum::enum_integer(camera->rotation));

        auto [begin, end]{ std::ranges::remove_if(island->layers[mixedLayerInt]->currentTiles, [&](const Tile& t_tile)
        {
            return IsWorldPositionOutsideScreen(t_tile.posX + island->startX, t_tile.posY + island->startY);
        }) };

        island->layers[mixedLayerInt]->currentTiles.erase(begin, end);

        if (!island->layers[mixedLayerInt]->currentTiles.empty())
        {
            currentIslands.push_back(island.get());
        }
    }

    MDCII_LOG_DEBUG("[World::FindVisibleIslands()] Render {} islands.", currentIslands.size());
}

void mdcii::world::World::FindVisibleDeepWaterTiles() const
{
    const auto shouldBeRemoved = [&](const Tile& t_tile)
    {
        return IsWorldPositionOutsideScreen(t_tile.posX, t_tile.posY);
    };

    std::vector<Tile>().swap(deepWater->layer->currentTiles);
    deepWater->layer->currentTiles = deepWater->layer->sortedTiles.at(magic_enum::enum_integer(camera->rotation));

    auto [begin, end]{ std::ranges::remove_if(deepWater->layer->currentTiles, shouldBeRemoved) };
    deepWater->layer->currentTiles.erase(begin, end);

    MDCII_LOG_DEBUG("[World::FindVisibleDeepWaterTiles()] Render {} deep water tiles.", deepWater->layer->currentTiles.size());
}
