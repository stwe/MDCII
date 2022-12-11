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

#include <imgui.h>
#include "MousePicker.h"
#include "World.h"
#include "Rotation.h"
#include "MdciiAssert.h"
#include "Game.h"
#include "eventpp/utilities/argumentadapter.h"
#include "renderer/TileRenderer.h"
#include "renderer/RenderUtils.h"
#include "state/State.h"
#include "ogl/resource/ResourceManager.h"
#include "ogl/resource/stb_image.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::MousePicker::MousePicker(world::World* t_world, const ogl::Window& t_window, const camera::Camera& t_camera)
    : m_world{ t_world }
{
    Log::MDCII_LOG_DEBUG("[MousePicker::MousePicker()] Create MousePicker.");

    MDCII_ASSERT(m_world, "[MousePicker::MousePicker()] Null pointer.")

    m_worldWidth = m_world->worldWidth;
    m_worldHeight = m_world->worldHeight;

    Init();
    AddListeners(t_window, t_camera);
}

mdcii::world::MousePicker::~MousePicker() noexcept
{
    Log::MDCII_LOG_DEBUG("[MousePicker::~MousePicker()] Destruct MousePicker.");

    CleanUp();
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::MousePicker::Render(const ogl::Window& t_window, const camera::Camera& t_camera)
{
    if (!inWindow || !m_world->IsPositionInWorld(currentPosition.x, currentPosition.y))
    {
        return;
    }

    auto screenPosition{ m_world->WorldToScreen(currentPosition.x, currentPosition.y, m_world->zoom, m_world->rotation) };
    screenPosition.y -= static_cast<float>(get_elevation(m_world->zoom));

    m_renderer->RenderTile(
        renderer::RenderUtils::GetModelMatrix(
            screenPosition,
            glm::vec2(get_tile_width(m_world->zoom), get_tile_height(m_world->zoom))
        ),
        ogl::resource::ResourceManager::LoadTexture(m_cursorFileNames.at(magic_enum::enum_integer(m_world->zoom))).id,
        t_window, t_camera
    );
}

void mdcii::world::MousePicker::RenderImGui() const
{
    auto winW{ static_cast<float>(m_world->context->window->GetWidth()) };
    auto winH{ static_cast<float>(m_world->context->window->GetHeight()) };

    ImGui::SetNextWindowSize(ImVec2(290.0f, 48.0f), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Pos.x + (winW / 1.4f), ImGui::GetMainViewport()->Pos.y + winH - (winH / 8.0f) + 12.0f), ImGuiCond_Once);

    int windowFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;
    //ImGuiWindowFlags_NoBackground;

    ImGui::SetNextWindowBgAlpha(0.8f);

    ImGui::Begin("Mouse Picker", nullptr, windowFlags);

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 230, 230, 255));

    ImGui::Text("Current position x: %d, y: %d", currentPosition.x, currentPosition.y);
    ImGui::Text("Old position x: %d, y: %d", lastPosition.x, lastPosition.y);

    ImGui::PopStyleColor();

    ImGui::End();
}

//-------------------------------------------------
// Event handler
//-------------------------------------------------

