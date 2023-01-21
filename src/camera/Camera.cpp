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

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include "Game.h"
#include "Camera.h"
#include "Log.h"
#include "eventpp/utilities/argumentadapter.h"
#include "physics/Aabb.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::camera::Camera::Camera(const int32_t t_windowWidth, const int32_t t_windowHeight)
    : m_windowWidth{ t_windowWidth }
    , m_windowHeight{ t_windowHeight }
{
    Log::MDCII_LOG_DEBUG("[Camera::Camera()] Create Camera.");

    // todo: config?
    zoom = world::Zoom::GFX;

    const auto pos{ Game::INI.GetVector<int32_t>("camera", "world_position") };
    if (pos.size() != 2)
    {
        throw MDCII_EXCEPTION("[Camera::Camera()] Invalid world position given.");
    }

    worldPosition = glm::ivec2(pos[0], pos[1]);

    position.x = static_cast<float>(worldPosition.x) * static_cast<float>(get_tile_width(zoom));
    position.y = static_cast<float>(worldPosition.y) * static_cast<float>(get_tile_height(zoom));

    Log::MDCII_LOG_DEBUG("[Camera::Camera()] Set the camera world position to ({}, {}).", worldPosition.x, worldPosition.y);
    Log::MDCII_LOG_DEBUG("[Camera::Camera()] Set the camera screen position to ({}, {}).", position.x, position.y);

    aabb = std::make_unique<physics::Aabb>(position, glm::ivec2(m_windowWidth, m_windowHeight));

    AddListeners();
}

mdcii::camera::Camera::~Camera() noexcept
{
    Log::MDCII_LOG_DEBUG("[Camera::~Camera()] Destruct Camera.");

    CleanUp();
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
// Culling
//-------------------------------------------------

bool mdcii::camera::Camera::IsIslandNotInCamera(const world::Zoom t_zoom, const world::Rotation t_rotation, const world::Island& t_island) const
{
    const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
    const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

    // todo: pre-calc
    const auto islandAabb{ physics::Aabb(
        glm::ivec2(t_island.min.at(zoomInt).at(rotationInt).x, t_island.min.at(zoomInt).at(rotationInt).y),
        glm::ivec2((t_island.max.at(zoomInt).at(rotationInt).x + static_cast<float>(get_tile_width(zoom))) - t_island.min.at(zoomInt).at(rotationInt).x,
                   (t_island.max.at(zoomInt).at(rotationInt).y + static_cast<float>(get_tile_height(zoom))) - t_island.min.at(zoomInt).at(rotationInt).y)
    ) };

    return !physics::Aabb::AabbVsAabb(islandAabb, *aabb);
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::camera::Camera::RenderImGui()
{
    ImGui::Text("Camera world (%d, %d)", worldPosition.x, worldPosition.y);
    ImGui::Text("Camera screen (%.2f, %.2f)", position.x, position.y);
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::camera::Camera::ProcessKeyboard(const Direction t_direction)
{
    const auto yTileOff{ glm::ivec2(0, 1) };
    const auto xTileOff{ glm::ivec2(1, 0) };

    const auto yOff{ glm::vec2(0.0f, get_tile_height(zoom)) };
    const auto xOff{ glm::vec2(get_tile_width(zoom), 0.0f) };

    if (t_direction == Direction::UP)
    {
        worldPosition -= yTileOff;
        position -= yOff;
    }

    if (t_direction == Direction::DOWN)
    {
        worldPosition += yTileOff;
        position += yOff;
    }

    if (t_direction == Direction::LEFT)
    {
        worldPosition -= xTileOff;
        position -= xOff;
    }

    if (t_direction == Direction::RIGHT)
    {
        worldPosition += xTileOff;
        position += xOff;
    }

    aabb->position = position;
}

//-------------------------------------------------
// Listeners
//-------------------------------------------------

void mdcii::camera::Camera::AddListeners()
{
    Log::MDCII_LOG_DEBUG("[Camera::AddListeners()] Add listeners.");

    m_keyPressed = event::EventManager::event_dispatcher.appendListener(
        event::MdciiEventType::KEY_PRESSED,
        eventpp::argumentAdapter<void(const event::KeyPressedEvent&)>(
            [this](const event::KeyPressedEvent& t_event) {
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

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::camera::Camera::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[Camera::CleanUp()] CleanUp Camera.");

    event::EventManager::event_dispatcher.removeListener(event::MdciiEventType::KEY_PRESSED, m_keyPressed);
}
