#include <imgui.h>
#include "WorldState.h"
#include "Log.h"
#include "ogl/OpenGL.h"
#include "ogl/Window.h"

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
    // ESC for quit
    if (context->window->IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        Log::MDCII_LOG_INFO("[WorldState::Input()] Starts POP WorldState.");
        RequestStackPop();
    }
}

void mdcii::WorldState::Update()
{
}

void mdcii::WorldState::Render()
{
}

void mdcii::WorldState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    ImGui::Begin("WorldState");


    ImGui::End();

    ogl::Window::ImGuiEnd();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::WorldState::Init()
{
    Log::MDCII_LOG_DEBUG("[WorldState::Init()] Initializing world state.");

    Log::MDCII_LOG_DEBUG("[WorldState::Init()] The world state was successfully initialized.");
}
