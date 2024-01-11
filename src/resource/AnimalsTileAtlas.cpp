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

#include "AnimalsTileAtlas.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "world/World.h"
#include "world/Island.h"
#include "state/State.h"
#include "camera/Camera.h"
#include "world/Layer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::resource::AnimalsTileAtlas::AnimalsTileAtlas(const world::World* t_world)
    : BshTileAtlas(TILE_ATLAS_NAME, NR_OF_ATLAS_IMAGES, NR_OF_ATLAS_IMAGES, NR_OF_ATLAS_IMAGES)
    , m_world{ t_world }
{
    MDCII_LOG_DEBUG("[AnimalsTileAtlas::AnimalsTileAtlas()] Create AnimalsTileAtlas.");

    MDCII_ASSERT(m_world, "[AnimalsTileAtlas::AnimalsTileAtlas()] Null pointer.")
}

mdcii::resource::AnimalsTileAtlas::~AnimalsTileAtlas() noexcept
{
    MDCII_LOG_DEBUG("[AnimalsTileAtlas::~AnimalsTileAtlas()] Destruct AnimalsTileAtlas.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::resource::AnimalsTileAtlas::Render(Animation* t_animation, const olc::Pixel& t_tint) const
{
    t_animation->frame = m_frame_values[0] % t_animation->spriteSheet->count;

    const auto zoomInt{ magic_enum::enum_integer(m_world->camera->zoom) };
    const auto gfx{ t_animation->spriteSheet->start + t_animation->frame };
    const olc::vf2d atlasOffset{ GetAtlasOffset(gfx, NR_OF_ROWS) };

    olc::vf2d screenPosition;
    if (t_animation->island->IsWorldPositionOverIsland({ t_animation->island->startX + t_animation->posX, t_animation->island->startY + t_animation->posY }))
    {
        auto& terrainTileToCheck{ t_animation->island->GetLayer(world::LayerType::TERRAIN)->GetSortedTile(t_animation->posX, t_animation->posY, m_world->camera->rotation) };
        screenPosition = m_world->ToScreen(t_animation->island->startX + t_animation->posX, t_animation->island->startY + t_animation->posY);
        screenPosition.y -= CalcOffset(&terrainTileToCheck, gfx);
    }

    m_world->state->game->DrawPartialDecal(
        screenPosition,
        m_atlas[zoomInt][GetAtlasIndex(gfx, NR_OF_ROWS)]->Decal(),
        {
            atlasOffset.x * LARGEST_SIZE[zoomInt].second.first,
            atlasOffset.y * LARGEST_SIZE[zoomInt].second.second
        },
        {
            LARGEST_SIZE[zoomInt].second.first,
            LARGEST_SIZE[zoomInt].second.second
        },
        { 1.0f, 1.0f },
        t_tint
    );
}

void mdcii::resource::AnimalsTileAtlas::CalcAnimationFrame(const float t_elapsedTime)
{
    for (auto& timer : m_timer_values)
    {
        timer += t_elapsedTime;
    }

    if (m_timer_values[0] >= 0.09f)
    {
        m_frame_values[0] = ++m_frame_values[0];
        m_timer_values[0] = 0.0f;
        m_frame_values[0] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[1] >= 0.13f)
    {
        m_frame_values[1] = ++m_frame_values[1];
        m_timer_values[1] = 0.0f;
        m_frame_values[1] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[2] >= 0.15f)
    {
        m_frame_values[2] = ++m_frame_values[2];
        m_timer_values[2] = 0.0f;
        m_frame_values[2] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[3] >= 0.18f)
    {
        m_frame_values[3] = ++m_frame_values[3];
        m_timer_values[3] = 0.0f;
        m_frame_values[3] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[4] >= 0.22f)
    {
        m_frame_values[4] = ++m_frame_values[4];
        m_timer_values[4] = 0.0f;
        m_frame_values[4] %= MAX_FRAME_VALUE + 1;
    }
}

float mdcii::resource::AnimalsTileAtlas::CalcOffset(const world::Tile* t_tile, const int t_gfx) const
{
    auto offset{ 0.0f };
    const auto zoomInt{ magic_enum::enum_integer(m_world->camera->zoom) };
    auto tileHeight{ get_tile_height(m_world->camera->zoom) };
    const auto gfxHeight{ m_heights[zoomInt][t_gfx] };

    if (m_world->camera->zoom == world::Zoom::GFX)
    {
        tileHeight = 31;
    }
    if (gfxHeight > tileHeight)
    {
        offset = static_cast<float>(gfxHeight) - static_cast<float>(tileHeight);
    }
    if (t_tile->HasBuildingAboveWaterAndCoast())
    {
        offset += world::ELEVATIONS[zoomInt];
    }

    return offset;
}
