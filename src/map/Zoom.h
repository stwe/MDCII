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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#pragma once

#include <magic_enum.hpp>
#include "MdciiLiterals.h"

namespace mdcii::map
{
    enum class ChangeZoom
    {
        ZOOM_IN, // +
        ZOOM_OUT // -
    };

    enum class Zoom
    {
        SGFX, MGFX, GFX
    };

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

    inline const char* zoom_to_string(const Zoom t_zoom)
    {
        return magic_enum::enum_name(t_zoom).data();
    }
}
