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

#include <imgui.h>
#include "EventManager.h"

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::event::EventManager::SetKeyboardGlfwCallbacks(GLFWwindow* t_windowHandle)
{
    // Registers a callback that will be invoked when a key is pressed or released
    glfwSetKeyCallback(
        t_windowHandle,
        [](GLFWwindow* t_window, const int t_key, const int t_scanCode, const int t_action, const int t_mods) {
            // stuff needed by ImGui
            auto& io{ ImGui::GetIO() };
            if (t_key >= 0 && t_key < IM_ARRAYSIZE(io.KeysDown))
            {
                if (t_action == GLFW_PRESS)
                {
                    io.KeysDown[t_key] = true;
                }

                if (t_action == GLFW_RELEASE)
                {
                    io.KeysDown[t_key] = false;
                }
            }

            // MDCII
            switch (t_action)
            {
            case GLFW_PRESS:
                eventDispatcher.dispatch(MdciiEventType::KEY_PRESSED, KeyPressedEvent(t_key));
                break;
            case GLFW_RELEASE:
                eventDispatcher.dispatch(MdciiEventType::KEY_RELEASED, KeyReleasedEvent(t_key));
                break;
            case GLFW_REPEAT:
                eventDispatcher.dispatch(MdciiEventType::KEY_PRESSED, KeyPressedEvent(t_key, 1));
                break;
            default:;
            }
        }
    );

    // Registers a callback that will be called when a Unicode character is input.
    glfwSetCharCallback(
        // stuff needed by ImGui
        t_windowHandle,
        [](GLFWwindow* t_window, const unsigned int t_char) {
            ImGuiIO& io = ImGui::GetIO();
            io.AddInputCharacter(t_char);
        }
    );
}

void mdcii::event::EventManager::SetMouseGlfwCallbacks(GLFWwindow* t_windowHandle)
{
    // Registers a callback that will be invoked when the mouse is moved.
    glfwSetCursorPosCallback(
        t_windowHandle,
        [](GLFWwindow* t_window, const double t_x, const double t_y) {
            eventDispatcher.dispatch(
                MdciiEventType::MOUSE_MOVED,
                MouseMovedEvent(
                    static_cast<float>(t_x),
                    static_cast<float>(t_y)
                )
            );
        }
    );

    // Registers a callback that will be invoked when a scrolling device is used.
    glfwSetScrollCallback(
        t_windowHandle,
        [](GLFWwindow* t_window, const double t_xOffset, const double t_yOffset) {
            // stuff needed by ImGui
            ImGuiIO& io = ImGui::GetIO();
            io.MouseWheelH += static_cast<float>(t_xOffset);
            io.MouseWheel += static_cast<float>(t_yOffset);

            // MDCII
            eventDispatcher.dispatch(
                MdciiEventType::MOUSE_SCROLLED,
                MouseScrolledEvent(
                    static_cast<float>(t_xOffset),
                    static_cast<float>(t_yOffset)
                )
            );
        }
    );

    // Registers a callback that will be invoked when the mouse enters our window.
    glfwSetCursorEnterCallback(
        t_windowHandle,
        [](GLFWwindow* t_window, const int t_entered) {
            eventDispatcher.dispatch(MdciiEventType::MOUSE_ENTER, MouseEnterEvent(t_entered));
        }
    );

    // Registers a callback that will be invoked when a mouse button is pressed or released.
    glfwSetMouseButtonCallback(
        t_windowHandle,
        [](GLFWwindow* t_window, const int t_button, const int t_action, const int t_mods) {
            switch (t_action)
            {
            case GLFW_PRESS:
                eventDispatcher.dispatch(MdciiEventType::MOUSE_BUTTON_PRESSED, MouseButtonPressedEvent(t_button));
                break;
            case GLFW_RELEASE:
                eventDispatcher.dispatch(MdciiEventType::MOUSE_BUTTON_RELEASED, MouseButtonReleasedEvent(t_button));
                break;
            default:;
            }
        }
    );
}
