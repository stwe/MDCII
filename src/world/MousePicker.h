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

#include "Zoom.h"
#include "ogl/Window.h"
#include "camera/Camera.h"
#include "event/EventManager.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::renderer
{
    /**
     * Forward declaration class TileRenderer.
     */
    class TileRenderer;
}

//-------------------------------------------------
// MousePicker
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * Forward declaration class World.
     */
    class World;

    /**
     * Represents a mouse picker to select tiles.
     */
    class MousePicker
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The current tile position under the mouse.
         */
        glm::ivec2 currentPosition{ -1 };

        /**
         * The last tile position under the mouse.
         */
        glm::ivec2 lastPosition{ -1 };

        /**
         * Indicates whether the mouse is in a new tile position since the last movement.
         */
        bool tilePositionHasChanged{ false };

        /**
         * Indicates whether the mouse is in the window.
         */
        bool inWindow{ false };

        /**
         * Specifies whether the terrain height should be taken into
         * account when calculating the positions.
         */
        bool calcForIslandTerrain{ false };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        /**
         * Constructs a new MousePicker object.
         *
         * @param t_world The parent World object.
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         */
        MousePicker(World* t_world, const ogl::Window& t_window, const camera::Camera& t_camera);

        MousePicker(const MousePicker& t_other) = delete;
        MousePicker(MousePicker&& t_other) noexcept = delete;
        MousePicker& operator=(const MousePicker& t_other) = delete;
        MousePicker& operator=(MousePicker&& t_other) noexcept = delete;

        ~MousePicker() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * Renders the mouse cursor.
         *
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         */
        void Render(const ogl::Window& t_window, const camera::Camera& t_camera);

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
         * The parent World object.
         */
        World* m_world{ nullptr };

        /**
         * The world width.
         */
        int32_t m_worldWidth{ -1 };

        /**
         * The world height.
         */
        int32_t m_worldHeight{ -1 };

        /**
         * A renderer to show the mouse cursor in the form of a tile.
         */
        std::unique_ptr<renderer::TileRenderer> m_renderer;

        /**
         * Cheat images pixel data for each zoom level (SGFX, MGFX, GFX).
         */
        std::array<unsigned char*, NR_OF_ZOOMS> m_cheatImages{ nullptr, nullptr, nullptr };

        /**
         * Each zoom level has a different mouse cursor texture.
         */
        std::array<std::string, NR_OF_ZOOMS> m_cursorFileNames{};

        /**
         * The mouse enter listener handle.
         */
        decltype(event::EventManager::event_dispatcher)::Handle m_mouseEnter;

        /**
         * The mouse moved listener handle.
         */
        decltype(event::EventManager::event_dispatcher)::Handle m_mouseMoved;

        //-------------------------------------------------
        // Event handler
        //-------------------------------------------------

        /**
         * Sets the information whether the mouse is in the window or not.
         *
         * @param t_inWindow Indicates whether the mouse is in the window.
         */
        void OnMouseEnter(const bool t_inWindow) { inWindow = t_inWindow; }

        /**
         * Updates the current and last tile world positions under the mouse.
         *
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         */
        void OnMouseMoved(const ogl::Window& t_window, const camera::Camera& t_camera);

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Gets the tile world position under the mouse.
         *
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         *
         * @return The tile world position under the mouse.
         */
        [[nodiscard]] glm::ivec2 GetWorldPosition(const ogl::Window& t_window, const camera::Camera& t_camera) const;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes the class.
         */
        void Init();

        /**
         * Adds event listeners.
         *
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         */
        void AddListeners(const ogl::Window& t_window, const camera::Camera& t_camera);

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up.
         */
        void CleanUp() const;
    };
}
