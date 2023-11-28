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

#include "Tile.h"
#include "Rotation.h"
#include "MdciiAssert.h"
#include "resource/Buildings.h"

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::Tile::UpdateFrame(const std::array<int, NR_OF_ANIM_TIMES>& t_frameValues)
{
    switch (building->animTime)
    {
    case 90:
        frame = t_frameValues[0] % building->animAnz;
        break;
    case 130:
        frame = t_frameValues[1] % building->animAnz;
        break;
    case 150:
        frame = t_frameValues[2] % building->animAnz;
        break;
    case 180:
        frame = t_frameValues[3] % building->animAnz;
        break;
    case 220:
        frame = t_frameValues[4] % building->animAnz;
        break;
    default:
        frame = 0;
    }
}

//-------------------------------------------------
// Gfx
//-------------------------------------------------

void mdcii::world::Tile::CalculateGfx()
{
    MDCII_ASSERT(HasBuilding(), "[Tile::CalculateGfx()] Null pointer.")

    const auto gfx0{ building->gfx };

    gfxs.push_back(gfx0);
    if (building->rotate > 0)
    {
        gfxs.push_back(gfx0 + (1 * building->rotate));
        gfxs.push_back(gfx0 + (2 * building->rotate));
        gfxs.push_back(gfx0 + (3 * building->rotate));
    }

    if (building->size.w > 1 || building->size.h > 1)
    {
        for (auto& gfx : gfxs)
        {
            AdjustGfxForBigBuildings(gfx);
        }
    }
}

void mdcii::world::Tile::AdjustGfxForBigBuildings(int& t_gfx) const
{
    MDCII_ASSERT(HasBuilding(), "[Tile::AdjustGfxForBigBuildings()] Null pointer.")

    // default: orientation 0
    auto rp{ olc::vi2d(x, y) };

    if (rotation == magic_enum::enum_integer(Rotation::DEG270))
    {
        rp = rotate_position(
            x, y,
            building->size.w, building->size.h,
            Rotation::DEG90
        );
    }

    if (rotation == magic_enum::enum_integer(Rotation::DEG180))
    {
        rp = rotate_position(
            x, y,
            building->size.w, building->size.h,
            Rotation::DEG180
        );
    }

    if (rotation == magic_enum::enum_integer(Rotation::DEG90))
    {
        rp = rotate_position(
            x, y,
            building->size.w, building->size.h,
            Rotation::DEG270
        );
    }

    const auto offset{ rp.y * building->size.w + rp.x };
    t_gfx += offset;
}
