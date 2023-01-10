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

#include <glm/mat4x4.hpp>
#include <string>

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

/**
 * Forward declaration struct GLFWwindow.
 */
struct GLFWwindow;

//-------------------------------------------------
// Window
//-------------------------------------------------

namespace mdcii::ogl
{
    /**
     * Encapsulate all the window initialization code.
     */
    class Window
    {
    public:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * The default min width of the window.
         */
        static constexpr auto MIN_WIDTH{ 640 };

        /**
         * The default min height of the window.
         */
        static constexpr auto MIN_HEIGHT{ 480 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The width of the window.
         */
        int32_t width{ MIN_WIDTH };

        /**
         * The height of the window.
         */
        int32_t height{ MIN_HEIGHT };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Window();

        Window(const Window& t_other) = delete;
        Window(Window&& t_other) noexcept = delete;
        Window& operator=(const Window& t_other) = delete;
        Window& operator=(Window&& t_other) noexcept = delete;

        ~Window() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] const std::string& GetTitle() const;
        [[nodiscard]] const glm::mat4& GetOrthographicProjectionMatrix() const;
        [[nodiscard]] GLFWwindow* GetWindowHandle() const;

        //-------------------------------------------------
        // Rendering
        //-------------------------------------------------

        /**
         * Swaps the front and back buffers and checks if any events are triggered.
         */
        void SwapBuffersAndCallEvents() const;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Encapsulates glfwWindowShouldClose.
         *
         * @return boolean
         */
        [[nodiscard]] bool WindowShouldClose() const;

        /**
         * Sets the close flag value on true.
         */
        void Close() const;

        //-------------------------------------------------
        // Input Polling
        //-------------------------------------------------

        /**
         * Poll the key pressed status.
         *
         * @param t_keyCode A constant such as GLFW_KEY_W or GLFW_KEY_SPACE
         *
         * @return true if GLFW_PRESS or GLFW_REPEAT
         */
        [[nodiscard]] bool IsKeyPressed(int t_keyCode) const;

        /**
         * Poll the mouse button pressed status.
         *
         * @return true if GLFW_PRESS
         */
        [[nodiscard]] bool IsMouseButtonPressed(int t_button) const;

        /**
         * Returns the mouse position.
         *
         * @return glm::vec2
         */
        [[nodiscard]] glm::vec2 GetMousePosition() const;

        /**
         * Returns the mouse x position.
         *
         * @return The x position.
         */
        [[nodiscard]] float GetMouseX() const { return GetMousePosition().x; }

        /**
         * Returns the mouse y position.
         *
         * @return The y position.
         */
        [[nodiscard]] float GetMouseY() const { return GetMousePosition().y; }

        //-------------------------------------------------
        // ImGui
        //-------------------------------------------------

        /**
         * Start new ImGui frame.
         */
        static void ImGuiBegin();

        /**
         * Render ImGui into screen.
         */
        static void ImGuiEnd();

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The title of the window.
         */
        std::string m_title{ "MDCII" };

        /**
         * The orthographic projection matrix.
         */
        glm::mat4 m_orthographicProjectionMatrix{ glm::mat4(1.0f) };

        /**
         * The glfw window handle.
         */
        GLFWwindow* m_windowHandle{ nullptr };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializing class.
         */
        void Init();

        /**
         * Loads values from config.ini.
         */
        void LoadConfig();

        /**
         * The Window initialization code.
         */
        void InitWindow();

        /**
         * Init projection matrices.
         */
        void InitProjectionMatrix();

        /**
         * Initializing ImGui.
         */
        void InitImGui() const;

        /**
         * Initializing input callbacks.
         */
        void InitInputCallbacks() const;

        //-------------------------------------------------
        // Projection matrix
        //-------------------------------------------------

        /**
         * Update orthographic projection matrix.
         */
        void UpdateOrthographicProjectionMatrix();

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up.
         */
        void CleanUp() const;
    };
}
