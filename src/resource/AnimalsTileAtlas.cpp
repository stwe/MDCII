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
#include "world/layer/Layer.h"
#include "camera/Camera.h"

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

void mdcii::resource::AnimalsTileAtlas::RenderTile(
    const int t_startX,
    const int t_startY,
    const world::tile::FigureTile* t_tile,
    const olc::Pixel& t_tint
) const
{
    const auto zoomInt{ magic_enum::enum_integer(m_world->camera->zoom) };

    const auto animationOffset{ t_tile->figure->animations[t_tile->currentAnimation].animOffs };
    const auto gfx{
        t_tile->figure->gfx + animationOffset + (t_tile->rotation * t_tile->figure->animations[t_tile->currentAnimation].animAnz) + t_tile->frame
    };

    const olc::vf2d atlasOffset{ GetAtlasOffset(gfx, NR_OF_ROWS) };

    olc::vf2d screenPosition{ m_world->ToScreen(t_tile->posX + t_startX, t_tile->posY + t_startY) };
    screenPosition.y -= CalcOffset(gfx);

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

    if (m_timer_values[0] >= 0.075f)
    {
        frame_values[0] = ++frame_values[0];
        m_timer_values[0] = 0.0f;
        frame_values[0] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[1] >= 0.11f)
    {
        frame_values[1] = ++frame_values[1];
        m_timer_values[1] = 0.0f;
        frame_values[1] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[2] >= 0.13f)
    {
        frame_values[2] = ++frame_values[2];
        m_timer_values[2] = 0.0f;
        frame_values[2] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[3] >= 0.135f)
    {
        frame_values[3] = ++frame_values[3];
        m_timer_values[3] = 0.0f;
        frame_values[3] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[4] >= 0.195f)
    {
        frame_values[4] = ++frame_values[4];
        m_timer_values[4] = 0.0f;
        frame_values[4] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[5] >= 0.3f)
    {
        frame_values[5] = ++frame_values[5];
        m_timer_values[5] = 0.0f;
        frame_values[5] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[6] >= 0.5f)
    {
        frame_values[6] = ++frame_values[6];
        m_timer_values[6] = 0.0f;
        frame_values[6] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[7] >= 0.75f)
    {
        frame_values[7] = ++frame_values[7];
        m_timer_values[7] = 0.0f;
        frame_values[7] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[8] >= 1.0f)
    {
        frame_values[8] = ++frame_values[8];
        m_timer_values[8] = 0.0f;
        frame_values[8] %= MAX_FRAME_VALUE + 1;
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

float mdcii::resource::AnimalsTileAtlas::CalcOffset(const int t_gfx) const
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

    offset += world::ELEVATIONS[zoomInt];

    return offset;
}
