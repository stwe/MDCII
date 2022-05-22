#include <imgui.h>
#include "EventManager.h"

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::event::EventManager::SetKeyboardGlfwCallbacks(GLFWwindow* t_windowHandle)
{
    // Registers a callback that will be invoked when a key is pressed or released
    glfwSetKeyCallback
    (
        t_windowHandle,
        [](GLFWwindow* t_window, const int t_key, const int t_scanCode, const int t_action, const int t_mods)
        {
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
    glfwSetCharCallback
    (
        // stuff needed by ImGui
        t_windowHandle,
        [](GLFWwindow* t_window, const unsigned int t_char)
        {
            ImGuiIO& io = ImGui::GetIO();
            io.AddInputCharacter(t_char);
        }
    );
}

void mdcii::event::EventManager::SetMouseGlfwCallbacks(GLFWwindow* t_windowHandle)
{
    // Registers a callback that will be invoked when the mouse is moved.
    glfwSetCursorPosCallback
    (
        t_windowHandle,
        [](GLFWwindow* t_window, const double t_x, const double t_y)
        {
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
    glfwSetScrollCallback
    (
        t_windowHandle,
        [](GLFWwindow* t_window, const double t_xOffset, const double t_yOffset)
        {
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
    glfwSetCursorEnterCallback
    (
        t_windowHandle,
        [](GLFWwindow* t_window, const int t_entered)
        {
            eventDispatcher.dispatch(MdciiEventType::MOUSE_ENTER, MouseEnterEvent(t_entered));
        }
    );

    // Registers a callback that will be invoked when a mouse button is pressed or released.
    glfwSetMouseButtonCallback
    (
        t_windowHandle,
        [](GLFWwindow* t_window, const int t_button, const int t_action, const int t_mods)
        {
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
