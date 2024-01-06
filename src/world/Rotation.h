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

#pragma once

#include "MdciiException.h"
#include "vendor/enum/magic_enum.hpp"
#include "vendor/olc/olcPixelGameEngine.h"

namespace mdcii::world
{
    //-------------------------------------------------
    // Rotation
    //-------------------------------------------------

    enum class Rotation
    {
        DEG0,
        DEG90,
        DEG180,
        DEG270
    };

    //-------------------------------------------------
    // Operators
    //-------------------------------------------------

    inline Rotation& operator++(Rotation& t_rotation)
    {
        if (t_rotation == Rotation::DEG270)
        {
            t_rotation = Rotation::DEG270;
        }
        else
        {
            t_rotation = static_cast<Rotation>(static_cast<std::underlying_type_t<Rotation>>(t_rotation) + 1);
        }

        return t_rotation;
    }

    inline Rotation& operator--(Rotation& t_rotation)
    {
        if (t_rotation == Rotation::DEG0)
        {
            t_rotation = Rotation::DEG0;
        }
        else
        {
            t_rotation = static_cast<Rotation>(static_cast<std::underlying_type_t<Rotation>>(t_rotation) - 1);
        }

        return t_rotation;
    }

    //-------------------------------------------------
    // For convenience
    //-------------------------------------------------

    inline Rotation int_to_rotation(const int t_rotation)
    {
        if (const auto rotation{ magic_enum::enum_cast<Rotation>(t_rotation) }; rotation.has_value())
        {
            return rotation.value();
        }

        throw MDCII_EXCEPTION("[int_to_rotation()] Invalid rotation given.");
    }

    //-------------------------------------------------
    // Rotate position
    //-------------------------------------------------

    inline olc::vi2d rotate_position(
        const int t_mapX, const int t_mapY,
        const int t_width, const int t_height,
        const Rotation t_rotation = Rotation::DEG0
    )
    {
        auto x{ t_mapX };
        auto y{ t_mapY };

        switch (t_rotation)
        {
        case Rotation::DEG0:
            break;
        case Rotation::DEG90:
            x = t_width - t_mapY - 1;
            y = t_mapX;
            break;
        case Rotation::DEG180:
            x = t_width - t_mapX - 1;
            y = t_height - t_mapY - 1;
            break;
        case Rotation::DEG270:
            x = t_mapY;
            y = t_height - t_mapX - 1;
            break;
        }

        return { x, y };
    }

    //-------------------------------------------------
    // Operators
    //-------------------------------------------------

    inline Rotation operator+(const Rotation t_rotation, const Rotation t_other)
    {
        const auto result{ (magic_enum::enum_integer(t_rotation) + magic_enum::enum_integer(t_other)) % static_cast<int>(magic_enum::enum_count<Rotation>()) };
        return int_to_rotation(result);
    }

    inline Rotation operator-(const Rotation t_rotation, const Rotation t_other)
    {
        const auto result{ (magic_enum::enum_integer(t_rotation) - magic_enum::enum_integer(t_other) + static_cast<int>(magic_enum::enum_count<Rotation>())) % static_cast<int>(magic_enum::enum_count<Rotation>()) };
        return int_to_rotation(result);
    }
}
