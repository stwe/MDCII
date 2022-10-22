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

#include <string>

//-------------------------------------------------
// WorldGenerator
//-------------------------------------------------

namespace mdcii::world
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * Forward declaration struct Tile.
     */
    struct Tile;

    //-------------------------------------------------
    // WorldGenerator
    //-------------------------------------------------

    /**
     * Creates a world map.
     */
    class WorldGenerator
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

        WorldGenerator() = delete;

        /**
         * Constructs a new WorldGenerator object.
         *
         * @param t_width The width of the world to create.
         * @param t_height The height of the world to create.
         * @param t_fileName The filename under which to save the world.
         */
        WorldGenerator(int t_width, int t_height, const std::string& t_fileName);

        WorldGenerator(const WorldGenerator& t_other) = delete;
        WorldGenerator(WorldGenerator&& t_other) noexcept = delete;
        WorldGenerator& operator=(const WorldGenerator& t_other) = delete;
        WorldGenerator& operator=(WorldGenerator&& t_other) noexcept = delete;

        ~WorldGenerator() noexcept;

    protected:

    private:
        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Creates a world.
         *
         * @param t_width The width of the world to create.
         * @param t_height The height of the world to create.
         * @param t_fileName The filename under which to save the world.
         */
        static void Init(int t_width, int t_height, const std::string& t_fileName);
    };
}
