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

#include "World.h"
#include "MdciiAssert.h"
#include "MousePicker.h"
#include "state/State.h"
#include "physics/Aabb.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::World::World(std::shared_ptr<state::Context> t_context, const state::StateId t_stateId)
    : context{ std::move(t_context) }
    , stateId{ t_stateId }
{
    Log::MDCII_LOG_DEBUG("[World::World()] Create World.");

    MDCII_ASSERT(context, "[World::World()] Null pointer.")

    rotation = Rotation::DEG0;
    zoom = Zoom::GFX;
}

mdcii::world::World::~World() noexcept
{
    Log::MDCII_LOG_DEBUG("[World::~World()] Destruct World.");
}

//-------------------------------------------------
// Rotate && Zoom
//-------------------------------------------------

void mdcii::world::World::RotateWorld(const ChangeRotation t_changeRotation)
{
    switch (t_changeRotation)
    {
    case ChangeRotation::LEFT:
        --rotation;
        break;
    case ChangeRotation::RIGHT:
        ++rotation;
        break;
    }
}

void mdcii::world::World::ZoomWorld(const ChangeZoom t_changeZoom)
{
    switch (t_changeZoom)
    {
    case ChangeZoom::ZOOM_IN:
        ++zoom;
        break;
    case ChangeZoom::ZOOM_OUT:
        --zoom;
        break;
    }

    context->camera->zoom = zoom;
    context->camera->position.x = static_cast<float>(context->camera->worldPosition.x) * static_cast<float>(get_tile_width(zoom));
    context->camera->position.y = static_cast<float>(context->camera->worldPosition.y) * static_cast<float>(get_tile_height(zoom));
    context->camera->aabb->position = context->camera->position;
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

bool mdcii::world::World::IsPositionInWorld(const int32_t t_x, const int32_t t_y) const
{
    if (t_x >= 0 && t_x < width && t_y >= 0 && t_y < height)
    {
        return true;
    }

    return false;
}

bool mdcii::world::World::IsPositionInWorld(const glm::ivec2& t_position) const
{
    return IsPositionInWorld(t_position.x, t_position.y);
}

glm::vec2 mdcii::world::World::WorldToScreen(const int32_t t_x, const int32_t t_y, const Zoom t_zoom, const Rotation t_rotation) const
{
    const auto position{ rotate_position(t_x, t_y, width, height, t_rotation) };
    return {
        (position.x - position.y) * get_tile_width_half(t_zoom),
        (position.x + position.y) * get_tile_height_half(t_zoom)
    };
}
