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

#include <glm/vec2.hpp>
#include "data/json.hpp"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::state
{
    /**
     * Forward declaration struct Context.
     */
    struct Context;
}

//-------------------------------------------------
// Terrain
//-------------------------------------------------

namespace mdcii::world
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * Forward declaration class World.
     */
    class World;

    /**
     * Forward declaration class Island.
     */
    class Island;

    //-------------------------------------------------
    // Terrain
    //-------------------------------------------------

    /**
     * Represents the terrain of the world.
     */
    class Terrain
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The parent World object.
         */
        World* world{ nullptr };

        /**
         * The Island objects of the Terrain.
         */
        std::vector<std::unique_ptr<Island>> islands;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Terrain() = delete;

        /**
         * Constructs a new Terrain object.
         *
         * @param t_context Access to shared objects.
         * @param t_world The parent World object.
         */
        Terrain(std::shared_ptr<state::Context> t_context, World* t_world);

        Terrain(const Terrain& t_other) = delete;
        Terrain(Terrain&& t_other) noexcept = delete;
        Terrain& operator=(const Terrain& t_other) = delete;
        Terrain& operator=(Terrain&& t_other) noexcept = delete;

        ~Terrain() noexcept;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Creates the Island objects from a Json value.
         *
         * @param t_json The Json value.
         */
        void CreateIslandsFromJson(const nlohmann::json& t_json);

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * Checks whether a given position on any Island.
         * The Aabb of the Island objects is used for this test.
         *
         * @param t_position The position to be checked.
         *
         * @return True or false.
         */
        [[nodiscard]] bool IsPositionOnTerrain(const glm::ivec2& t_position) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * To have access to the shared objects (Window, Camera, Original-Assets).
         */
        std::shared_ptr<state::Context> m_context;

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up.
         */
        void CleanUp() const;
    };
}
