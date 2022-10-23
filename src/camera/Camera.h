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
#include <glm/mat4x4.hpp>
#include "event/EventManager.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    enum class Zoom;
}

//-------------------------------------------------
// Camera
//-------------------------------------------------

namespace mdcii::camera
{
    /**
     * Represents an orthographic camera.
     */
    class Camera
    {
    public:
        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        /**
         * The movement possibilities of the camera.
         */
        enum class Direction
        {
            LEFT,
            RIGHT,
            UP,
            DOWN
        };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The position of the camera.
         */
        glm::vec2 position{ 0.0f };

        /**
         * The current zoom.
         */
        world::Zoom zoom;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Camera();

        Camera(const Camera& t_other) = delete;
        Camera(Camera&& t_other) noexcept = delete;
        Camera& operator=(const Camera& t_other) = delete;
        Camera& operator=(Camera&& t_other) noexcept = delete;

        ~Camera() noexcept;

        //-------------------------------------------------
        // View matrix
        //-------------------------------------------------

        /**
         * Creates the view matrix.
         *
         * @return The created view matrix.
         */
        [[nodiscard]] glm::mat4 GetViewMatrix() const noexcept;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The key pressed listener handle.
         */
        decltype(event::EventManager::event_dispatcher)::Handle m_keyPressed;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Handle key input.
         */
        void ProcessKeyboard(Direction t_direction);

        //-------------------------------------------------
        // Listeners
        //-------------------------------------------------

        /**
         * Adds listeners to change camera position.
         */
        void AddListeners();

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up.
         */
        void CleanUp() const;
    };
}
