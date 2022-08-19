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

#include <imgui.h>
#include "MousePicker.h"
#include "Game.h"
#include "Map.h"
#include "MapContent.h"
#include "MdciiAssert.h"
#include "MdciiException.h"
#include "ecs/Components.h"
#include "event/EventManager.h"
#include "eventpp/utilities/argumentadapter.h"
#include "renderer/TileRenderer.h"
#include "renderer/RenderUtils.h"
#include "ogl/resource/ResourceManager.h"
#include "ogl/resource/stb_image.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::map::MousePicker::MousePicker(Map* t_map, const ogl::Window& t_window, const camera::Camera& t_camera)
    : m_map{ t_map }
{
    Log::MDCII_LOG_DEBUG("[MousePicker::MousePicker()] Create MousePicker.");

    MDCII_ASSERT(m_map, "[MousePicker::MousePicker()] Null pointer.")

    Init();
    AddListeners(t_window, t_camera);
}

mdcii::map::MousePicker::~MousePicker() noexcept
{
    Log::MDCII_LOG_DEBUG("[MousePicker::~MousePicker()] Destruct MousePicker.");

    CleanUp();
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::map::MousePicker::Render(const ogl::Window& t_window, const camera::Camera& t_camera)
{
    if (!m_inWindow)
    {
        return;
    }

    RenderMouseCursor(t_window, t_camera);
}

void mdcii::map::MousePicker::RenderImGui() const
{
    ImGui::Begin("MousePicker");

    ImGui::Text("Current position x: %d, y: %d", m_currentPosition.x, m_currentPosition.y);
    ImGui::Text("Old position x: %d, y: %d", m_lastPosition.x, m_lastPosition.y);

    ImGui::End();
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::map::MousePicker::UpdatePositions(const ogl::Window& t_window, const camera::Camera& t_camera)
{
    const auto newPosition{ GetMapPosition(t_window, t_camera) };
    if (newPosition != m_currentPosition)
    {
        m_lastPosition = m_currentPosition;
        m_currentPosition = newPosition;
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

glm::ivec2 mdcii::map::MousePicker::GetMapPosition(const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    const auto mouse{ glm::ivec2(t_window.GetMouseX(), t_window.GetMouseY()) };
    const auto cell{ glm::ivec2(mouse.x / get_tile_width(m_map->mapContent->zoom), (mouse.y + get_elevation(m_map->mapContent->zoom)) / get_tile_height(m_map->mapContent->zoom)) };
    const auto offsetIntoCell{ glm::ivec2(mouse.x % get_tile_width(m_map->mapContent->zoom), (mouse.y + get_elevation(m_map->mapContent->zoom)) % get_tile_height(m_map->mapContent->zoom)) };

    const glm::ivec2 origin{
        static_cast<int>(t_camera.position.x) / get_tile_width(m_map->mapContent->zoom),
        static_cast<int>(t_camera.position.y) / get_tile_height(m_map->mapContent->zoom)
    };

    const auto* pixelCol{ m_cornerImages.at(magic_enum::enum_integer(m_map->mapContent->zoom)) + (4_uz * (static_cast<size_t>(offsetIntoCell.y) * get_tile_width(m_map->mapContent->zoom) + offsetIntoCell.x)) };
    const auto r = pixelCol[0];
    const auto g = pixelCol[1];
    const auto b = pixelCol[2];

    auto result{ glm::ivec2(-1) };

    if (m_map->mapContent->rotation == Rotation::DEG0)
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

    if (m_map->mapContent->rotation == Rotation::DEG90)
    {
        result = glm::ivec2(
            (cell.y + origin.y) - (cell.x + origin.x),
            m_map->mapContent->width - 1 - ((cell.x + origin.x) + (cell.y + origin.y))
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

    if (m_map->mapContent->rotation == Rotation::DEG180)
    {
        result = glm::ivec2(
            m_map->mapContent->width - 1 - ((cell.y + origin.y) + (cell.x + origin.x)),
            m_map->mapContent->height - 1 - ((cell.y + origin.y) - (cell.x + origin.x))
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

    if (m_map->mapContent->rotation == Rotation::DEG270)
    {
        result = glm::ivec2(
            m_map->mapContent->height - 1 - ((cell.y + origin.y) - (cell.x + origin.x)),
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

bool mdcii::map::MousePicker::IsCurrentMouseInMap() const
{
    return m_map->mapContent->IsPositionInMap(m_currentPosition.x, m_currentPosition.y);
}

bool mdcii::map::MousePicker::IsLastMouseInMap() const
{
    return m_map->mapContent->IsPositionInMap(m_lastPosition.x, m_lastPosition.y);
}

//-------------------------------------------------
// Cursor
//-------------------------------------------------

void mdcii::map::MousePicker::RenderMouseCursor(const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    if (!m_map->mapContent->IsPositionInMap(m_currentPosition.x, m_currentPosition.y))
    {
        return;
    }

    auto screenPosition{ m_map->mapContent->MapToScreen(m_currentPosition.x, m_currentPosition.y, m_map->mapContent->zoom, m_map->mapContent->rotation) };
    screenPosition.y -= static_cast<float>(get_elevation(m_map->mapContent->zoom));

    m_renderer->RenderTile(
        renderer::RenderUtils::GetModelMatrix(
            screenPosition,
            glm::vec2(get_tile_width(m_map->mapContent->zoom), get_tile_height(m_map->mapContent->zoom))
        ),
        ogl::resource::ResourceManager::LoadTexture(m_cursorFileNames.at(magic_enum::enum_integer(m_map->mapContent->zoom))).id,
        t_window, t_camera
    );
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::map::MousePicker::Init()
{
    Log::MDCII_LOG_DEBUG("[MousePicker::Init()] Initializing mouse picker.");

    m_renderer = std::make_unique<renderer::TileRenderer>();

    magic_enum::enum_for_each<Zoom>([&](const Zoom t_zoom) {
        // store cursor file names
        const auto zoomStr{ to_lower_case(std::string(magic_enum::enum_name<Zoom>(t_zoom))) };
        const auto cursorFileName{ "textures/" + zoomStr + "/frame_" + zoomStr + ".png" };
        m_cursorFileNames.at(magic_enum::enum_integer(t_zoom)) = cursorFileName;

        // store cheat images
        const auto fileName{ "textures/" + zoomStr + "/corner_" + zoomStr + ".png" };
        const std::string path{ Game::RESOURCES_REL_PATH + fileName };

        Log::MDCII_LOG_DEBUG("[MousePicker::Init()] Load cheat image for zoom {}...", magic_enum::enum_name(t_zoom));

        int width, height, channels;
        auto* cornerImage{ stbi_load(path.c_str(), &width, &height, &channels, 0) };
        if (!cornerImage)
        {
            throw MDCII_EXCEPTION("[MousePicker::Init()] Mouse picker corner image failed to load at path: " + path);
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

        m_cornerImages.at(magic_enum::enum_integer(t_zoom)) = cornerImage;
    });

    Log::MDCII_LOG_DEBUG("[MousePicker::Init()] The mouse picker was successfully initialized.");
}

void mdcii::map::MousePicker::AddListeners(const ogl::Window& t_window, const camera::Camera& t_camera)
{
    Log::MDCII_LOG_DEBUG("[MousePicker::AddListeners()] Add listeners.");

    // mouse enter
    event::EventManager::eventDispatcher.appendListener(
        event::MdciiEventType::MOUSE_ENTER,
        eventpp::argumentAdapter<void(const event::MouseEnterEvent&)>(
            [&](const event::MouseEnterEvent& t_event) {
                m_inWindow = t_event.enter;
            }
        )
    );

    // mouse button pressed
    event::EventManager::eventDispatcher.appendListener(
        event::MdciiEventType::MOUSE_BUTTON_PRESSED,
        eventpp::argumentAdapter<void(const event::MouseButtonPressedEvent&)>(
            [&](const event::MouseButtonPressedEvent& t_event) {
                if (t_event.button == LEFT_MOUSE_BUTTON)
                {
                    if (m_map->currentAction == Map::Action::STATUS)
                    {
                        if (IsCurrentMouseInMap())
                        {
                            m_map->currentSelectedMapTile = m_map->mapContent->GetLayer(LayerType::BUILDINGS).GetTile(m_currentPosition.x, m_currentPosition.y);

                            // fallback
                            if (!m_map->currentSelectedMapTile.HasBuilding())
                            {
                                m_map->currentSelectedMapTile = m_map->mapContent->GetLayer(LayerType::TERRAIN).GetTile(m_currentPosition.x, m_currentPosition.y);
                            }
                        }
                    }

                    if (m_map->currentAction == Map::Action::BUILD && m_map->demolitionMode)
                    {
                        if (IsCurrentMouseInMap())
                        {
                            auto& buildingMapTile{ m_map->mapContent->GetLayer(LayerType::BUILDINGS).GetTile(m_currentPosition.x, m_currentPosition.y) };
                            if (buildingMapTile.HasBuilding())
                            {
                                const MapTile mapTile;

                                if (buildingMapTile.connectedMapTiles.empty())
                                {
                                    // remove BuildingsLayerTileComponent from entity
                                    Game::ecs.remove<ecs::BuildingsLayerTileComponent>(buildingMapTile.entity);

                                    // remove map tile from buildings layer / override
                                    buildingMapTile = mapTile;
                                }
                                else
                                {
                                    for (const auto tileIndex : buildingMapTile.connectedMapTiles)
                                    {
                                        // get tile
                                        auto& tile{ m_map->mapContent->GetLayer(LayerType::BUILDINGS).mapTiles.at(tileIndex) };

                                        // remove BuildingsLayerTileComponent from entity
                                        Game::ecs.remove<ecs::BuildingsLayerTileComponent>(tile.entity);

                                        // remove map tile from buildings layer / override
                                        tile = mapTile;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        )
    );

    // mouse button released
    event::EventManager::eventDispatcher.appendListener(
        event::MdciiEventType::MOUSE_BUTTON_RELEASED,
        eventpp::argumentAdapter<void(const event::MouseButtonReleasedEvent&)>(
            [&](const event::MouseButtonReleasedEvent& t_event) {
            }
        )
    );

    // mouse moved
    event::EventManager::eventDispatcher.appendListener(
        event::MdciiEventType::MOUSE_MOVED,
        eventpp::argumentAdapter<void(const event::MouseMovedEvent&)>(
            [&](const event::MouseMovedEvent& t_event) {
                UpdatePositions(t_window, t_camera);

                if (m_map->currentAction == Map::Action::BUILD)
                {
                    if (IsCurrentMouseInMap() && IsLastMouseInMap())
                    {
                        if (m_map->currentSelectedBauGfx.HasBuilding())
                        {
                            m_map->mapContent->RemoveBuildingsLayerComponent(m_lastPosition.x, m_lastPosition.y, m_map->currentSelectedBauGfx);
                            m_map->mapContent->AddBuildingsLayerComponent(m_currentPosition.x, m_currentPosition.y, m_map->currentSelectedBauGfx);
                        }
                    }
                }
            }
        )
    );
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::map::MousePicker::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[MousePicker::CleanUp()] CleanUp MousePicker.");

    magic_enum::enum_for_each<Zoom>([&](const Zoom t_zoom) {
        Log::MDCII_LOG_DEBUG("[MousePicker::CleanUp()] CleanUp cheat image for zoom {}.", magic_enum::enum_name(t_zoom));

        stbi_image_free(m_cornerImages.at(magic_enum::enum_integer(t_zoom)));
    });
}
