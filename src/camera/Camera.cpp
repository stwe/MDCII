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

#include <glm/gtc/matrix_transform.hpp>
#include "Game.h"
#include "Camera.h"
#include "Log.h"
#include "event/EventManager.h"
#include "eventpp/utilities/argumentadapter.h"
#include "map/MapTile.h"
#include "map/Zoom.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::camera::Camera::Camera()
{
    Log::MDCII_LOG_DEBUG("[Camera::Camera()] Create Camera.");

    const auto pos{ Game::INI.GetVector<float>("camera", "position") };
    position = glm::vec2(pos[0], pos[1]);

    Log::MDCII_LOG_DEBUG("[Camera::Camera()] Set the camera position to (x: {}, y: {}).", position.x, position.y);

    AddListeners();

    zoom = map::Zoom::GFX;
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
// Helper
//-------------------------------------------------

void mdcii::camera::Camera::ProcessKeyboard(const Direction t_direction)
{
    const auto yOff{ glm::vec2(0.0f, get_tile_height(zoom)) };
    const auto xOff{ glm::vec2(get_tile_width(zoom), 0.0f) };

    if (t_direction == Direction::UP)
    {
        position -= yOff;
    }

    if (t_direction == Direction::DOWN)
    {
        position += yOff;
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
    Log::MDCII_LOG_DEBUG("[Camera::AddListeners()] Add listeners.");

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
