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

#include "vendor/nlohmann/json.hpp"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii
{
    /**
     * @brief Forward declaration class Game.
     */
    class Game;
}

namespace mdcii::world
{
    //-------------------------------------------------
    // WorldGenerator
    //-------------------------------------------------

    /**
     * @brief Represents a world generator.
     */
    class WorldGenerator
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        WorldGenerator() = delete;

        /**
         * @brief Constructs a new WorldGenerator object.
         *
         * @param t_game Pointer to the parent Game object.
         */
        explicit WorldGenerator(Game* t_game);

        WorldGenerator(const WorldGenerator& t_other) = delete;
        WorldGenerator(WorldGenerator&& t_other) noexcept = delete;
        WorldGenerator& operator=(const WorldGenerator& t_other) = delete;
        WorldGenerator& operator=(WorldGenerator&& t_other) noexcept = delete;

        ~WorldGenerator() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------


    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the parent Game object.
         */
        Game* m_game{ nullptr };
    };

    //-------------------------------------------------
    // Json
    //-------------------------------------------------

    void to_json(nlohmann::json& t_json, const WorldGenerator& t_worldGenerator);
}
