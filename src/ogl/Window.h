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
         * The field of view.
         */
        float fovDeg{ 60.0f };

        /**
         * The near plane of the frustum.
         */
        float nearPlane{ 0.01f };

        /**
         * The far plane of the frustum.
         */
        float farPlane{ 5000.0f };

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
        [[nodiscard]] int GetWidth() const;
        [[nodiscard]] int GetHeight() const;
        [[nodiscard]] const glm::mat4& GetProjectionMatrix() const;
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
         * The width of the window.
         */
        int m_width{ MIN_WIDTH };

        /**
         * The height of the window.
         */
        int m_height{ MIN_HEIGHT };

        /**
         * The projection matrix.
         */
        glm::mat4 m_projectionMatrix{ glm::mat4(1.0f) };

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
         * Update projection matrix.
         */
        void UpdateProjectionMatrix();

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
