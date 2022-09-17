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

#include "map/MapTile.h"

//-------------------------------------------------
// MapGenerator
//-------------------------------------------------

namespace mdcii::map
{
    /**
     * Class to create maps.
     * Currently the class creates a simple rectangular map.
     */
    class MapGenerator
    {
    public:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        static constexpr auto GRASS{ 101 };
        static constexpr auto COAST{ 1011 };
        static constexpr auto COAST_CORNER{ 1051 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MapGenerator() = delete;

        /**
         * Constructs a new MapGenerator object.
         *
         * @param t_width The width of the map to create.
         * @param t_height The height of the map to create.
         * @param t_fileName The filename under which to save the map.
         */
        MapGenerator(int t_width, int t_height, const std::string& t_fileName);

        MapGenerator(const MapGenerator& t_other) = delete;
        MapGenerator(MapGenerator&& t_other) noexcept = delete;
        MapGenerator& operator=(const MapGenerator& t_other) = delete;
        MapGenerator& operator=(MapGenerator&& t_other) noexcept = delete;

        ~MapGenerator() noexcept;

    protected:

    private:
        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Creates a map.
         *
         * @param t_width The width of the map to create.
         * @param t_height The height of the map to create.
         * @param t_fileName The filename under which to save the map.
         */
        void Init(int t_width, int t_height, const std::string& t_fileName);

        //-------------------------------------------------
        // Create map
        //-------------------------------------------------

        std::vector<MapTile> CreateTerrainLayer();
        std::vector<MapTile> CreateBuildingsLayer();
    };
}
