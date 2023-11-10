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

#include "Camera.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "GameState.h"
#include "world/World.h"
#include "world/DeepWater.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::camera::Camera::Camera(const world::World* t_world)
    : m_world{ t_world }
{
    MDCII_LOG_DEBUG("[Camera::Camera()] Create Camera.");

    MDCII_ASSERT(m_world, "[Camera::Camera()] Null pointer.")
}

mdcii::camera::Camera::~Camera() noexcept
{
    MDCII_LOG_DEBUG("[Camera::~Camera()] Destruct Camera.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::camera::Camera::OnUserUpdate(const float t_elapsedTime, const olc::vf2d& t_vel)
{
    origin.x += t_vel.x * t_elapsedTime * 32.0f;
    origin.y += t_vel.y * t_elapsedTime * 32.0f;

    worldPosition.x = static_cast<int>(origin.x);
    worldPosition.y = static_cast<int>(origin.y);

    screenPosition.x = worldPosition.x * get_tile_width(m_world->gameState->zoom);
    screenPosition.y = worldPosition.y * get_tile_height(m_world->gameState->zoom);

    m_world->gameState->game->DrawString(4, 14, fmt::format("Camera world: {}, {}", std::to_string(worldPosition.x), std::to_string(worldPosition.y)), olc::WHITE);
    m_world->gameState->game->DrawString(4, 24, fmt::format("Camera screen: {}, {}", std::to_string(screenPosition.x), std::to_string(screenPosition.y)), olc::WHITE);
}
