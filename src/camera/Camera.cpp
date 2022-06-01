#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "Log.h"
#include "ogl/Window.h"
#include "ogl/OpenGL.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::camera::Camera::Camera(std::shared_ptr<ogl::Window> t_window, const glm::vec2& t_position)
    : m_window{ std::move(t_window) }
{
    Log::MDCII_LOG_DEBUG("[Camera::Camera()] Create Camera.");

    position = t_position;
}

mdcii::camera::Camera::~Camera() noexcept
{
    Log::MDCII_LOG_DEBUG("[Camera::~Camera()] Destruct Camera.");
}

//-------------------------------------------------
// View matrix
//-------------------------------------------------

glm::mat4 mdcii::camera::Camera::GetViewMatrix() const noexcept
{
    auto transformMatrix{ glm::mat4(1.0f) };
    transformMatrix = translate(transformMatrix, glm::vec3(position.x, position.y, 0.0f));

    return inverse(transformMatrix);
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::camera::Camera::Update()
{
    if (m_window->IsKeyPressed(GLFW_KEY_W) || m_window->IsKeyPressed(GLFW_KEY_UP))
    {
        ProcessKeyboard(Direction::UP);
    }

    if (m_window->IsKeyPressed(GLFW_KEY_S) || m_window->IsKeyPressed(GLFW_KEY_DOWN))
    {
        ProcessKeyboard(Direction::DOWN);
    }

    if (m_window->IsKeyPressed(GLFW_KEY_A) || m_window->IsKeyPressed(GLFW_KEY_LEFT))
    {
        ProcessKeyboard(Direction::LEFT);
    }

    if (m_window->IsKeyPressed(GLFW_KEY_D) || m_window->IsKeyPressed(GLFW_KEY_RIGHT))
    {
        ProcessKeyboard(Direction::RIGHT);
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::camera::Camera::ProcessKeyboard(const Direction t_direction)
{
    const auto right{ glm::vec2(0.0f, 32.0f) };
    const auto left{ glm::vec2(64.0f, 0.0f) };

    if (t_direction == Direction::UP)
    {
        position += right;
    }

    if (t_direction == Direction::DOWN)
    {
        position -= right;
    }

    if (t_direction == Direction::LEFT)
    {
        position -= left;
    }

    if (t_direction == Direction::RIGHT)
    {
        position += left;
    }
}
