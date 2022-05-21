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
    class Camera
    {
    public:
        enum class Direction { LEFT, RIGHT, UP, DOWN };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        glm::vec2 position{ 0.0f };
        float velocity{ 2.0f };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Camera() = delete;
        Camera(std::shared_ptr<ogl::Window> t_window, const glm::vec2& t_position);

        Camera(const Camera& t_other) = delete;
        Camera(Camera&& t_other) noexcept = delete;
        Camera& operator=(const Camera& t_other) = delete;
        Camera& operator=(Camera&& t_other) noexcept = delete;

        ~Camera() noexcept;

        //-------------------------------------------------
        // View matrix
        //-------------------------------------------------

        [[nodiscard]] glm::mat4 GetViewMatrix() const noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Update();

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        std::shared_ptr<ogl::Window> m_window;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        void ProcessKeyboard(Direction t_direction);
    };
}
