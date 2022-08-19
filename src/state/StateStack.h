// This file is part of the MDCII project.
//
// Copyright (c) 2022. stwe <https://github.com/stwe/MDCII>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#pragma once

#include <vector>
#include <unordered_map>
#include "State.h"
#include "Log.h"

//-------------------------------------------------
// StateStack
//-------------------------------------------------

namespace mdcii::state
{
    /**
     * Class to manage all the State objects.
     */
    class StateStack
    {
    public:
        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        /**
         * Possible stack operations.
         */
        enum class Action
        {
            PUSH,
            POP,
            CLEAR
        };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        StateStack() = delete;

        /**
         * Constructs a new StateStack object.
         *
         * @param t_context The holder of shared objects.
         */
        explicit StateStack(std::shared_ptr<Context> t_context);

        StateStack(const StateStack& t_other) = delete;
        StateStack(StateStack&& t_other) noexcept = delete;
        StateStack& operator=(const StateStack& t_other) = delete;
        StateStack& operator=(StateStack&& t_other) noexcept = delete;

        ~StateStack() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * Returns whether the stack is empty.
         */
        [[nodiscard]] bool IsEmpty() const { return m_stack.empty(); }

        //-------------------------------------------------
        // Register
        //-------------------------------------------------

        /**
         * Registers a State factory function so that it can be created on demand.
         *
         * @tparam T The derived state class.
         * @param t_id The unique identifier of the state.
         */
        template<typename T>
        void RegisterState(const State::Id t_id)
        {
            Log::MDCII_LOG_DEBUG("[StateStack::RegisterState()] Register factory function for state {}.", State::STATE_IDS.at(static_cast<int>(t_id)));
            m_factories[t_id] = [this, t_id]() { return std::make_unique<T>(t_id, this, m_context); };
        }

        //-------------------------------------------------
        // Stack operations
        //-------------------------------------------------

        void PushState(State::Id t_id);
        void PopState(State::Id t_id);
        void ClearStates();

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Input();
        void Update();
        void Render() const;

    protected:

    private:
        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        /**
         * A pending stack operation.
         */
        struct PendingChange
        {
            PendingChange(const Action t_action, const State::Id t_id)
                : action{ t_action }
                , id{ t_id }
            {}

            Action action;
            State::Id id;
        };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The created state objects.
         */
        std::vector<std::unique_ptr<State>> m_stack;

        /**
         * A list with pending stack operations.
         */
        std::vector<PendingChange> m_pendingChanges;

        /**
         * Works as a holder of shared objects between all states.
         */
        std::shared_ptr<Context> m_context;

        /**
         * A list of factory functions for each Stack object.
         */
        std::unordered_map<State::Id, std::function<std::unique_ptr<State>()>> m_factories;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Takes an Id of a state, and returns a smart pointer to a newly
         * created object of the corresponding state class.
         *
         * @param t_id The unique identifier of a state.
         *
         * @return The created state object.
         */
        std::unique_ptr<State> CreateState(State::Id t_id);

        /**
         * Applies stack operations from a list.
         */
        void ApplyPendingChanges();
    };
}
