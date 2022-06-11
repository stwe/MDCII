#pragma once

#include "state/State.h"

//-------------------------------------------------
// WorldState
//-------------------------------------------------

namespace mdcii
{
    class WorldState : public state::State
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        WorldState() = delete;

        /**
         * Constructs a new WorldState object.
         *
         * @param t_id The unique identifier of the State.
         * @param t_stateStack A pointer to the parent StateStack object.
         * @param t_context The holder of shared objects.
         */
        WorldState(Id t_id, state::StateStack* t_stateStack, std::shared_ptr<Context> t_context);

        WorldState(const WorldState& t_other) = delete;
        WorldState(WorldState&& t_other) noexcept = delete;
        WorldState& operator=(const WorldState& t_other) = delete;
        WorldState& operator=(WorldState&& t_other) noexcept = delete;

        ~WorldState() noexcept override;

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        void Input() override;
        void Update() override;
        void PreRender() override {}
        void Render() override;
        void RenderImGui() override;

    protected:

    private:
        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init();
    };
}
