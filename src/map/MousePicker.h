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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#pragma once

#include <memory>
#include <array>
#include "Zoom.h"
#include "ogl/Window.h"
#include "camera/Camera.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::renderer
{
    class TileRenderer;
}

//-------------------------------------------------
// MousePicker
//-------------------------------------------------

namespace mdcii::map
{
    /**
     * Forward declaration class Map.
     */
    class Map;

    /**
     * Represents a mouse picker to select tiles.
     */
    class MousePicker
    {
    public:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        static constexpr auto LEFT_MOUSE_BUTTON{ 0 };
        static constexpr auto RIGHT_MOUSE_BUTTON{ 1 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        /**
         * Constructs a new MousePicker object.
         *
         * @param t_map The parent Map object.
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         */
        MousePicker(Map* t_map, const ogl::Window& t_window, const camera::Camera& t_camera);

        MousePicker(const MousePicker& t_other) = delete;
        MousePicker(MousePicker&& t_other) noexcept = delete;
        MousePicker& operator=(const MousePicker& t_other) = delete;
        MousePicker& operator=(MousePicker&& t_other) noexcept = delete;

        ~MousePicker() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * Render method for the mouse cursor.
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
         * The parent Map object.
         */
        Map* m_map{ nullptr };

        /**
         * The current tile map position under the mouse.
         */
        glm::ivec2 m_currentPosition{ -1 };

        /**
         * The last tile map position under the mouse.
         */
        glm::ivec2 m_lastPosition{ -1 };

        /**
         * A renderer for highlighting tiles.
         */
        std::unique_ptr<renderer::TileRenderer> m_renderer;

        /**
         * One cheat image for each zoom level (SGFX, MGFX, GFX).
         */
        std::array<unsigned char*, NR_OF_ZOOMS> m_cornerImages{ nullptr, nullptr, nullptr };

        /**
         * Indicates whether the mouse is in the window.
         */
        bool m_inWindow{ false };

        /**
         * Each zoom level has a different mouse cursor texture.
         */
        std::array<std::string, NR_OF_ZOOMS> m_cursorFileNames{};

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * Updates the tile map positions under the mouse.
         *
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         */
        void UpdatePositions(const ogl::Window& t_window, const camera::Camera& t_camera);

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Get the tile map position under the mouse.
         *
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         *
         * @return The map position of the mouse.
         */
        [[nodiscard]] glm::ivec2 GetMapPosition(const ogl::Window& t_window, const camera::Camera& t_camera) const;

        /**
         * Checks whether the current mouse position is in map.
         *
         * @return True or false depending on the position in the map.
         */
        [[nodiscard]] bool IsCurrentMouseInMap() const;

        /**
         * Checks whether the last mouse position is in map.
         *
         * @return True or false depending on the position in the map.
         */
        [[nodiscard]] bool IsLastMouseInMap() const;

        //-------------------------------------------------
        // Cursor
        //-------------------------------------------------

        /**
         * Renders a texture as a mouse cursor.
         *
         * @param t_window The Window object.
         * @param t_camera The Camera object.
         */
        void RenderMouseCursor(const ogl::Window& t_window, const camera::Camera& t_camera) const;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initialize class.
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
