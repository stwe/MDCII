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
    camera->OnUserUpdate(t_elapsedTime);

    renderer->RenderDeepWater();
    //renderer->RenderIslands();
}

//-------------------------------------------------
// Helper
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
    for (auto const& island : islands)
    {
        if (physics::Aabb::PointVsAabb({ t_x, t_y }, island->aabb))
        {
            return true;
        }
    }

    return false;
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::World::Init(const std::string& t_fileName)
{
    MDCII_LOG_DEBUG("[World::Init()] Init World.");

    // create islands that encompass all island tiles and set worldWidth and worldHeight
    if (resource::MdciiFile file{ t_fileName }; file.LoadJsonFromFile())
    {
        islands = file.CreateWorldContent(worldWidth, worldHeight);
    }

    for (auto const& island : islands)
    {
        island->InitTiles(gameState->game);
    }

    // create deep water area
    deepWater = std::make_unique<DeepWater>();
    deepWater->width = worldWidth;
    deepWater->height = worldHeight;
    deepWater->tiles.resize(worldWidth * worldHeight);

    for (auto y{ 0 }; y < worldHeight; ++y)
    {
        for (auto x{ 0 }; x < worldWidth; ++x)
        {
            if (!IsWorldPositionOnAnyIsland(x, y))
            {
                deepWater->tiles.at(y * worldWidth + x).buildingId = 1201;
            }
        }
    }

    deepWater->InitTiles(gameState->game);

    // renderer && camera
    renderer = std::make_unique<renderer::Renderer>(this);
    camera = std::make_unique<camera::Camera>(this);
}
