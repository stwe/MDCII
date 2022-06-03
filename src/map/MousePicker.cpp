#include <imgui.h>
#include "MousePicker.h"
#include "Game.h"
#include "Map.h"
#include "MdciiAssert.h"
#include "MdciiException.h"
#include "renderer/TileRenderer.h"
#include "renderer/Utils.h"
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
    m_mouse = glm::ivec2(t_window.GetMouseX(), t_window.GetMouseY());
    m_cell = glm::ivec2(m_mouse.x / Map::TILE_WIDTH, (m_mouse.y + Map::ELEVATION) / Map::TILE_HEIGHT);
    m_offsetIntoCell = glm::ivec2(m_mouse.x % Map::TILE_WIDTH, (m_mouse.y + Map::ELEVATION) % Map::TILE_HEIGHT);

    const glm::ivec2 origin{
        static_cast<int>(t_camera.position.x) / Map::TILE_WIDTH,
    static_cast<int>(t_camera.position.y) / Map::TILE_HEIGHT
    };

    const auto* pixelCol{ m_cornerImage + (4ULL * (static_cast<size_t>(m_offsetIntoCell.y) * Map::TILE_WIDTH + m_offsetIntoCell.x)) };
    const auto r = pixelCol[0];
    const auto g = pixelCol[1];
    const auto b = pixelCol[2];

    if (m_map->rotation == Rotation::DEG0)
    {
        m_selected = glm::ivec2(
            (m_cell.y + origin.y) + (m_cell.x + origin.x),
            (m_cell.y + origin.y) - (m_cell.x + origin.x)
        );

        if (r == 255 && g == 0 && b == 0)
        {
            m_selected.x -= 1;
        }
        else if (r == 0 && g == 255 && b == 0)
        {
            m_selected.y -= 1;
        }
        else  if (r == 0 && g == 0 && b == 255)
        {
            m_selected.y += 1;
        }
        else if (r == 255 && g == 255 && b == 0)
        {
            m_selected.x += 1;
        }
    }

    if (m_map->rotation == Rotation::DEG90)
    {
        m_selected = glm::ivec2(
            (m_cell.y + origin.y) - (m_cell.x + origin.x),
            m_map->width - 1 - ((m_cell.x + origin.x) + (m_cell.y + origin.y))
        );

        if (r == 255 && g == 0 && b == 0)
        {
            m_selected.y += 1;
        }
        else if (r == 0 && g == 255 && b == 0)
        {
            m_selected.x -= 1;
        }
        else  if (r == 0 && g == 0 && b == 255)
        {
            m_selected.x += 1;
        }
        else if (r == 255 && g == 255 && b == 0)
        {
            m_selected.y -= 1;
        }
    }

    if (m_map->rotation == Rotation::DEG180)
    {
        m_selected = glm::ivec2(
            m_map->width - 1 - ((m_cell.y + origin.y) + (m_cell.x + origin.x)),
            m_map->height - 1 - ((m_cell.y + origin.y) - (m_cell.x + origin.x))
        );

        if (r == 255 && g == 0 && b == 0)
        {
            m_selected.x += 1;
        }
        else if (r == 0 && g == 255 && b == 0)
        {
            m_selected.y += 1;
        }
        else  if (r == 0 && g == 0 && b == 255)
        {
            m_selected.y -= 1;
        }
        else if (r == 255 && g == 255 && b == 0)
        {
            m_selected.x -= 1;
        }
    }

    if (m_map->rotation == Rotation::DEG270)
    {
        m_selected = glm::ivec2(
            m_map->height - 1 - ((m_cell.y + origin.y) - (m_cell.x + origin.x)),
            (m_cell.y + origin.y) + (m_cell.x + origin.x)
        );

        if (r == 255 && g == 0 && b == 0)
        {
            m_selected.y -= 1;
        }
        else if (r == 0 && g == 255 && b == 0)
        {
            m_selected.x += 1;
        }
        else  if (r == 0 && g == 0 && b == 255)
        {
            m_selected.x -= 1;
        }
        else if (r == 255 && g == 255 && b == 0)
        {
            m_selected.y += 1;
        }
    }

    auto selected{ m_map->MapToIso(m_selected.x, m_selected.y, m_map->rotation) }; // todo: rotation remove arg.
    selected.y -= Map::ELEVATION;

    m_renderer->RenderTile(
        renderer::Utils::GetModelMatrix(
            selected,
            glm::vec2(Map::TILE_WIDTH, Map::TILE_HEIGHT)
        ),
        ogl::resource::ResourceManager::LoadTexture("resources/textures/frame.png").id,
        t_window, t_camera
    );

    /*
    m_renderer->RenderTile(
        renderer::Utils::GetModelMatrix(
            glm::vec2(
                (m_cell.x + origin.x) * Map::TILE_WIDTH,
                (m_cell.y + origin.y) * Map::TILE_HEIGHT - Map::ELEVATION
            ),
            glm::vec2(Map::TILE_WIDTH, Map::TILE_HEIGHT)
        ),
        ogl::resource::ResourceManager::LoadTexture("resources/textures/frame.png").id,
        t_window, t_camera
    );
    */
}

void mdcii::map::MousePicker::RenderImGui() const
{
    ImGui::Begin("MousePicker", nullptr, 0);

    ImGui::Text("Mouse x: %d, y: %d", m_mouse.x, m_mouse.y);
    ImGui::Text("Cell x: %d, y: %d", m_cell.x, m_cell.y);
    ImGui::Text("Offset into cell x: %d, y: %d", m_offsetIntoCell.x, m_offsetIntoCell.y);
    ImGui::Text("Selected x: %d, y: %d", m_selected.x, m_selected.y);

    ImGui::End();
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

    MDCII_ASSERT(width == Map::TILE_WIDTH, "[MousePicker::Init()] Invalid width.")
    MDCII_ASSERT(height == Map::TILE_HEIGHT, "[MousePicker::Init()] Invalid height.")

    Log::MDCII_LOG_DEBUG("[MousePicker::Init()] The mouse picker was successfully initialized.");
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::map::MousePicker::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[MousePicker::CleanUp()] CleanUp MousePicker.");

    stbi_image_free(m_cornerImage);
}
