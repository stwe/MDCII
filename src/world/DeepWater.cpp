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

#include <algorithm>
#include "DeepWater.h"
#include "Game.h"
#include "World.h"
#include "Rotation.h"
#include "Layer.h"
#include "MdciiAssert.h"
#include "GameState.h"
#include "resource/OriginalResourcesManager.h"
#include "renderer/Renderer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::DeepWater::DeepWater(World* t_world)
    : m_world{ t_world }
{
    MDCII_LOG_DEBUG("[DeepWater::DeepWater()] Create DeepWater.");

    MDCII_ASSERT(m_world, "[DeepWater::DeepWater()] Null pointer.")

    Init();
}

mdcii::world::DeepWater::~DeepWater() noexcept
{
    MDCII_LOG_DEBUG("[DeepWater::~DeepWater()] Destruct DeepWater.");
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::DeepWater::Init()
{
    MDCII_LOG_DEBUG("[DeepWater::Init()] Start creating the deep water area ...");

    InitLayer();
    SetWorldPositions();
    RemoveIrrelevantTiles();
    UpdateTileProperties();

    MDCII_LOG_DEBUG("[DeepWater::Init()] The deep water area was successfully created.");
}

void mdcii::world::DeepWater::InitLayer()
{
    MDCII_LOG_DEBUG("[DeepWater::InitLayer()] Init deep water layer.");

    layer = std::make_unique<Layer>(LayerType::DEEP_WATER, m_world->worldWidth, m_world->worldHeight);
    layer->tiles.resize(m_world->worldWidth * m_world->worldHeight);

    for (auto y{ 0 }; y < m_world->worldHeight; ++y)
    {
        for (auto x{ 0 }; x < m_world->worldWidth; ++x)
        {
            if (!m_world->IsWorldPositionOnAnyIsland(x, y))
            {
                const auto& building1201{ m_world->gameState->game->originalResourcesManager->GetBuildingById(DEEP_WATER_BUILDING_ID) };
                layer->tiles.at(y * m_world->worldWidth + x).building = &building1201;
            }
        }
    }
}

void mdcii::world::DeepWater::SetWorldPositions()
{
    MDCII_LOG_DEBUG("[DeepWater::SetWorldPositions()] Set a world position for each tile.");

    for (auto h{ 0 }; h < m_world->worldHeight; ++h)
    {
        for (auto w{ 0 }; w < m_world->worldWidth; ++w)
        {
            auto& tile{ layer->tiles.at(h * m_world->worldWidth + w) };
            tile.posX = w;
            tile.posY = h;
        }
    }
}

void mdcii::world::DeepWater::RemoveIrrelevantTiles() const
{
    MDCII_LOG_DEBUG("[DeepWater::RemoveIrrelevantTiles()] Remove tiles without building info.");

    auto [begin, end]{ std::ranges::remove_if(layer->tiles, [](const auto& t_tile)
    {
        return t_tile.building == nullptr; })
    };

    layer->tiles.erase(begin, end);
}

void mdcii::world::DeepWater::UpdateTileProperties() const
{
    MDCII_LOG_DEBUG("[DeepWater::UpdateTileProperties()] Pre-calculate other properties.");

    for (auto& tile : layer->tiles)
    {
        MDCII_ASSERT(tile.HasBuilding(), "[DeepWater::UpdateTileProperties()] Building information is missing.")

        // pre-calculate a gfx for each rotation
        const auto gfx0{ tile.building->gfx };

        tile.gfxs.push_back(gfx0);
        if (tile.building->rotate > 0)
        {
            tile.gfxs.push_back(gfx0 + (1 * tile.building->rotate));
            tile.gfxs.push_back(gfx0 + (2 * tile.building->rotate));
            tile.gfxs.push_back(gfx0 + (3 * tile.building->rotate));
        }

        tile.indices[0] = layer->GetMapIndex(tile.posX, tile.posY, Rotation::DEG0);
        tile.indices[1] = layer->GetMapIndex(tile.posX, tile.posY, Rotation::DEG90);
        tile.indices[2] = layer->GetMapIndex(tile.posX, tile.posY, Rotation::DEG180);
        tile.indices[3] = layer->GetMapIndex(tile.posX, tile.posY, Rotation::DEG270);
    }

    layer->SortTilesForRendering();
}
