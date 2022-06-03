#include <glm/gtc/matrix_transform.hpp>
#include "Game.h"
#include "Camera.h"
#include "Log.h"
#include "map/Map.h"
#include "ogl/Window.h"
#include "ogl/OpenGL.h"
#include "event/EventManager.h"
#include "eventpp/utilities/argumentadapter.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::camera::Camera::Camera(std::shared_ptr<ogl::Window> t_window, const glm::vec2& t_position)
    : m_window{ std::move(t_window) }
{
    Log::MDCII_LOG_DEBUG("[Camera::Camera()] Create Camera.");

    position = t_position;

    AddListeners();
}

mdcii::camera::Camera::Camera(std::shared_ptr<ogl::Window> t_window)
    : m_window{ std::move(t_window) }
{
    Log::MDCII_LOG_DEBUG("[Camera::Camera()] Create Camera.");

    const auto pos{ Game::INI.GetVector<float>("camera", "position") };
    position = glm::vec2(pos[0], pos[1]);

    AddListeners();
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
    /*
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
    */
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::camera::Camera::ProcessKeyboard(const Direction t_direction)
{
    const auto yOff{ glm::vec2(0.0f, map::Map::TILE_HEIGHT) };
    const auto xOff{ glm::vec2(map::Map::TILE_WIDTH, 0.0f) };

    if (t_direction == Direction::UP)
    {
        position += yOff;
    }

    if (t_direction == Direction::DOWN)
    {
        position -= yOff;
    }

    if (t_direction == Direction::LEFT)
    {
        position -= xOff;
    }

    if (t_direction == Direction::RIGHT)
    {
        position += xOff;
    }
}

//-------------------------------------------------
// Listeners
//-------------------------------------------------

void mdcii::camera::Camera::AddListeners()
{
    Log::MDCII_LOG_DEBUG("[Camera::AddListeners()] Append listeners.");

    event::EventManager::eventDispatcher.appendListener(
        event::MdciiEventType::MOUSE_ENTER,
        eventpp::argumentAdapter<void(const event::MouseEnterEvent&)>(
            [&](const event::MouseEnterEvent& t_event)
            {
                m_inWindow = t_event.enter;
            }
        )
    );

    event::EventManager::eventDispatcher.appendListener(
        event::MdciiEventType::KEY_PRESSED,
        eventpp::argumentAdapter<void(const event::KeyPressedEvent&)>(
            [&](const event::KeyPressedEvent& t_event)
            {
                if (t_event.key == GLFW_KEY_W || t_event.key == GLFW_KEY_UP)
                {
                    ProcessKeyboard(Direction::UP);
                }

                if (t_event.key == GLFW_KEY_S || t_event.key == GLFW_KEY_DOWN)
                {
                    ProcessKeyboard(Direction::DOWN);
                }

                if (t_event.key == GLFW_KEY_A || t_event.key == GLFW_KEY_LEFT)
                {
                    ProcessKeyboard(Direction::LEFT);
                }

                if (t_event.key == GLFW_KEY_D || t_event.key == GLFW_KEY_RIGHT)
                {
                    ProcessKeyboard(Direction::RIGHT);
                }
            }
        )
    );
}
