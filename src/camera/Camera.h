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
#include "world/Island.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::physics
{
    /**
     * Forward declaration struct Aabb.
     */
    struct Aabb;
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
         * The current world zoom.
         */
        world::Zoom zoom;

        /**
         * The world position of the camera.
         */
        glm::ivec2 worldPosition{ 0 };

        /**
         * The screen position of the camera.
         */
        glm::vec2 position{ 0.0f };

        /**
         * An Aabb (axis-aligned bounding box) object for collision detection.
         */
        std::unique_ptr<physics::Aabb> aabb;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Camera() = delete;

        /**
         * Constructs an new Camera object.
         *
         * @param t_windowWidth The window width.
         * @param t_windowHeight The window height.
         */
        Camera(int32_t t_windowWidth, int32_t t_windowHeight);

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

        //-------------------------------------------------
        // Culling
        //-------------------------------------------------

        /**
         * Checks if there is an island in the camera's field of view.
         *
         * @param t_zoom The world zoom.
         * @param t_rotation The world rotation.
         * @param t_island The island to check.
         *
         * @return True or false.
         */
        [[nodiscard]] bool IsIslandNotInCamera(world::Zoom t_zoom, world::Rotation t_rotation, const world::Island& t_island) const;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * Renders ImGui stuff.
         */
        void RenderImGui() const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The window width.
         */
        int32_t m_windowWidth{ -1 };

        /**
         * The window height.
         */
        int32_t m_windowHeight{ -1 };

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