void mdcii::world::MousePicker::OnMouseMoved(const ogl::Window& t_window, const camera::Camera& t_camera)
{
    const auto newPosition{ GetWorldPosition(t_window, t_camera) };
    if (newPosition != currentPosition)
    {
        lastPosition = currentPosition;
        currentPosition = newPosition;
        newTilePosition = true;
    }
    else
    {
        newTilePosition = false;
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

glm::ivec2 mdcii::world::MousePicker::GetWorldPosition(const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    const auto mouse{ glm::ivec2(t_window.GetMouseX(), t_window.GetMouseY()) };
    const auto cell{ glm::ivec2(mouse.x / get_tile_width(m_world->zoom), (mouse.y + get_elevation(m_world->zoom)) / get_tile_height(m_world->zoom)) };
    const auto offsetIntoCell{ glm::ivec2(mouse.x % get_tile_width(m_world->zoom), (mouse.y + get_elevation(m_world->zoom)) % get_tile_height(m_world->zoom)) };

    const glm::ivec2 origin{
        static_cast<int>(t_camera.position.x) / get_tile_width(m_world->zoom),
        static_cast<int>(t_camera.position.y) / get_tile_height(m_world->zoom)
    };

    const auto* pixelCol{ m_cheatImages.at(magic_enum::enum_integer(m_world->zoom)) + (4_uz * (static_cast<size_t>(offsetIntoCell.y) * get_tile_width(m_world->zoom) + offsetIntoCell.x)) };
    const auto r = pixelCol[0];
    const auto g = pixelCol[1];
    const auto b = pixelCol[2];

    auto result{ glm::ivec2(-1) };

    if (m_world->rotation == Rotation::DEG0)
    {
        result = glm::ivec2(
            (cell.y + origin.y) + (cell.x + origin.x),
            (cell.y + origin.y) - (cell.x + origin.x)
        );

        if (r == 255 && g == 0 && b == 0)
        {
            result.x -= 1;
        }
        else if (r == 0 && g == 255 && b == 0)
        {
            result.y -= 1;
        }
        else if (r == 0 && g == 0 && b == 255)
        {
            result.y += 1;
        }
        else if (r == 255 && g == 255 && b == 0)
        {
            result.x += 1;
        }
    }

    if (m_world->rotation == Rotation::DEG90)
    {
        result = glm::ivec2(
            (cell.y + origin.y) - (cell.x + origin.x),
            m_worldWidth - 1 - ((cell.x + origin.x) + (cell.y + origin.y))
        );

        if (r == 255 && g == 0 && b == 0)
        {
            result.y += 1;
        }
        else if (r == 0 && g == 255 && b == 0)
        {
            result.x -= 1;
        }
        else if (r == 0 && g == 0 && b == 255)
        {
            result.x += 1;
        }
        else if (r == 255 && g == 255 && b == 0)
        {
            result.y -= 1;
        }
    }

    if (m_world->rotation == Rotation::DEG180)
    {
        result = glm::ivec2(
            m_worldWidth - 1 - ((cell.y + origin.y) + (cell.x + origin.x)),
            m_worldHeight - 1 - ((cell.y + origin.y) - (cell.x + origin.x))
        );

        if (r == 255 && g == 0 && b == 0)
        {
            result.x += 1;
        }
        else if (r == 0 && g == 255 && b == 0)
        {
            result.y += 1;
        }
        else if (r == 0 && g == 0 && b == 255)
        {
            result.y -= 1;
        }
        else if (r == 255 && g == 255 && b == 0)
        {
            result.x -= 1;
        }
    }

    if (m_world->rotation == Rotation::DEG270)
    {
        result = glm::ivec2(
            m_worldHeight - 1 - ((cell.y + origin.y) - (cell.x + origin.x)),
            (cell.y + origin.y) + (cell.x + origin.x)
        );

        if (r == 255 && g == 0 && b == 0)
        {
            result.y -= 1;
        }
        else if (r == 0 && g == 255 && b == 0)
        {
            result.x += 1;
        }
        else if (r == 0 && g == 0 && b == 255)
        {
            result.x -= 1;
        }
        else if (r == 255 && g == 255 && b == 0)
        {
            result.y += 1;
        }
    }

    return result;
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::MousePicker::Init()
{
    Log::MDCII_LOG_DEBUG("[MousePicker::Init()] Initializing mouse picker...");

    m_renderer = std::make_unique<renderer::TileRenderer>();

    // initial setup
    if (glfwGetWindowAttrib(m_world->context->window->GetWindowHandle(), GLFW_HOVERED))
    {
        inWindow = true;
    }

    magic_enum::enum_for_each<Zoom>([this](const Zoom t_zoom) {
        // store cursor file names
        const auto zoomStr{ to_lower_case(std::string(magic_enum::enum_name<Zoom>(t_zoom))) };
        const auto cursorFileName{ "textures/" + zoomStr + "/mouse_" + zoomStr + ".png" };
        m_cursorFileNames.at(magic_enum::enum_integer(t_zoom)) = cursorFileName;

        // store cheat images
        const auto fileName{ "textures/" + zoomStr + "/corner_" + zoomStr + ".png" };
        const std::string path{ Game::RESOURCES_REL_PATH + fileName };

        Log::MDCII_LOG_DEBUG("[MousePicker::Init()] Load cheat image for zoom {}...", magic_enum::enum_name(t_zoom));

        int width;
        int height;
        int channels;
        auto* cornerImage{ stbi_load(path.c_str(), &width, &height, &channels, 0) };
        if (!cornerImage)
        {
            throw MDCII_EXCEPTION("[MousePicker::Init()] Mouse picker cheat image failed to load at path: " + path);
        }

        if (width != get_tile_width(t_zoom))
        {
            throw MDCII_EXCEPTION("[MousePicker::Init()] Invalid width.");
        }

        if (height != get_tile_height(t_zoom))
        {
            throw MDCII_EXCEPTION("[MousePicker::Init()] Invalid height.");
        }

        Log::MDCII_LOG_DEBUG("[MousePicker::Init()] The cheat image was loaded successfully.");

        m_cheatImages.at(magic_enum::enum_integer(t_zoom)) = cornerImage;
    });

    Log::MDCII_LOG_DEBUG("[MousePicker::Init()] The mouse picker was successfully initialized.");
}

void mdcii::world::MousePicker::AddListeners(const ogl::Window& t_window, const camera::Camera& t_camera)
{
    Log::MDCII_LOG_DEBUG("[MousePicker::AddListeners()] Add listeners.");

    // OnMouseEnter
    m_mouseEnter = event::EventManager::event_dispatcher.appendListener(
        event::MdciiEventType::MOUSE_ENTER,
        eventpp::argumentAdapter<void(const event::MouseEnterEvent&)>(
            [this](const event::MouseEnterEvent& t_event) {
                OnMouseEnter(t_event.enter);
            }
        )
    );

    // OnMouseMoved
    m_mouseMoved = event::EventManager::event_dispatcher.appendListener(
        event::MdciiEventType::MOUSE_MOVED,
        eventpp::argumentAdapter<void(const event::MouseMovedEvent&)>(
            [this, &t_window, &t_camera]([[maybe_unused]] const event::MouseMovedEvent& t_event) {
                OnMouseMoved(t_window, t_camera);
            }
        )
    );
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::world::MousePicker::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[MousePicker::CleanUp()] CleanUp MousePicker.");

    magic_enum::enum_for_each<Zoom>([this](const Zoom t_zoom) {
        stbi_image_free(m_cheatImages.at(magic_enum::enum_integer(t_zoom)));
    });

    event::EventManager::event_dispatcher.removeListener(event::MdciiEventType::MOUSE_ENTER, m_mouseEnter);
    event::EventManager::event_dispatcher.removeListener(event::MdciiEventType::MOUSE_MOVED, m_mouseMoved);
}
