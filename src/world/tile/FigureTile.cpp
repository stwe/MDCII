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
    const int t_currentAnimation,
    const int t_rotation,
    const int t_posX,
    const int t_posY
)
    : Tile(t_rotation, t_posX, t_posY)
    , figure{ t_figure }
    , currentAnimation{ t_currentAnimation }
{

}

//-------------------------------------------------
// Getter
//-------------------------------------------------

bool mdcii::world::tile::FigureTile::HasFigure(const resource::FigureId t_figureId) const
{
    return HasFigure() && figure->id == t_figureId;
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::tile::FigureTile::UpdateFrame(const std::array<int, NR_OF_ANIM_TIMES>& t_frameValues)
{
    // 75, 110, 130, 135, 195, 300, 500, 750, 1000
    switch (figure->animations[currentAnimation].animSpeed)
    {
        case 75:
            frame = t_frameValues[0] % figure->animations[currentAnimation].animAnz;
            break;
        case 110:
            frame = t_frameValues[1] % figure->animations[currentAnimation].animAnz;
            break;
        case 130:
            frame = t_frameValues[2] % figure->animations[currentAnimation].animAnz;
            break;
        case 135:
            frame = t_frameValues[3] % figure->animations[currentAnimation].animAnz;
            break;
        case 195:
            frame = t_frameValues[4] % figure->animations[currentAnimation].animAnz;
            break;
        case 300:
            frame = t_frameValues[5] % figure->animations[currentAnimation].animAnz;
            break;
        case 500:
            frame = t_frameValues[6] % figure->animations[currentAnimation].animAnz;
            break;
        case 750:
            frame = t_frameValues[7] % figure->animations[currentAnimation].animAnz;
            break;
        case 1000:
            frame = t_frameValues[8] % figure->animations[currentAnimation].animAnz;
            break;
        default:
            frame = 0;
    }
}

void mdcii::world::tile::to_json(nlohmann::json& t_json, const world::tile::FigureTile& t_tile)
{
    if (t_tile.HasFigure())
    {
        t_json = nlohmann::json{
            { "id", t_tile.figure->id },
            { "rotation", t_tile.rotation },
            { "animation", t_tile.currentAnimation },
        };
    }
    else
    {
        t_json = nlohmann::json::object();
    }
}
