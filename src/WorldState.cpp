#include <glm/ext/matrix_transform.hpp>
#include <imgui.h>
#include "WorldState.h"
#include "Log.h"
#include "renderer/MeshRenderer.h"
#include "file/BshFile.h"
#include "ogl/OpenGL.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::WorldState::WorldState(const Id t_id, state::StateStack* t_stateStack, std::shared_ptr<Context> t_context)
    : State(t_id, t_stateStack, std::move(t_context))
{
    Log::MDCII_LOG_DEBUG("[WorldState::WorldState()] Create WorldState.");

    Init();
}

mdcii::WorldState::~WorldState() noexcept
{
    Log::MDCII_LOG_DEBUG("[WorldState::~WorldState()] Destruct WorldState.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::WorldState::Input()
{
    // m_camera->Input()

    // ESC for quit
    if (context->window->IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        Log::MDCII_LOG_INFO("[WorldState::Input()] Starts POP WorldState.");
        RequestStackPop();
    }
}

void mdcii::WorldState::Update()
{
    m_camera->Update();
}

void mdcii::WorldState::Render()
{
    auto getModelMatrix = [](const glm::vec2& t_screenPosition, const glm::vec2& t_size)
    {
        auto modelMatrix{ glm::mat4(1.0f) };

        modelMatrix = translate(modelMatrix, glm::vec3(t_screenPosition.x, t_screenPosition.y, 0.0f));
        modelMatrix = scale(modelMatrix, glm::vec3(t_size.x, t_size.y, 1.0f));

        return modelMatrix;
    };

    auto mapToScreen = [](const int t_x, const int t_y) -> glm::vec2
    {
        return {
            (t_x - t_y) << 5,
            (t_x + t_y) << 4
        };
    };

    for (int y = 0; y < 350; ++y)
    {
        for (int x = 0; x < 500; ++x)
        {
            auto s = mapToScreen(x, y);

            if (s.x < m_camera->position.x || s.y < m_camera->position.y)
            {
                continue;
            }

            if (s.x > m_camera->position.x + 640 - 64 || s.y > m_camera->position.y + 480 - 32)
            {
                continue;
            }

            m_renderer->Render(
                getModelMatrix(s, glm::vec2(64.0f, 32.0f)),
                758,
                *context->window,
                *m_camera
            );
        }
    }
}

void mdcii::WorldState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    ImGui::Begin("World");
    ImGui::End();

    ogl::Window::ImGuiEnd();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::WorldState::Init()
{
    Log::MDCII_LOG_DEBUG("[WorldState::Init()] Initializing world state.");

    m_paletteFile = std::make_unique<file::PaletteFile>("STADTFLD.COL");
    m_paletteFile->ReadDataFromChunks();

    m_bshFile = std::make_unique<file::BshFile>("STADTFLD.BSH", m_paletteFile->palette);
    m_bshFile->ReadDataFromChunks();

    m_camera = std::make_unique<camera::Camera>(context->window, glm::vec2(0.0f, 0.0f));

    m_renderer = std::make_unique<renderer::MeshRenderer>();

    Log::MDCII_LOG_DEBUG("[WorldState::Init()] The world state was successfully initialized.");
}
