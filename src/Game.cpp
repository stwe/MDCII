#include <sstream>
#include "Game.h"
#include "WorldState.h"
#include "state/StateStack.h"
#include "ogl/Window.h"
#include "ogl/OpenGL.h"

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

    m_stateStack = std::make_unique<state::StateStack>(std::make_unique<state::State::Context>(m_window));
    m_stateStack->RegisterState<WorldState>(state::State::Id::WORLD);
    m_stateStack->PushState(state::State::Id::WORLD);

    Log::MDCII_LOG_DEBUG("[Game::Init()] The game was successfully initialized.");
}

void mdcii::Game::Input()
{
    m_stateStack->Input();
}

void mdcii::Game::Update()
{
    m_stateStack->Update();
}

void mdcii::Game::Render()
{
    m_stateStack->Render();
}

//-------------------------------------------------
// Game loop
//-------------------------------------------------

void mdcii::Game::GameLoop()
{
    Log::MDCII_LOG_DEBUG("[Game::GameLoop()] Starting the game loop.");

    auto previousTime{ glfwGetTime() };
    auto lag{ 0.0 };

    auto infoTimer{ previousTime };
    auto render{ 0 };
    auto updates{ 0 };

    while (!m_window->WindowShouldClose())
    {
        const auto currentTime{ glfwGetTime() };
        const auto elapsedTime{ currentTime - previousTime };
        previousTime = currentTime;

        lag += elapsedTime;

        Input();

        while (lag >= FRAME_TIME)
        {
            Update();
            updates++;
            lag -= FRAME_TIME;
        }

        if (m_stateStack->IsEmpty())
        {
            m_window->Close();
        }

        Render();
        render++;

        if (glfwGetTime() - infoTimer > 1.0)
        {
            infoTimer++;

            std::stringstream ss;
#ifdef MDCII_DEBUG_BUILD
            ss << m_window->GetTitle() << " [DEBUG BUILD] " << "   |   Render: " << render << "   |   Updates: " << updates;
#else
            ss << m_window->GetTitle() << " [RELEASE BUILD] " << "   |   Render: " << render << "   |   Updates: " << updates;
#endif
            glfwSetWindowTitle(m_window->GetWindowHandle(), ss.str().c_str());

            updates = 0;
            render = 0;
        }
    }

    Log::MDCII_LOG_DEBUG("[Game::GameLoop()] The game loop has ended.");
}
