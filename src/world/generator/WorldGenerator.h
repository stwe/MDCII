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

#pragma once

#include "vendor/nlohmann/json.hpp"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::state
{
    /**
     * @brief Forward declaration class State.
     */
    class State;
}

namespace mdcii::world
{
    /**
     * @brief Forward declaration class World.
     */
    class World;

    /**
     * @brief Forward declaration class Island.
     */
    class Island;
}

namespace mdcii::world::generator
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
        // Member
        //-------------------------------------------------

        std::unique_ptr<world::World> world;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        WorldGenerator() = delete;

        /**
         * @brief Constructs a new WorldGenerator object.
         *
         * @param t_state Pointer to the parent State object.
         */
        explicit WorldGenerator(state::State* t_state);

        WorldGenerator(const WorldGenerator& t_other) = delete;
        WorldGenerator(WorldGenerator&& t_other) noexcept = delete;
        WorldGenerator& operator=(const WorldGenerator& t_other) = delete;
        WorldGenerator& operator=(WorldGenerator&& t_other) noexcept = delete;

        ~WorldGenerator() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void OnUserUpdate(float t_elapsedTime);

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        inline static int m_island_file_index{ 0 };

        state::State* m_state{ nullptr };

        Island* m_currentIsland{ nullptr };

        void SaveWorldImGui() const;
    };
}
