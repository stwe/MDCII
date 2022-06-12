#include "State.h"
#include "StateStack.h"
#include "MdciiAssert.h"
#include "ogl/Window.h"
#include "ogl/OpenGL.h"
#include "data/Buildings.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::state::State::State(const Id t_id, StateStack* t_stateStack, std::shared_ptr<Context> t_context)
    : context{ std::move(t_context) }
    , m_id{ t_id }
    , m_stateStack{ t_stateStack }
{
    MDCII_ASSERT(t_stateStack, "[State::State()] Null pointer")

    Log::MDCII_LOG_DEBUG("[State::State()] Create State.");
}

mdcii::state::State::~State() noexcept
{
    Log::MDCII_LOG_DEBUG("[State::~State()] Destruct State.");
}

//-------------------------------------------------
// Frame
//-------------------------------------------------

void mdcii::state::State::StartFrame()
{
    ogl::OpenGL::SetClearColor(0.39f, 0.58f, 0.93f, 1.0f);
    ogl::OpenGL::Clear();
}

void mdcii::state::State::EndFrame() const
{
    context->window->SwapBuffersAndCallEvents();
}

//-------------------------------------------------
// Stack operations
//-------------------------------------------------

void mdcii::state::State::RequestStackPush(const Id t_id) const
{
    m_stateStack->PushState(t_id);
}

void mdcii::state::State::RequestStackPop() const
{
    m_stateStack->PopState(m_id);
}

void mdcii::state::State::RequestStateClear() const
{
    m_stateStack->ClearStates();
}
