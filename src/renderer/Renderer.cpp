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

#include "Renderer.h"
#include "Game.h"
#include "world/World.h"
#include "world/Tile.h"
#include "resource/AssetManager.h"
#include "camera/Camera.h"
#include "state/State.h"

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::renderer::Renderer::RenderAsset(
    const resource::Asset t_asset,
    const int t_startX,
    const int t_startY,
    const world::World* t_world,
    const world::Tile* t_tile,
    const bool t_minusElevation,
    const olc::Pixel& t_tint
)
{
    olc::vf2d screenPosition{ t_world->ToScreen(t_tile->posX + t_startX, t_tile->posY + t_startY) };
    if (t_minusElevation)
    {
        screenPosition.y -= world::ELEVATIONS[magic_enum::enum_integer(t_world->camera->zoom)];
    }

    t_world->state->game->DrawDecal(
        screenPosition,
        t_world->state->game->assetManager->GetAsset(t_asset, t_world->camera->zoom)->Decal(),
        { 1.0f, 1.0f },
        t_tint
    );
}
