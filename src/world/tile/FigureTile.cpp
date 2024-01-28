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

#include "FigureTile.h"
#include "resource/Buildings.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::tile::FigureTile::FigureTile()
{

}

mdcii::world::tile::FigureTile::FigureTile(
    const resource::Figure* t_figure,
    const int t_rotation,
    const int t_posX,
    const int t_posY
)
    : Tile(t_rotation, t_posX, t_posY)
    , figure{ t_figure }
{

}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::tile::FigureTile::UpdateFrame(const std::array<int, NR_OF_ANIM_TIMES>& t_frameValues)
{
    // 75, 110, 130, 135, 195, 300, 500, 750, 1000

    // todo: statt dessen die aktive Animation auswählen
    switch (figure->animations[0].animSpeed)
    {
        case 75:
            frame = t_frameValues[0] % figure->animations[0].animAnz;
            break;
        case 110:
            frame = t_frameValues[1] % figure->animations[0].animAnz;
            break;
        case 130:
            frame = t_frameValues[2] % figure->animations[0].animAnz;
            break;
        case 135:
            frame = t_frameValues[3] % figure->animations[0].animAnz;
            break;
        case 195:
            frame = t_frameValues[4] % figure->animations[0].animAnz;
            break;
        case 300:
            frame = t_frameValues[5] % figure->animations[0].animAnz;
            break;
        case 500:
            frame = t_frameValues[6] % figure->animations[0].animAnz;
            break;
        case 750:
            frame = t_frameValues[7] % figure->animations[0].animAnz;
            break;
        case 1000:
            frame = t_frameValues[8] % figure->animations[0].animAnz;
            break;
        default:
            frame = 0;
    }
}
