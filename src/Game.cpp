#include <sstream>
#include <glm/ext/matrix_transform.hpp>
#include "Game.h"
#include "Log.h"
#include "ogl/OpenGL.h"
#include "file/BshFile.h"
#include "renderer/MeshRenderer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::Game::Game()
{
    Log::MDCII_LOG_DEBUG("[Game::Game()] Create Game.");
}

mdcii::Game::~Game() noexcept
{
    Log::MDCII_LOG_DEBUG("[Game::~Game()] Destruct Game.");
}

//-------------------------------------------------
// Run
//-------------------------------------------------

void mdcii::Game::Run()
{
    Log::MDCII_LOG_DEBUG("[Game::Run()] Starts running game.");

    Init();
    GameLoop();
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::Game::Init()
{
    Log::MDCII_LOG_DEBUG("[Game::Init()] Initializing game.");

    m_window = std::make_shared<ogl::Window>();

    m_paletteFile = std::make_unique<file::PaletteFile>("STADTFLD.COL");
    m_paletteFile->ReadDataFromChunks();

    m_bshFile = std::make_unique<file::BshFile>("STADTFLD.BSH", m_paletteFile->palette);
    m_bshFile->ReadDataFromChunks();

    m_camera = std::make_unique<camera::Camera>(m_window, glm::vec2(0.0f, 0.0f));

    m_renderer = std::make_unique<renderer::MeshRenderer>();

    Log::MDCII_LOG_DEBUG("[Game::Init()] The game was successfully initialized.");
}

void mdcii::Game::Input()
{
    if (m_window->IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        m_window->Close();
    }
}

void mdcii::Game::Update()
{
    m_camera->Update();
}

void mdcii::Game::Render()
{
    ogl::OpenGL::SetClearColor(0.39f, 0.58f, 0.93f, 1.0f);
    ogl::OpenGL::Clear();

    auto getModelMatrix = [](const glm::vec2& t_screenPosition, const glm::vec2& t_size)
    {
        auto modelMatrix{ glm::mat4(1.0f) };

        modelMatrix = translate(modelMatrix, glm::vec3(t_screenPosition.x, t_screenPosition.y, 0.0f));
        modelMatrix = scale(modelMatrix, glm::vec3(t_size.x, t_size.y, 1.0f));

        return modelMatrix;
    };

    auto mapToScreen = [](const int t_x, const int t_y) -> glm::vec2
    {
        const auto x{ static_cast<float>(t_x) };
        const auto y{ static_cast<float>(t_y) };

        return {
            (x - y) * 32.0f,
            (x + y) * 16.0f
        };
    };

    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 4; ++x)
        {
            m_renderer->Render(
                getModelMatrix(mapToScreen(x, y), glm::vec2(64.0f, 32.0f)),
                758,
                *m_window,
                *m_camera
            );
        }
    }

    m_window->SwapBuffersAndCallEvents();
}

//-------------------------------------------------
// Game loop
//-------------------------------------------------

void mdcii::Game::GameLoop()
{
    Log::MDCII_LOG_DEBUG("[Game::GameLoop()] Starting the game loop.");

    auto lastTime{ glfwGetTime() };
    auto resetTimer{ lastTime };

    auto dt{ 0.0 };
    auto fps{ 0 };
    auto updates{ 0 };

    while (!m_window->WindowShouldClose())
    {
        const auto now{ glfwGetTime() };
        dt += (now - lastTime) / FRAME_TIME;
        lastTime = now;

        Input();

        while (dt >= 1.0)
        {
            Update();
            updates++;
            dt--;
        }

        Render();
        fps++;

        if (glfwGetTime() - resetTimer > 1.0)
        {
            resetTimer++;

            std::stringstream ss;
#ifdef MDCII_DEBUG_BUILD
            ss << m_window->GetTitle() << " [DEBUG BUILD] " << "   |   Fps: " << fps << "   |   Updates: " << updates;
#else
            ss << m_window->GetTitle() << " [RELEASE BUILD] " << "   |   Fps: " << fps << "   |   Updates: " << updates;
#endif
            glfwSetWindowTitle(m_window->GetWindowHandle(), ss.str().c_str());

            updates = 0;
            fps = 0;
        }
    }

    Log::MDCII_LOG_DEBUG("[Game::GameLoop()] The game loop has ended.");
}
