#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <memory>

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::ogl
{
    class Window;
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
        enum class Direction { LEFT, RIGHT, UP, DOWN };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The position of the camera.
         */
        glm::vec2 position{ 0.0f };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Camera() = delete;

        /**
         * Constructs a new Camera object.
         *
         * @param t_window The Window object.
         * @param t_position The camera position.
         */
        Camera(std::shared_ptr<ogl::Window> t_window, const glm::vec2& t_position);

        /**
         * Constructs a new Camera object.
         *
         * @param t_window The Window object.
         */
        explicit Camera(std::shared_ptr<ogl::Window> t_window);

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
        // Logic
        //-------------------------------------------------

        /**
         * Update camera position.
         */
        void Update();

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The Window object.
         */
        std::shared_ptr<ogl::Window> m_window;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Handle key input.
         */
        void ProcessKeyboard(Direction t_direction);
    };
}
