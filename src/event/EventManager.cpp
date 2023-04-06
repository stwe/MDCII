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
            if (auto& io{ ImGui::GetIO() }; io.WantCaptureKeyboard && io.WantCaptureMouse)
            {
                io.AddKeyEvent(KeyToImGuiKey(t_key), (t_action == GLFW_PRESS));
            }
            else
            {
                switch (t_action)
                {
                case GLFW_PRESS:
                    event_dispatcher.dispatch(MdciiEventType::KEY_PRESSED, KeyPressedEvent(t_key));
                    break;
                case GLFW_RELEASE:
                    event_dispatcher.dispatch(MdciiEventType::KEY_RELEASED, KeyReleasedEvent(t_key));
                    break;
                case GLFW_REPEAT:
                    event_dispatcher.dispatch(MdciiEventType::KEY_PRESSED, KeyPressedEvent(t_key, 1));
                    break;
                default:;
                }
            }
        }
    );
}

void mdcii::event::EventManager::SetMouseGlfwCallbacks(GLFWwindow* t_windowHandle)
{
    // Registers a callback that will be invoked when the mouse is moved.
    glfwSetCursorPosCallback(
        t_windowHandle,
        [](GLFWwindow* t_window, const double t_x, const double t_y) {
            if (const auto& io{ ImGui::GetIO() }; !io.WantCaptureMouse)
            {
                event_dispatcher.dispatch(
                    MdciiEventType::MOUSE_MOVED,
                    MouseMovedEvent(
                        static_cast<float>(t_x),
                        static_cast<float>(t_y)
                    )
                );
            }
        }
    );

    // Registers a callback that will be invoked when a scrolling device is used.
    glfwSetScrollCallback(
        t_windowHandle,
        [](GLFWwindow* t_window, const double t_xOffset, const double t_yOffset) {
            auto& io{ ImGui::GetIO() };
            io.AddMouseWheelEvent(static_cast<float>(t_xOffset), static_cast<float>(t_yOffset));

            if (!io.WantCaptureMouse)
            {
                event_dispatcher.dispatch(
                    MdciiEventType::MOUSE_SCROLLED,
                    MouseScrolledEvent(
                        static_cast<float>(t_xOffset),
                        static_cast<float>(t_yOffset)
                    )
                );
            }
        }
    );

    // Registers a callback that will be invoked when the mouse enters our window.
    glfwSetCursorEnterCallback(
        t_windowHandle,
        [](GLFWwindow* t_window, const int t_entered) {
            event_dispatcher.dispatch(MdciiEventType::MOUSE_ENTER, MouseEnterEvent(t_entered));
        }
    );

    // Registers a callback that will be invoked when a mouse button is pressed or released.
    glfwSetMouseButtonCallback(
        t_windowHandle,
        [](GLFWwindow* t_window, const int t_button, const int t_action, const int t_mods) {
            auto& io{ ImGui::GetIO() };
            io.AddMouseButtonEvent(t_button, t_action);

            if (!io.WantCaptureMouse)
            {
                switch (t_action)
                {
                case GLFW_PRESS:
                    event_dispatcher.dispatch(MdciiEventType::MOUSE_BUTTON_PRESSED, MouseButtonPressedEvent(t_button));
                    break;
                case GLFW_RELEASE:
                    event_dispatcher.dispatch(MdciiEventType::MOUSE_BUTTON_RELEASED, MouseButtonReleasedEvent(t_button));
                    break;
                default:;
                }
            }
        }
    );
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

ImGuiKey mdcii::event::EventManager::KeyToImGuiKey(const int t_key)
{
    switch (t_key)
    {
    case GLFW_KEY_TAB:
        return ImGuiKey_Tab;
    case GLFW_KEY_LEFT:
        return ImGuiKey_LeftArrow;
    case GLFW_KEY_RIGHT:
        return ImGuiKey_RightArrow;
    case GLFW_KEY_UP:
        return ImGuiKey_UpArrow;
    case GLFW_KEY_DOWN:
        return ImGuiKey_DownArrow;
    case GLFW_KEY_PAGE_UP:
        return ImGuiKey_PageUp;
    case GLFW_KEY_PAGE_DOWN:
        return ImGuiKey_PageDown;
    case GLFW_KEY_HOME:
        return ImGuiKey_Home;
    case GLFW_KEY_END:
        return ImGuiKey_End;
    case GLFW_KEY_INSERT:
        return ImGuiKey_Insert;
    case GLFW_KEY_DELETE:
        return ImGuiKey_Delete;
    case GLFW_KEY_BACKSPACE:
        return ImGuiKey_Backspace;
    case GLFW_KEY_SPACE:
        return ImGuiKey_Space;
    case GLFW_KEY_ENTER:
        return ImGuiKey_Enter;
    case GLFW_KEY_ESCAPE:
        return ImGuiKey_Escape;
    case GLFW_KEY_APOSTROPHE:
        return ImGuiKey_Apostrophe;
    case GLFW_KEY_COMMA:
        return ImGuiKey_Comma;
    case GLFW_KEY_MINUS:
        return ImGuiKey_Minus;
    case GLFW_KEY_PERIOD:
        return ImGuiKey_Period;
    case GLFW_KEY_SLASH:
        return ImGuiKey_Slash;
    case GLFW_KEY_SEMICOLON:
        return ImGuiKey_Semicolon;
    case GLFW_KEY_EQUAL:
        return ImGuiKey_Equal;
    case GLFW_KEY_LEFT_BRACKET:
        return ImGuiKey_LeftBracket;
    case GLFW_KEY_BACKSLASH:
        return ImGuiKey_Backslash;
    case GLFW_KEY_RIGHT_BRACKET:
        return ImGuiKey_RightBracket;
    case GLFW_KEY_GRAVE_ACCENT:
        return ImGuiKey_GraveAccent;
    case GLFW_KEY_CAPS_LOCK:
        return ImGuiKey_CapsLock;
    case GLFW_KEY_SCROLL_LOCK:
        return ImGuiKey_ScrollLock;
    case GLFW_KEY_NUM_LOCK:
        return ImGuiKey_NumLock;
    case GLFW_KEY_PRINT_SCREEN:
        return ImGuiKey_PrintScreen;
    case GLFW_KEY_PAUSE:
        return ImGuiKey_Pause;
    case GLFW_KEY_KP_0:
        return ImGuiKey_Keypad0;
    case GLFW_KEY_KP_1:
        return ImGuiKey_Keypad1;
    case GLFW_KEY_KP_2:
        return ImGuiKey_Keypad2;
    case GLFW_KEY_KP_3:
        return ImGuiKey_Keypad3;
    case GLFW_KEY_KP_4:
        return ImGuiKey_Keypad4;
    case GLFW_KEY_KP_5:
        return ImGuiKey_Keypad5;
    case GLFW_KEY_KP_6:
        return ImGuiKey_Keypad6;
    case GLFW_KEY_KP_7:
        return ImGuiKey_Keypad7;
    case GLFW_KEY_KP_8:
        return ImGuiKey_Keypad8;
    case GLFW_KEY_KP_9:
        return ImGuiKey_Keypad9;
    case GLFW_KEY_KP_DECIMAL:
        return ImGuiKey_KeypadDecimal;
    case GLFW_KEY_KP_DIVIDE:
        return ImGuiKey_KeypadDivide;
    case GLFW_KEY_KP_MULTIPLY:
        return ImGuiKey_KeypadMultiply;
    case GLFW_KEY_KP_SUBTRACT:
        return ImGuiKey_KeypadSubtract;
    case GLFW_KEY_KP_ADD:
        return ImGuiKey_KeypadAdd;
    case GLFW_KEY_KP_ENTER:
        return ImGuiKey_KeypadEnter;
    case GLFW_KEY_KP_EQUAL:
        return ImGuiKey_KeypadEqual;
    case GLFW_KEY_LEFT_SHIFT:
        return ImGuiKey_LeftShift;
    case GLFW_KEY_LEFT_CONTROL:
        return ImGuiKey_LeftCtrl;
    case GLFW_KEY_LEFT_ALT:
        return ImGuiKey_LeftAlt;
    case GLFW_KEY_LEFT_SUPER:
        return ImGuiKey_LeftSuper;
    case GLFW_KEY_RIGHT_SHIFT:
        return ImGuiKey_RightShift;
    case GLFW_KEY_RIGHT_CONTROL:
        return ImGuiKey_RightCtrl;
    case GLFW_KEY_RIGHT_ALT:
        return ImGuiKey_RightAlt;
    case GLFW_KEY_RIGHT_SUPER:
        return ImGuiKey_RightSuper;
    case GLFW_KEY_MENU:
        return ImGuiKey_Menu;
    case GLFW_KEY_0:
        return ImGuiKey_0;
    case GLFW_KEY_1:
        return ImGuiKey_1;
    case GLFW_KEY_2:
        return ImGuiKey_2;
    case GLFW_KEY_3:
        return ImGuiKey_3;
    case GLFW_KEY_4:
        return ImGuiKey_4;
    case GLFW_KEY_5:
        return ImGuiKey_5;
    case GLFW_KEY_6:
        return ImGuiKey_6;
    case GLFW_KEY_7:
        return ImGuiKey_7;
    case GLFW_KEY_8:
        return ImGuiKey_8;
    case GLFW_KEY_9:
        return ImGuiKey_9;
    case GLFW_KEY_A:
        return ImGuiKey_A;
    case GLFW_KEY_B:
        return ImGuiKey_B;
    case GLFW_KEY_C:
        return ImGuiKey_C;
    case GLFW_KEY_D:
        return ImGuiKey_D;
    case GLFW_KEY_E:
        return ImGuiKey_E;
    case GLFW_KEY_F:
        return ImGuiKey_F;
    case GLFW_KEY_G:
        return ImGuiKey_G;
    case GLFW_KEY_H:
        return ImGuiKey_H;
    case GLFW_KEY_I:
        return ImGuiKey_I;
    case GLFW_KEY_J:
        return ImGuiKey_J;
    case GLFW_KEY_K:
        return ImGuiKey_K;
    case GLFW_KEY_L:
        return ImGuiKey_L;
    case GLFW_KEY_M:
        return ImGuiKey_M;
    case GLFW_KEY_N:
        return ImGuiKey_N;
    case GLFW_KEY_O:
        return ImGuiKey_O;
    case GLFW_KEY_P:
        return ImGuiKey_P;
    case GLFW_KEY_Q:
        return ImGuiKey_Q;
    case GLFW_KEY_R:
        return ImGuiKey_R;
    case GLFW_KEY_S:
        return ImGuiKey_S;
    case GLFW_KEY_T:
        return ImGuiKey_T;
    case GLFW_KEY_U:
        return ImGuiKey_U;
    case GLFW_KEY_V:
        return ImGuiKey_V;
    case GLFW_KEY_W:
        return ImGuiKey_W;
    case GLFW_KEY_X:
        return ImGuiKey_X;
    case GLFW_KEY_Y:
        return ImGuiKey_Y;
    case GLFW_KEY_Z:
        return ImGuiKey_Z;
    case GLFW_KEY_F1:
        return ImGuiKey_F1;
    case GLFW_KEY_F2:
        return ImGuiKey_F2;
    case GLFW_KEY_F3:
        return ImGuiKey_F3;
    case GLFW_KEY_F4:
        return ImGuiKey_F4;
    case GLFW_KEY_F5:
        return ImGuiKey_F5;
    case GLFW_KEY_F6:
        return ImGuiKey_F6;
    case GLFW_KEY_F7:
        return ImGuiKey_F7;
    case GLFW_KEY_F8:
        return ImGuiKey_F8;
    case GLFW_KEY_F9:
        return ImGuiKey_F9;
    case GLFW_KEY_F10:
        return ImGuiKey_F10;
    case GLFW_KEY_F11:
        return ImGuiKey_F11;
    case GLFW_KEY_F12:
        return ImGuiKey_F12;
    default:
        return ImGuiKey_None;
    }
}
