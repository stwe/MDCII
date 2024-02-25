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

#include "Tile.h"
#include "MdciiAssert.h"
#include "world/Rotation.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::tile::Tile::Tile()
{
}

mdcii::world::tile::Tile::Tile(const int t_rotation, const int t_posX, const int t_posY)
    : rotation{ t_rotation }
    , posX{ t_posX }
    , posY{ t_posY }
{
}

mdcii::world::tile::Tile::Tile(const int t_rotation, const int t_posX, const int t_posY, const TileType t_tileType)
    : rotation{ t_rotation }
    , posX{ t_posX }
    , posY{ t_posY }
    , type{ t_tileType }
{
}

mdcii::world::tile::Tile::~Tile() noexcept
{
}

//-------------------------------------------------
// Render index
//-------------------------------------------------

int mdcii::world::tile::Tile::GetRenderIndex(const int t_x, const int t_y, const int t_width, const int t_height, const Rotation t_rotation)
{
    MDCII_ASSERT(t_x >= 0 && t_x < t_width, "[Tile::GetRenderIndex()] Invalid x position given.")
    MDCII_ASSERT(t_y >= 0 && t_y < t_height, "[Tile::GetRenderIndex()] Invalid y position given.")

    const auto position{ rotate_position(t_x, t_y, t_width, t_height, t_rotation) };

    if (t_rotation == world::Rotation::DEG0 || t_rotation == world::Rotation::DEG180)
    {
        return position.y * t_width + position.x;
    }

    return position.y * t_height + position.x;
}

void mdcii::world::tile::Tile::CalcRenderPositions(const int t_width, const int t_height)
{
    renderIndices.resize(magic_enum::enum_count<Rotation>());
    renderIndices[0] = GetRenderIndex(posX, posY, t_width, t_height, Rotation::DEG0);
    renderIndices[1] = GetRenderIndex(posX, posY, t_width, t_height, Rotation::DEG90);
    renderIndices[2] = GetRenderIndex(posX, posY, t_width, t_height, Rotation::DEG180);
    renderIndices[3] = GetRenderIndex(posX, posY, t_width, t_height, Rotation::DEG270);
}
