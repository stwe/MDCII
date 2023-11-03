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

#include <vector>
#include "vendor/olc/olcPixelGameEngine.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii
{
    /**
     * @brief Forward declaration class GameState.
     */
    class GameState;
}

namespace mdcii::renderer
{
    /**
     * @brief Forward declaration class Renderer.
     */
    class Renderer;
}

namespace mdcii::camera
{
    /**
     * @brief Forward declaration class Camera.
     */
    class Camera;
}

namespace mdcii::world
{
    /**
     * @brief Forward declaration class Island.
     */
    class Island;

    //-------------------------------------------------
    // World
    //-------------------------------------------------

    /**
     * @brief Represents the game world.
     */
    class World
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the parent GameState.
         */
        GameState* gameState{ nullptr };

        /**
         * @brief The width of the world in tiles.
         */
        int worldWidth{ 64 };

        /**
         * @brief The height of the world in tiles.
         */
        int worldHeight{ 48 };

        /**
         * @brief A collection of unique pointers to Island objects.
         */
        std::vector<std::unique_ptr<Island>> islands;

        /**
         * @brief For rendering this world.
         */
        std::unique_ptr<renderer::Renderer> renderer;

        /**
         * @brief A camera to move around.
         */
        std::unique_ptr<camera::Camera> camera;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        World() = delete;

        /**
         * @brief Constructs a new World object.
         *
         * @param t_gameState Pointer to the parent GameState.
         */
        explicit World(GameState* t_gameState);

        World(const World& t_other) = delete;
        World(World&& t_other) noexcept = delete;
        World& operator=(const World& t_other) = delete;
        World& operator=(World&& t_other) noexcept = delete;

        ~World() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void OnUserUpdate(float t_elapsedTime, const olc::vf2d& t_vel);

    protected:

    private:
        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * @brief Initializes the game world.
         *
         * Called upon construction of a World object.
         */
        void Init();
    };
}
