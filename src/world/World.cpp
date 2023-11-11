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
    if (deepWater->currentTiles.empty() && m_flag)
    {
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
    MDCII_LOG_DEBUG("[World::Init()] Init World.");

    if (resource::MdciiFile file{ t_fileName }; file.LoadJsonFromFile())
    {
        islands = file.CreateWorldContent(worldWidth, worldHeight);
    }

    for (auto const& island : islands)
    {
        island->InitTiles(gameState->game);
    }

    deepWater = CreateDeepWaterArea();
    renderer = std::make_unique<renderer::Renderer>(this);
    camera = std::make_unique<camera::Camera>(this);
}

std::unique_ptr<mdcii::world::DeepWater> mdcii::world::World::CreateDeepWaterArea() const
{
    auto deepWaterArea{ std::make_unique<DeepWater>() };
    deepWaterArea->width = worldWidth;
    deepWaterArea->height = worldHeight;
    deepWaterArea->tiles.resize(worldWidth * worldHeight);

    for (auto y{ 0 }; y < worldHeight; ++y)
    {
        for (auto x{ 0 }; x < worldWidth; ++x)
        {
            if (!IsWorldPositionOnAnyIsland(x, y))
            {
                deepWaterArea->tiles.at(y * worldWidth + x).buildingId = 1201;
            }
        }
    }
    deepWaterArea->InitTiles(gameState->game);

    return deepWaterArea;
}

//-------------------------------------------------
// Render helper
//-------------------------------------------------

void mdcii::world::World::FindVisibleIslands()
{
    std::vector<Island*>().swap(currentIslands);
    size_t c{ 0 };

    // todo: it makes sense to sort out the islands beforehand that are completely invisible
    for (const auto& island : islands)
    {
        std::vector<Tile>().swap(island->currentTiles);
        island->currentTiles = island->sortedTiles.at(magic_enum::enum_integer(camera->rotation));

        auto [begin, end]{ std::ranges::remove_if(island->currentTiles, [&](const Tile& t_tile)
        {
            return IsWorldPositionOutsideScreen(t_tile.posX + island->startX, t_tile.posY + island->startY);
        }) };
        island->currentTiles.erase(begin, end);
        c += island->currentTiles.size();

        if (!island->currentTiles.empty())
        {
            currentIslands.push_back(island.get());
        }
    }

    MDCII_LOG_DEBUG("[World::FindVisibleIslands()] Render {} islands.", currentIslands.size());
    MDCII_LOG_DEBUG("[World::FindVisibleIslands()] Render {} island tiles.", c);
}

void mdcii::world::World::FindVisibleDeepWaterTiles() const
{
    const auto shouldBeRemoved = [&](const Tile& t_tile)
    {
        return IsWorldPositionOutsideScreen(t_tile.posX, t_tile.posY);
    };

    std::vector<Tile>().swap(deepWater->currentTiles);
    deepWater->currentTiles = deepWater->sortedTiles.at(magic_enum::enum_integer(camera->rotation));

    auto [begin, end]{ std::ranges::remove_if(deepWater->currentTiles, shouldBeRemoved) };
    deepWater->currentTiles.erase(begin, end);

    MDCII_LOG_DEBUG("[World::FindVisibleDeepWaterTiles()] Render {} deep water tiles.", deepWater->currentTiles.size());
}
