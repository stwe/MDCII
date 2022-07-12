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

mdcii::map::MousePicker::MousePicker(std::shared_ptr<Map> t_map)
    : m_map{ std::move(t_map) }
{
    Log::MDCII_LOG_DEBUG("[MousePicker::MousePicker()] Create MousePicker.");

    Init();
    AddListeners();
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

    m_mouse = glm::ivec2(t_window.GetMouseX(), t_window.GetMouseY());
    m_cell = glm::ivec2(m_mouse.x / MapTile::TILE_WIDTH, (m_mouse.y + Map::ELEVATION) / MapTile::TILE_HEIGHT);
    m_offsetIntoCell = glm::ivec2(m_mouse.x % MapTile::TILE_WIDTH, (m_mouse.y + Map::ELEVATION) % MapTile::TILE_HEIGHT);

    const glm::ivec2 origin{
        static_cast<int>(t_camera.position.x) / MapTile::TILE_WIDTH,
        static_cast<int>(t_camera.position.y) / MapTile::TILE_HEIGHT
    };

    const auto* pixelCol{ m_cornerImage + (4ULL * (static_cast<size_t>(m_offsetIntoCell.y) * MapTile::TILE_WIDTH + m_offsetIntoCell.x)) };
    const auto r = pixelCol[0];
    const auto g = pixelCol[1];
    const auto b = pixelCol[2];

    if (m_map->mapContent->rotation == Rotation::DEG0)
    {
        selected.currentPosition = glm::ivec2(
            (m_cell.y + origin.y) + (m_cell.x + origin.x),
            (m_cell.y + origin.y) - (m_cell.x + origin.x)
        );

        if (r == 255 && g == 0 && b == 0)
        {
            selected.currentPosition.x -= 1;
        }
        else if (r == 0 && g == 255 && b == 0)
        {
            selected.currentPosition.y -= 1;
        }
        else  if (r == 0 && g == 0 && b == 255)
        {
            selected.currentPosition.y += 1;
        }
        else if (r == 255 && g == 255 && b == 0)
        {
            selected.currentPosition.x += 1;
        }
    }

    if (m_map->mapContent->rotation == Rotation::DEG90)
    {
        selected.currentPosition = glm::ivec2(
            (m_cell.y + origin.y) - (m_cell.x + origin.x),
            m_map->mapContent->width - 1 - ((m_cell.x + origin.x) + (m_cell.y + origin.y))
        );

        if (r == 255 && g == 0 && b == 0)
        {
            selected.currentPosition.y += 1;
        }
        else if (r == 0 && g == 255 && b == 0)
        {
            selected.currentPosition.x -= 1;
        }
        else  if (r == 0 && g == 0 && b == 255)
        {
            selected.currentPosition.x += 1;
        }
        else if (r == 255 && g == 255 && b == 0)
        {
            selected.currentPosition.y -= 1;
        }
    }

    if (m_map->mapContent->rotation == Rotation::DEG180)
    {
        selected.currentPosition = glm::ivec2(
            m_map->mapContent->width - 1 - ((m_cell.y + origin.y) + (m_cell.x + origin.x)),
            m_map->mapContent->height - 1 - ((m_cell.y + origin.y) - (m_cell.x + origin.x))
        );

        if (r == 255 && g == 0 && b == 0)
        {
            selected.currentPosition.x += 1;
        }
        else if (r == 0 && g == 255 && b == 0)
        {
            selected.currentPosition.y += 1;
        }
        else  if (r == 0 && g == 0 && b == 255)
        {
            selected.currentPosition.y -= 1;
        }
        else if (r == 255 && g == 255 && b == 0)
        {
            selected.currentPosition.x -= 1;
        }
    }

    if (m_map->mapContent->rotation == Rotation::DEG270)
    {
        selected.currentPosition = glm::ivec2(
            m_map->mapContent->height - 1 - ((m_cell.y + origin.y) - (m_cell.x + origin.x)),
            (m_cell.y + origin.y) + (m_cell.x + origin.x)
        );

        if (r == 255 && g == 0 && b == 0)
        {
            selected.currentPosition.y -= 1;
        }
        else if (r == 0 && g == 255 && b == 0)
        {
            selected.currentPosition.x += 1;
        }
        else  if (r == 0 && g == 0 && b == 255)
        {
            selected.currentPosition.x -= 1;
        }
        else if (r == 255 && g == 255 && b == 0)
        {
            selected.currentPosition.y += 1;
        }
    }

    RenderMouseCursor(t_window, t_camera);
}

void mdcii::map::MousePicker::RenderImGui() const
{
    ImGui::Begin("MousePicker");

    ImGui::Text("Mouse x: %d, y: %d, in window: %s", m_mouse.x, m_mouse.y, m_inWindow ? "yes" : "no");
    ImGui::Text("Cell x: %d, y: %d", m_cell.x, m_cell.y);
    ImGui::Text("Offset into cell x: %d, y: %d", m_offsetIntoCell.x, m_offsetIntoCell.y);
    ImGui::Text("Current tile position x: %d, y: %d", selected.currentPosition.x, selected.currentPosition.y);
    ImGui::Text("Last tile position x: %d, y: %d", selected.lastPosition.x, selected.lastPosition.y);

    ImGui::End();
}

//-------------------------------------------------
// Cursor
//-------------------------------------------------

void mdcii::map::MousePicker::RenderMouseCursor(const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    if (!m_map->mapContent->IsPositionInMap(selected.currentPosition))
    {
        return;
    }

    auto screenPosition{ m_map->mapContent->MapToScreen(selected.currentPosition.x, selected.currentPosition.y, m_map->mapContent->rotation) };
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

void mdcii::map::MousePicker::AddListeners()
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
                    selected.lastPosition = selected.currentPosition;
                    selected.lastChanged = true;
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
                    selected.lastChanged = false;
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
