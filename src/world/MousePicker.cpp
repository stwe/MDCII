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
#include "renderer/TileRenderer.h"
#include "renderer/RenderUtils.h"
#include "state/State.h"
#include "ogl/resource/ResourceManager.h"
#include "data/Text.h"
#include "vendor/eventpp/utilities/argumentadapter.h"
#include "vendor/stb/stb_image.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::MousePicker::MousePicker(world::World* t_world, const ogl::Window& t_window, const camera::Camera& t_camera)
    : m_world{ t_world }
{
    Log::MDCII_LOG_DEBUG("[MousePicker::MousePicker()] Create MousePicker.");

    MDCII_ASSERT(m_world, "[MousePicker::MousePicker()] Null pointer.")
    MDCII_ASSERT(m_world->width != -1, "[MousePicker::MousePicker()] Invalid world width.")
    MDCII_ASSERT(m_world->height != -1, "[MousePicker::MousePicker()] Invalid world height.")

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
    if (!inWindow || !m_world->IsPositionInWorld(currentPosition))
    {
        return;
    }

    auto screenPosition{ m_world->WorldToScreen(currentPosition.x, currentPosition.y, m_world->zoom, m_world->rotation) };
    if (calcForIslandTerrain)
    {
        screenPosition.y -= static_cast<float>(get_elevation(m_world->zoom));
    }

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
    begin_bottom_right("Mouse", 322.0f);

    ImGui::SetWindowSize({ 321.0f, 65.0f });

    auto terrainMode{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "TerrainMode") };
    auto mousePos{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "CurrentMousePosition") };
    auto oldMousePos{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "OldMousePosition") };

    terrainMode.append(": ");
    if (calcForIslandTerrain)
    {
        terrainMode.append(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Yes"));
    }
    else
    {
        terrainMode.append(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "No"));
    }

    mousePos.append(": (").append(std::to_string(currentPosition.x)).append(", ").append(std::to_string(currentPosition.y)).append(")");
    oldMousePos.append(": (").append(std::to_string(lastPosition.x)).append(", ").append(std::to_string(lastPosition.y)).append(")");

    ImGui::TextUnformatted(terrainMode.c_str());
    ImGui::TextUnformatted(mousePos.c_str());
    ImGui::TextUnformatted(oldMousePos.c_str());

    ImGui::End();
}

//-------------------------------------------------
// Event handler
//-------------------------------------------------

void mdcii::world::MousePicker::OnMouseMoved(const ogl::Window& t_window, const camera::Camera& t_camera)
{
    if (const auto newPosition{ GetWorldPosition(t_window, t_camera) }; newPosition != currentPosition)
    {
        lastPosition = currentPosition;
        currentPosition = newPosition;
        tilePositionHasChanged = true;
    }
    else
    {
        tilePositionHasChanged = false;
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

glm::ivec2 mdcii::world::MousePicker::GetWorldPosition(const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    const auto mouse{ glm::ivec2(t_window.GetMouseX(), t_window.GetMouseY()) };

    glm::ivec2 cell;
    glm::ivec2 offsetIntoCell;

    if (calcForIslandTerrain)
    {
        cell = glm::ivec2(mouse.x / get_tile_width(m_world->zoom), (mouse.y + get_elevation(m_world->zoom)) / get_tile_height(m_world->zoom));
        offsetIntoCell = glm::ivec2(mouse.x % get_tile_width(m_world->zoom), (mouse.y + get_elevation(m_world->zoom)) % get_tile_height(m_world->zoom));
    }
    else
    {
        cell = glm::ivec2(mouse.x / get_tile_width(m_world->zoom), mouse.y / get_tile_height(m_world->zoom));
        offsetIntoCell = glm::ivec2(mouse.x % get_tile_width(m_world->zoom), mouse.y % get_tile_height(m_world->zoom));
    }

    const auto* pixelCol{ m_cheatImages.at(magic_enum::enum_integer(m_world->zoom)) + (4_uz * (static_cast<size_t>(offsetIntoCell.y) * get_tile_width(m_world->zoom) + offsetIntoCell.x)) };
    const auto r = pixelCol[0];
    const auto g = pixelCol[1];
    const auto b = pixelCol[2];

    auto result{ glm::ivec2(-1) };

    if (m_world->rotation == Rotation::DEG0)
    {
        result = glm::ivec2(
            (cell.y + t_camera.worldPosition.y) + (cell.x + t_camera.worldPosition.x),
            (cell.y + t_camera.worldPosition.y) - (cell.x + t_camera.worldPosition.x)
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
            (cell.y + t_camera.worldPosition.y) - (cell.x + t_camera.worldPosition.x),
            m_world->width - 1 - ((cell.x + t_camera.worldPosition.x) + (cell.y + t_camera.worldPosition.y))
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
            m_world->width - 1 - ((cell.y + t_camera.worldPosition.y) + (cell.x + t_camera.worldPosition.x)),
            m_world->height - 1 - ((cell.y + t_camera.worldPosition.y) - (cell.x + t_camera.worldPosition.x))
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
            m_world->height - 1 - ((cell.y + t_camera.worldPosition.y) - (cell.x + t_camera.worldPosition.x)),
            (cell.y + t_camera.worldPosition.y) + (cell.x + t_camera.worldPosition.x)
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
