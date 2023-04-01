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
#include <memory>
#include <glm/vec2.hpp>

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::state
{
    /**
     * Forward declaration struct Context.
     */
    struct Context;

    /**
     * Forward declaration enum class StateId.
     */
    enum class StateId;
}

//-------------------------------------------------
// World
//-------------------------------------------------

namespace mdcii::world
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * Forward declaration enum class Rotation.
     */
    enum class Rotation;

    /**
     * Forward declaration enum class Zoom.
     */
    enum class Zoom;

    /**
     * Forward declaration enum class ChangeRotation.
     */
    enum class ChangeRotation;

    /**
     * Forward declaration enum class ChangeZoom.
     */
    enum class ChangeZoom;

    /**
     * Forward declaration class MousePicker.
     */
    class MousePicker;

    //-------------------------------------------------
    // World
    //-------------------------------------------------

    /**
     * The world base class.
     */
    class World
    {
    public:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * The min width of the world.
         */
        static constexpr auto WORLD_MIN_WIDTH{ 50 };

        /**
         * The min height of the world.
         */
        static constexpr auto WORLD_MIN_HEIGHT{ 35 };

        /**
         * The max width of the world.
         */
        static constexpr auto WORLD_MAX_WIDTH{ 500 };

        /**
         * The max height of the world.
         */
        static constexpr auto WORLD_MAX_HEIGHT{ 350 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * To have access to the shared objects (Window, Camera, Original-Assets, Mdcii-Assets).
         */
        std::shared_ptr<state::Context> context;

        /**
         * The parent StateId.
         */
        state::StateId stateId;

        /**
         * The world width.
         */
        int32_t width{ WORLD_MIN_WIDTH };

        /**
         * The world height.
         */
        int32_t height{ WORLD_MIN_HEIGHT };

        /**
         * The world rotation.
         */
        Rotation rotation;

        /**
         * The world zoom.
         */
        Zoom zoom;

        /**
         * A MousePicker object for the world.
         */
        std::unique_ptr<MousePicker> mousePicker;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        World() = delete;

        /**
         * Constructs a new World object.
         *
         * @param t_context Access to shared objects.
         * @param t_stateId The parent StateId.
         */
        World(std::shared_ptr<state::Context> t_context, state::StateId t_stateId);

        World(const World& t_other) = delete;
        World(World&& t_other) noexcept = delete;
        World& operator=(const World& t_other) = delete;
        World& operator=(World&& t_other) noexcept = delete;

        virtual ~World() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * Updates the world.
         */
        virtual void Update() = 0;

        /**
         * Renders the world.
         */
        virtual void Render() = 0;

        /**
         * Renders ImGui menus.
         */
        virtual void RenderImGui() = 0;

        //-------------------------------------------------
        // Rotate && Zoom
        //-------------------------------------------------

        /**
         * Changes the rotation of the world.
         *
         * @param t_changeRotation Rotate left or right.
         */
        virtual void RotateWorld(ChangeRotation t_changeRotation);

        /**
         * Changes the zoom of the world.
         *
         * @param t_changeZoom Zoom in or out.
         */
        virtual void ZoomWorld(ChangeZoom t_changeZoom);

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Checks whether a given position is in the world width and height.
         *
         * @param t_x The x position to check.
         * @param t_y The y position to check.
         *
         * @return True or false.
         */
        [[nodiscard]] bool IsPositionInWorld(int32_t t_x, int32_t t_y) const;

        /**
         * Checks whether a given position is in the world width and height.
         *
         * @param t_position The position to check.
         *
         * @return True or false.
         */
        [[nodiscard]] bool IsPositionInWorld(const glm::ivec2& t_position) const;

        /**
         * Projects a position into an isometric position on the screen.
         *
         * @param t_x The x position.
         * @param t_y The y position.
         * @param t_zoom The zoom to get the tile sizes.
         * @param t_rotation The position is previously rotated by the specified value.
         *
         * @return The isometric coordinates on the screen.
         */
        [[nodiscard]] glm::vec2 WorldToScreen(int32_t t_x, int32_t t_y, Zoom t_zoom, Rotation t_rotation) const;

    protected:

    private:

    };
}
