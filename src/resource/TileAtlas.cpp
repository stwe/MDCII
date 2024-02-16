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

#include "TileAtlas.h"
#include "world/layer/Layer.h"
#include "camera/Camera.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::resource::TileAtlas::TileAtlas(const world::World* t_world)
    : BshTileAtlas(TILE_ATLAS_NAME, NR_OF_SGFX_ATLAS_IMAGES, NR_OF_MGFX_ATLAS_IMAGES, NR_OF_GFX_ATLAS_IMAGES)
    , m_world{ t_world }
{
    MDCII_LOG_DEBUG("[TileAtlas::TileAtlas()] Create TileAtlas.");

    MDCII_ASSERT(m_world, "[TileAtlas::TileAtlas()] Null pointer.")
}

mdcii::resource::TileAtlas::~TileAtlas() noexcept
{
    MDCII_LOG_DEBUG("[TileAtlas::~TileAtlas()] Destruct TileAtlas.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::resource::TileAtlas::RenderTile(
    const int t_startX,
    const int t_startY,
    const world::tile::TerrainTile* t_tile,
    const olc::Pixel& t_tint
) const
{
    const auto zoomInt{ magic_enum::enum_integer(m_world->camera->zoom) };
    const auto gfx{ GetGfxForCurrentRotation(t_tile) };
    const olc::vf2d atlasOffset{ GetAtlasOffset(gfx, NR_OF_ROWS[zoomInt]) };

    olc::vf2d screenPosition{ m_world->ToScreen(t_tile->posX + t_startX, t_tile->posY + t_startY) };
    screenPosition.y -= CalcOffset(t_tile, gfx);

    m_world->state->game->DrawPartialDecal(
        screenPosition,
        m_atlas[zoomInt][GetAtlasIndex(gfx, NR_OF_ROWS[zoomInt])]->Decal(),
        {
            atlasOffset.x * LARGEST_SIZE[zoomInt].second.first,
            atlasOffset.y * LARGEST_SIZE[zoomInt].second.second
        },
        {
            LARGEST_SIZE[zoomInt].second.first,
            LARGEST_SIZE[zoomInt].second.second
        },
        { 1.0f, 1.0f },
        t_tile->tintFlag == 1 ? olc::BLUE : t_tint
    );
}

void mdcii::resource::TileAtlas::CalcAnimationFrame(const float t_elapsedTime)
{
    for (auto& timer : m_timer_values)
    {
        timer += t_elapsedTime;
    }

    if (m_timer_values[0] >= 0.09f)
    {
        frame_values[0] = ++frame_values[0];
        m_timer_values[0] = 0.0f;
        frame_values[0] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[1] >= 0.13f)
    {
        frame_values[1] = ++frame_values[1];
        m_timer_values[1] = 0.0f;
        frame_values[1] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[2] >= 0.15f)
    {
        frame_values[2] = ++frame_values[2];
        m_timer_values[2] = 0.0f;
        frame_values[2] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[3] >= 0.18f)
    {
        frame_values[3] = ++frame_values[3];
        m_timer_values[3] = 0.0f;
        frame_values[3] %= MAX_FRAME_VALUE + 1;
    }

    if (m_timer_values[4] >= 0.22f)
    {
        frame_values[4] = ++frame_values[4];
        m_timer_values[4] = 0.0f;
        frame_values[4] %= MAX_FRAME_VALUE + 1;
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

int mdcii::resource::TileAtlas::GetGfxForCurrentRotation(const world::tile::TerrainTile* t_tile) const
{
    MDCII_ASSERT(!t_tile->gfxs.empty(), "[TileAtlas::GetGfxForCurrentRotation()] Missing gfx values.")

    if (t_tile->gfxs.size() == 1)
    {
        return t_tile->gfxs[0] + t_tile->frame * t_tile->building->animAdd;
    }

    const auto worldRotation{ m_world->camera->rotation + world::int_to_rotation(t_tile->rotation) };
    return t_tile->gfxs[magic_enum::enum_integer(worldRotation)] + t_tile->frame * t_tile->building->animAdd;
}

float mdcii::resource::TileAtlas::CalcOffset(const world::tile::TerrainTile* t_tile, const int t_gfx) const
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
