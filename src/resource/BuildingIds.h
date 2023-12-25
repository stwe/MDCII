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

#include <unordered_set>
#include <array>

namespace mdcii::resource
{
    /**
     * @brief Contains the building IDs that are not shown in different rotations.
     */
    const std::unordered_set NON_ROTATABLE_BUILDING_IDS{ 605, 1073, 1331 };

    /**
     * @brief Contains the IDs of the buildings that can only be built on the coast.
     */
    const std::unordered_set WATER_RELATED_BUILDING_IDS{ 1073, 1075 };

    // height = 20

    static constexpr auto GRASS_BUILDING_ID{ 101 };

    static constexpr auto BANK_BUILDING_ID{ 1011 };
    static constexpr auto BANK_CORNER_INSIDE_BUILDING_ID{ 1031 };
    static constexpr auto BANK_CORNER_BUILDING_ID{ 1051 };

    static constexpr std::array EMBANKMENT_CORNER_INSIDE_BUILDING_IDS{ 1031, 1032, 1033, 1034 };
    static constexpr std::array EMBANKMENT_CORNER_BUILDING_IDS{ 1051, 1052, 1053 };
    static constexpr std::array EMBANKMENT_BUILDING_IDS{ 1011, 1012, 1013, 1014, 1015, 1016, 1017, 1018, 1019, 1020 };

    // height = 0

    static constexpr auto SHALLOW_WATER_BUILDING_ID{ 1203 };
    static constexpr auto SHALLOW_TO_MEDIUM_WATER_BUILDING_ID{ 1202 };
    static constexpr auto MEDIUM_WATER_BUILDING_ID{ 1204 };
    static constexpr auto MEDIUM_TO_DEEP_WATER_BUILDING_ID{ 1209 };
    static constexpr auto DEEP_WATER_BUILDING_ID{ 1201 };

    static constexpr auto BEACH_BUILDING_ID{ 1205 };
    static constexpr auto BEACH_CORNER_INSIDE_BUILDING_ID{ 1206 };
    static constexpr auto BEACH_CORNER_BUILDING_ID{ 1207 };
    static constexpr auto BEACH_MOUTH_BUILDING_ID{ 1208 };

    static constexpr std::array NORTH_TREES_BUILDING_IDS{ 1304, 1306, 1308, 1310, 1312, 1314, 1316, 1318, 1320, 1322, 1324 };
    static constexpr std::array SOUTH_TREES_BUILDING_IDS{ 1352, 1354, 1356, 1358, 1360, 1362, 1364, 1366, 1368, 1370, 1372 };
}