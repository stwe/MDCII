// This file is part of the MDCII project.
//
// Copyright (c) 2022. stwe <https://github.com/stwe/MDCII>
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

#pragma once

#include "MdciiUtils.h"
#include "vendor/enum/magic_enum.hpp"

namespace mdcii::world
{
    //-------------------------------------------------
    // Change zoom
    //-------------------------------------------------

    enum class ChangeZoom
    {
        ZOOM_IN, // +
        ZOOM_OUT // -
    };

    //-------------------------------------------------
    // Zoom
    //-------------------------------------------------

    enum class Zoom
    {
        SGFX,
        MGFX,
        GFX
    };

    static constexpr auto NR_OF_ZOOMS{ 3 };

    //-------------------------------------------------
    // Operators
    //-------------------------------------------------

    inline Zoom& operator++(Zoom& t_zoom)
    {
        if (t_zoom == Zoom::GFX)
        {
            t_zoom = Zoom::GFX;
        }
        else
        {
            t_zoom = magic_enum::enum_value<Zoom>(magic_enum::enum_integer(t_zoom) + 1_uz);
        }

        return t_zoom;
    }

    inline Zoom& operator--(Zoom& t_zoom)
    {
        if (t_zoom == Zoom::SGFX)
        {
            t_zoom = Zoom::SGFX;
        }
        else
        {
            t_zoom = magic_enum::enum_value<Zoom>(magic_enum::enum_integer(t_zoom) - 1_uz);
        }

        return t_zoom;
    }

    //-------------------------------------------------
    // Tile sizes
    //-------------------------------------------------

    inline static constexpr std::array<std::pair<Zoom, std::pair<int, int>>, NR_OF_ZOOMS> TILE_SIZES = {
        { std::make_pair<Zoom, std::pair<int, int>>(Zoom::SGFX, std::make_pair<int, int>(16, 8)),
          std::make_pair<Zoom, std::pair<int, int>>(Zoom::MGFX, std::make_pair<int, int>(32, 16)),
          std::make_pair<Zoom, std::pair<int, int>>(Zoom::GFX, std::make_pair<int, int>(64, 32)) }
    };

    //-------------------------------------------------
    // Elevation
    //-------------------------------------------------

    inline static constexpr std::array<int, NR_OF_ZOOMS> ELEVATIONS = { 20 / 4, 20 / 2, 20 };

    //-------------------------------------------------
    // For convenience
    //-------------------------------------------------

    constexpr int get_tile_width(const Zoom t_zoom) { return TILE_SIZES[magic_enum::enum_integer(t_zoom)].second.first; }
    constexpr int get_tile_height(const Zoom t_zoom) { return TILE_SIZES[magic_enum::enum_integer(t_zoom)].second.second; }

    constexpr int get_tile_width_half(const Zoom t_zoom) { return TILE_SIZES[magic_enum::enum_integer(t_zoom)].second.first / 2; }
    constexpr int get_tile_height_half(const Zoom t_zoom) { return TILE_SIZES[magic_enum::enum_integer(t_zoom)].second.second / 2; }

    constexpr int get_elevation(const Zoom t_zoom) { return ELEVATIONS[magic_enum::enum_integer(t_zoom)]; }

    //-------------------------------------------------
    // Test
    //-------------------------------------------------

    static_assert(16 == get_tile_width(Zoom::SGFX));
    static_assert(8 == get_tile_height(Zoom::SGFX));

    static_assert(8 == get_tile_width_half(Zoom::SGFX));
    static_assert(4 == get_tile_height_half(Zoom::SGFX));

    static_assert(32 == get_tile_width(Zoom::MGFX));
    static_assert(16 == get_tile_height(Zoom::MGFX));

    static_assert(16 == get_tile_width_half(Zoom::MGFX));
    static_assert(8 == get_tile_height_half(Zoom::MGFX));

    static_assert(64 == get_tile_width(Zoom::GFX));
    static_assert(32 == get_tile_height(Zoom::GFX));

    static_assert(32 == get_tile_width_half(Zoom::GFX));
    static_assert(16 == get_tile_height_half(Zoom::GFX));

    static_assert(5 == get_elevation(Zoom::SGFX));
    static_assert(10 == get_elevation(Zoom::MGFX));
    static_assert(20 == get_elevation(Zoom::GFX));
}
