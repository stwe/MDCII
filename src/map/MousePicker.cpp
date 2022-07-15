#include <imgui.h>
#include "MousePicker.h"
#include "Game.h"
#include "Map.h"
#include "MapContent.h"
#include "MdciiAssert.h"
#include "MdciiException.h"
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

    ImGui::Text("Current position x: %d, y: %d", currentPosition.x, currentPosition.y);
    ImGui::Text("Old position x: %d, y: %d", lastPosition.x, lastPosition.y);

    ImGui::End();
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

glm::ivec2 mdcii::map::MousePicker::GetMapPosition(const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    const auto mouse{ glm::ivec2(t_window.GetMouseX(), t_window.GetMouseY()) };
    const auto cell{ glm::ivec2(mouse.x / MapTile::TILE_WIDTH, (mouse.y + Map::ELEVATION) / MapTile::TILE_HEIGHT) };
    const auto offsetIntoCell{ glm::ivec2(mouse.x % MapTile::TILE_WIDTH, (mouse.y + Map::ELEVATION) % MapTile::TILE_HEIGHT) };

    const glm::ivec2 origin{
        static_cast<int>(t_camera.position.x) / MapTile::TILE_WIDTH,
        static_cast<int>(t_camera.position.y) / MapTile::TILE_HEIGHT
    };

    const auto* pixelCol{ m_cornerImage + (4ULL * (static_cast<size_t>(offsetIntoCell.y) * MapTile::TILE_WIDTH + offsetIntoCell.x)) };
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
        else  if (r == 0 && g == 0 && b == 255)
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
        else  if (r == 0 && g == 0 && b == 255)
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
        else  if (r == 0 && g == 0 && b == 255)
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
        else  if (r == 0 && g == 0 && b == 255)
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
// Cursor
//-------------------------------------------------

void mdcii::map::MousePicker::RenderMouseCursor(const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    if (!m_map->mapContent->IsPositionInMap(currentPosition.x, currentPosition.y))
    {
        return;
    }

    auto screenPosition{ m_map->mapContent->MapToScreen(currentPosition.x, currentPosition.y, m_map->mapContent->rotation) };
    screenPosition.y -= Map::ELEVATION;

    m_renderer->RenderTile(
        renderer::RenderUtils::GetModelMatrix(
            screenPosition,
            glm::vec2(MapTile::TILE_WIDTH, MapTile::TILE_HEIGHT)
        ),
        ogl::resource::ResourceManager::LoadTexture("textures/frame.png").id,
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

    const std::string path{Game::RESOURCES_PATH + "textures/corner.png" };
    int width, height, channels;
    m_cornerImage = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!m_cornerImage)
    {
        throw MDCII_EXCEPTION("[MousePicker::Init()] Mouse picker corner image failed to load at path: " + path);
    }

    MDCII_ASSERT(width == MapTile::TILE_WIDTH, "[MousePicker::Init()] Invalid width.")
    MDCII_ASSERT(height == MapTile::TILE_HEIGHT, "[MousePicker::Init()] Invalid height.")

    Log::MDCII_LOG_DEBUG("[MousePicker::Init()] The mouse picker was successfully initialized.");
}

void mdcii::map::MousePicker::AddListeners(const ogl::Window& t_window, const camera::Camera& t_camera)
{
    Log::MDCII_LOG_DEBUG("[MousePicker::AddListeners()] Add listeners.");

    // mouse enter
    event::EventManager::eventDispatcher.appendListener(
        event::MdciiEventType::MOUSE_ENTER,
        eventpp::argumentAdapter<void(const event::MouseEnterEvent&)>
        (
            [&](const event::MouseEnterEvent& t_event)
            {
                m_inWindow = t_event.enter;
            }
        )
    );

    // left mouse button pressed
    event::EventManager::eventDispatcher.appendListener(
        event::MdciiEventType::MOUSE_BUTTON_PRESSED,
        eventpp::argumentAdapter<void(const event::MouseButtonPressedEvent&)>
        (
            [&](const event::MouseButtonPressedEvent& t_event)
            {
                if (t_event.button == 0)
                {
                }
            }
        )
    );

    // left mouse button released
    event::EventManager::eventDispatcher.appendListener(
        event::MdciiEventType::MOUSE_BUTTON_RELEASED,
        eventpp::argumentAdapter<void(const event::MouseButtonReleasedEvent&)>
        (
            [&](const event::MouseButtonReleasedEvent& t_event)
            {
                if (t_event.button == 0)
                {
                }
            }
        )
    );

    // mouse moved
    event::EventManager::eventDispatcher.appendListener(
        event::MdciiEventType::MOUSE_MOVED,
        eventpp::argumentAdapter<void(const event::MouseMovedEvent&)>
        (
            [&](const event::MouseMovedEvent& t_event)
            {
                const auto newPosition{ GetMapPosition(t_window, t_camera) };
                if (newPosition != currentPosition)
                {
                    lastPosition = currentPosition;
                    currentPosition = newPosition;
                }

                if (m_map->mapContent->IsPositionInMap(currentPosition.x, currentPosition.y) &&
                    m_map->mapContent->IsPositionInMap(lastPosition.x, lastPosition.y))
                {
                    if (m_map->selectedBauGfx.HasBuilding())
                    {
                        m_map->mapContent->RemoveBuilding(
                            lastPosition.x,
                            lastPosition.y,
                            m_map->selectedBauGfx.buildingId
                        );

                        // todo pass event::SelectedBauGfx type
                        m_map->mapContent->AddBuilding(
                            currentPosition.x,
                            currentPosition.y,
                            m_map->selectedBauGfx.buildingId, m_map->selectedBauGfx.orientation
                        );
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

    stbi_image_free(m_cornerImage);
}
