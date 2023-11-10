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

#include "world/Zoom.h"
#include "world/Rotation.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * @brief Forward declaration class World.
     */
    class World;
}

namespace mdcii::camera
{
    //-------------------------------------------------
    // Camera
    //-------------------------------------------------

    /**
     * @brief Represents a camera.
     */
    class Camera
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The current zoom.
         */
        world::Zoom zoom{ world::Zoom::GFX };

        /**
         * @brief The current rotation.
         */
        world::Rotation rotation{ world::Rotation::DEG0 };

        /**
         * @brief Where to place first tile (0, 0) on screen (in tile size steps).
         */
        olc::vf2d origin{ 0.0f, 0.0f };

        /**
         * @brief The world position of the camera.
         */
        olc::vi2d worldPosition{ 0, 0 };

        /**
         * @brief The screen position of the camera.
         */
        olc::vi2d screenPosition{ 0, 0 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Camera() = delete;

        /**
         * @brief Constructs a new Camera object.
         *
         * @param t_world Pointer to the parent World.
         */
        explicit Camera(const world::World* t_world);

        Camera(const Camera& t_other) = delete;
        Camera(Camera&& t_other) noexcept = delete;
        Camera& operator=(const Camera& t_other) = delete;
        Camera& operator=(Camera&& t_other) noexcept = delete;

        ~Camera() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        [[nodiscard]] bool OnUserUpdate(float t_elapsedTime);

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The speed of the camera.
         */
        static constexpr auto SPEED{ 32.0f };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the parent World.
         */
        const world::World* m_world{ nullptr };
    };
}
