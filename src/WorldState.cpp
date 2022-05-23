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
    // todo
    // m_camera->Input()

    /*
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }
    */

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

    /*
    for (int y = 0; y < WORLD_HEIGHT; ++y)
    {
        for (int x = 0; x < WORLD_WIDTH; ++x)
        {
            const auto s{ mapToScreen(x, y) };

            if (s.x < m_camera->position.x || s.y < m_camera->position.y)
            {
                continue;
            }

            if (s.x > m_camera->position.x + static_cast<float>(context->window->GetWidth()) - 64 ||
                s.y > m_camera->position.y + static_cast<float>(context->window->GetHeight()) - 32
                )
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
    */

    /*
    const auto w{ m_stdBshFile->bshTextures[5373]->width };
    const auto h{ m_stdBshFile->bshTextures[5373]->height };

    auto now = glfwGetTime();
    static double old = 0.0;
    auto diff = now - old;

    auto t{ 220.0 / 1000.0 }; // oder per Frame = (220.0 / 24.0) / 1000.0

    if (diff >= t)
    {
        m_frame = (m_frame + 1) % 24;
        old = now;
    }

    m_renderer->Render(
        getModelMatrix(mapToScreen(20, 2), glm::vec2(w, h)),
        5373 + m_frame,
        *context->window,
        *m_camera
    );
    */

    auto now = glfwGetTime();
    static double old = 0.0;
    auto diff = now - old;

    auto t{ 110.0 / 1000.0 };

    if (diff >= t)
    {
        m_frame = (m_frame + 1) % 8;
        old = now;
    }

    m_renderer->Render(
        getModelMatrix(mapToScreen(10, 10),
            glm::vec2
            (
                m_traegerBshFile->bshTextures[0]->width,
                m_traegerBshFile->bshTextures[0]->height)
            ),
        m_traegerBshFile->bshTextures[0]->textureId + m_frame,
        *context->window,
        *m_camera
    );
}

void mdcii::WorldState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    ImGui::Begin("World", nullptr, 0);
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

    m_stdBshFile = std::make_unique<file::BshFile>("STADTFLD.BSH", m_paletteFile->palette);
    m_stdBshFile->ReadDataFromChunks();

    m_traegerBshFile = std::make_unique<file::BshFile>("TRAEGER.BSH", m_paletteFile->palette);
    m_traegerBshFile->ReadDataFromChunks();

    m_camera = std::make_unique<camera::Camera>(context->window, glm::vec2(0.0f, 0.0f));

    m_renderer = std::make_unique<renderer::MeshRenderer>();

    Log::MDCII_LOG_DEBUG("[WorldState::Init()] The world state was successfully initialized.");
}
