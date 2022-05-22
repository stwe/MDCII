#include "StateStack.h"
#include "MdciiAssert.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::state::StateStack::StateStack(std::shared_ptr<State::Context> t_context)
    : m_context{ std::move(t_context) }
{
    Log::MDCII_LOG_DEBUG("[StateStack::StateStack()] Create StateStack.");
}

mdcii::state::StateStack::~StateStack() noexcept
{
    Log::MDCII_LOG_DEBUG("[StateStack::~StateStack()] Destruct StateStack.");
}

//-------------------------------------------------
// Stack operations
//-------------------------------------------------

void mdcii::state::StateStack::PushState(const State::Id t_id)
{
    Log::MDCII_LOG_DEBUG("[StateStack::PushState()] Add pending stack operation PUSH for state {}.", State::STATE_IDS.at(static_cast<int>(t_id)));
    m_pendingChanges.emplace_back(Action::PUSH, t_id);
}

void mdcii::state::StateStack::PopState(const State::Id t_id)
{
    Log::MDCII_LOG_DEBUG("[StateStack::PushState()] Add pending stack operation POP for state {}.", State::STATE_IDS.at(static_cast<int>(t_id)));
    m_pendingChanges.emplace_back(Action::POP, t_id);
}

void mdcii::state::StateStack::ClearStates()
{
    Log::MDCII_LOG_DEBUG("[StateStack::ClearStates()] Add pending stack operation CLEAR all states.");
    m_pendingChanges.emplace_back(Action::CLEAR, State::Id::ALL);
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::state::StateStack::Input()
{
    // iterate from top to bottom
    for (auto itr{ m_stack.rbegin() }; itr != m_stack.rend(); ++itr)
    {
        (*itr)->Input();
    }

    ApplyPendingChanges();
}

void mdcii::state::StateStack::Update()
{
    // iterate from top to bottom
    for (auto itr{ m_stack.rbegin() }; itr != m_stack.rend(); ++itr)
    {
        (*itr)->Update();
    }

    ApplyPendingChanges();
}

void mdcii::state::StateStack::Render() const
{
    for (const auto& state : m_stack)
    {
        state->PreRender();
    }

    for (const auto& state : m_stack)
    {
        state->StartFrame();

        state->Render();
        state->RenderImGui();

        state->EndFrame();
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

std::unique_ptr<mdcii::state::State> mdcii::state::StateStack::CreateState(const State::Id t_id)
{
    Log::MDCII_LOG_DEBUG("[StateStack::CreateState()] Running factory function for state {}.", State::STATE_IDS.at(static_cast<int>(t_id)));

    const auto it{ m_factories.find(t_id) };
    MDCII_ASSERT(it != m_factories.end(), "[StateStack::CreateState()] Factory function not found for state " + std::string(State::STATE_IDS.at(static_cast<int>(t_id))) + ".")

    return it->second();
}

void mdcii::state::StateStack::ApplyPendingChanges()
{
    for (const auto& change : m_pendingChanges)
    {
        switch (change.action)
        {
            case Action::PUSH:
                // add element at the end
                m_stack.emplace_back(CreateState(change.id));
                Log::MDCII_LOG_INFO("Stack size is {} after PUSH state {}.", m_stack.size(), State::STATE_IDS.at(static_cast<int>(change.id)));
                break;
            case Action::POP:
                // removes the last element in the vector
                MDCII_ASSERT(change.id == m_stack.back()->GetId(), "[StateStack::ApplyPendingChanges()] Invalid POP operation.")
                m_stack.pop_back();
                Log::MDCII_LOG_INFO("Stack size is {} after POP state {}.", m_stack.size(), State::STATE_IDS.at(static_cast<int>(change.id)));
                break;
            case Action::CLEAR:
                m_stack.clear();
                Log::MDCII_LOG_INFO("Stack size is {} after CLEAR all states.", m_stack.size());
                break;
        }
    }

    m_pendingChanges.clear();
}
