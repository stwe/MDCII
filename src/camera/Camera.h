#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

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

        Camera();

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

    protected:

    private:
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
    };
}
