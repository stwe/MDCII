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

#include "Renderer.h"
#include "Game.h"
#include "world/Island.h"
#include "resource/TileAtlas.h"
#include "vendor/olc/olcPixelGameEngine.h"

void mdcii::renderer::Renderer::Render(Game* t_game)
{
    auto toScreen = [](int t_x, int t_y, float t_startX, float t_startY)
    {
        return olc::vi2d
            {
                ((int)t_startX * 64) + (t_x - t_y) * (64 / 2),
                ((int)t_startY * 32) + (t_x + t_y) * (32 / 2)
            };
    };

    auto getAtlasIndex = [](int t_gfx, int t_rows)
    {
        return (t_gfx / (t_rows * t_rows));
    };

    auto getOffset = [](int t_gfx, int t_rows)
    {
        auto picInPic{ t_gfx % (t_rows * t_rows) };

        return olc::vi2d(picInPic % t_rows, picInPic / t_rows);
    };

    for (auto const& island : t_game->islands)
    {
        const auto zoomInt{ magic_enum::enum_integer(t_game->zoom) };

        // render
        for (auto y{ 0 }; y < island->height; ++y)
        {
            for (auto x{ 0 }; x < island->width; ++x)
            {
                const auto index{ y * island->width + x };
                const auto& tile{ island->tiles.at(index) };

                if (tile.HasBuildingAndGfx())
                {
                    auto atlas{ getAtlasIndex(tile.gfx, 16) };
                    auto offsetXy{ getOffset(tile.gfx, 16) };
                    auto sc{ toScreen(x, y, island->x, island->y) };

                    t_game->DrawPartialDecal(
                        olc::vf2d(
                            (float)sc.x,
                            (float)sc.y - tile.offsets[zoomInt]
                        ),
                        t_game->tileAtlas->gfxAtlas.at(atlas)->Decal(),
                        olc::vf2d(
                            (float)offsetXy.x * 64.0f,
                            (float)offsetXy.y * 286.0f
                        ),
                        olc::vf2d(64.0f, 286.0f)
                    );
                }
            }
        }
    }
}
