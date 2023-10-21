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

#include "Zoom.h"
#include "MdciiAssert.h"

namespace mdcii::world
{
    //-------------------------------------------------
    // Tile
    //-------------------------------------------------

    struct Tile
    {
        int32_t buildingId{ -1 };
        int32_t gfx{ -1 };
        int32_t rotation{ 0 };
        int32_t x{ 0 };
        int32_t y{ 0 };
        int32_t posoffs{ 0 };

        std::array<int32_t, NR_OF_ZOOMS> heights{ 0, 0, 0 };
        std::array<float, NR_OF_ZOOMS> offsets{ 0.0f, 0.0f, 0.0f };

        [[nodiscard]] bool HasBuilding() const { return buildingId != -1; }
        [[nodiscard]] bool HasBuildingAndGfx() const { return buildingId != -1 && gfx != -1; }
        [[nodiscard]] bool IsAboveWater() const { return posoffs > 0; }

        void SetOffset(const Zoom t_zoom)
        {
            const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
            MDCII_ASSERT(heights.at(zoomInt) > 0, "[Tile::SetOffset()] Invalid height.")

            auto height{ get_tile_height(t_zoom) };
            if (t_zoom == Zoom::GFX)
            {
                height -= 1; // = 31
            }

            if (IsAboveWater() && heights.at(zoomInt) > height)
            {
                offsets.at(zoomInt) = static_cast<float>(heights.at(zoomInt)) - static_cast<float>(height);
            }
        }
    };
}
