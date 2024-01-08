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
#include "state/State.h"
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

void mdcii::resource::AnimalsTileAtlas::Render(const int t_x, const int t_y, const olc::Pixel& t_tint) const
{
    const auto zoomInt{ magic_enum::enum_integer(m_world->camera->zoom) };
    const auto gfx{ 8 };
    const olc::vf2d atlasOffset{ GetAtlasOffset(gfx, resource::AnimalsTileAtlas::NR_OF_ROWS) };
    const olc::vf2d screenPosition{ m_world->ToScreen(t_x, t_y) };

    m_world->state->game->DrawPartialDecal(
        screenPosition,
        m_atlas[zoomInt][GetAtlasIndex(gfx, resource::AnimalsTileAtlas::NR_OF_ROWS)]->Decal(),
        {
            atlasOffset.x * resource::AnimalsTileAtlas::LARGEST_SIZE[zoomInt].second.first,
            atlasOffset.y * resource::AnimalsTileAtlas::LARGEST_SIZE[zoomInt].second.second
        },
        {
            resource::AnimalsTileAtlas::LARGEST_SIZE[zoomInt].second.first,
            resource::AnimalsTileAtlas::LARGEST_SIZE[zoomInt].second.second
        },
        { 1.0f, 1.0f },
        t_tint
    );
}
