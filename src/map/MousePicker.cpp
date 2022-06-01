#include <imgui.h>
#include "MousePicker.h"
#include "renderer/TileRenderer.h"
#include "renderer/Utils.h"
#include "ogl/resource/ResourceManager.h"

mdcii::map::MousePicker::MousePicker()
{
    Init();
}

mdcii::map::MousePicker::~MousePicker() noexcept
{
}

void mdcii::map::MousePicker::Render(const ogl::Window& t_window, const camera::Camera& t_camera)
{
    m_mouse = glm::ivec2(t_window.GetMouseX(), t_window.GetMouseY());
    m_cell = glm::ivec2(m_mouse.x / 64, m_mouse.y / 32);
    m_offsetIntoCell = glm::ivec2( m_mouse.x % 64, m_mouse.y % 32 );

    const glm::ivec2 origin{
        static_cast<int>(t_camera.position.x) / 64,
    static_cast<int>(t_camera.position.y) / 32
    };

    m_selected = glm::ivec2(
        (m_cell.y + origin.y) + (m_cell.x + origin.x),
        (m_cell.y + origin.y) - (m_cell.x + origin.x)
    );

    auto MapToIso = [](const int t_mapX, const int t_mapY)
    {
        return glm::vec2{
            (t_mapX - t_mapY) << 5, // 32
            (t_mapX + t_mapY) << 4  // 16
        };
    };

    m_renderer->RenderTile(
        renderer::Utils::GetModelMatrix(
            MapToIso(m_selected.x, m_selected.y),
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

    ImGui::End();
}

void mdcii::map::MousePicker::Init()
{
    m_renderer = std::make_unique<renderer::TileRenderer>();
}
