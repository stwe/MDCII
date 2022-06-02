#include <imgui.h>
#include "MousePicker.h"
#include "Map.h"
#include "Log.h"
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
    m_cell = glm::ivec2(m_mouse.x / 64, m_mouse.y / 32);
    m_offsetIntoCell = glm::ivec2(m_mouse.x % 64, m_mouse.y % 32);

    const glm::ivec2 origin{
        static_cast<int>(t_camera.position.x) / 64,
    static_cast<int>(t_camera.position.y) / 32
    };

    const auto* offset{ m_cornerImage + (4 * (m_offsetIntoCell.y * 64 + m_offsetIntoCell.x)) };
    const auto r = offset[0];
    const auto g = offset[1];
    const auto b = offset[2];

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

    m_renderer->RenderTile(
        renderer::Utils::GetModelMatrix(
            m_map->MapToIso(m_selected.x, m_selected.y, m_map->rotation), // todo: rotation remove arg.
            glm::vec2(64.0f, 32.0f)
        ),
        ogl::resource::ResourceManager::LoadTexture("resources/textures/frame.png").id,
        t_window, t_camera
    );

    /*
    m_renderer->RenderTile(
        renderer::Utils::GetModelMatrix(
            glm::vec2(m_cell.x * 64, m_cell.y * 32),
            glm::vec2(64.0f, 32.0f)
        ),
        ogl::resource::ResourceManager::LoadTexture("resources/textures/frame.png").id,
        t_window, t_camera
    );
    */
}

void mdcii::map::MousePicker::RenderImGui()
{
    ImGui::Begin("MousePicker", nullptr, 0);

    ImGui::Text("Mouse x: %d, y: %d", m_mouse.x, m_mouse.y);
    ImGui::Text("Cell x: %d, y: %d", m_cell.x, m_cell.y);
    ImGui::Text("Selected x: %d, y: %d", m_selected.x, m_selected.y);

    const auto* offset{ m_cornerImage + (4 * (m_offsetIntoCell.y * 64 + m_offsetIntoCell.x)) };
    const auto r = offset[0];
    const auto g = offset[1];
    const auto b = offset[2];

    ImGui::Text("r: %d", r);
    ImGui::Text("g: %d", g);
    ImGui::Text("b: %d", b);

    ImGui::End();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::map::MousePicker::Init()
{
    Log::MDCII_LOG_DEBUG("[MousePicker::Init()] Initializing mouse picker.");

    m_renderer = std::make_unique<renderer::TileRenderer>();

    const std::string path{ "E:/Dev/MDCII/resources/textures/corner.png" };
    int width, height, channels;
    m_cornerImage = stbi_load(path.c_str(), &width, &height, &channels, 0);

    Log::MDCII_LOG_DEBUG("[MousePicker::Init()] The mouse picker was successfully initialized.");
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::map::MousePicker::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[MousePicker::CleanUp()] Clean up MousePicker.");

    stbi_image_free(m_cornerImage);
}
