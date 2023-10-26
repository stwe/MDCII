// This file is part of the MDCII project.
//
// Copyright (c) 2023. stwe <https://github.com/stwe/MDCII>
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include "StateId.h"
#include "MdciiAssert.h"
#include "vendor/enum/magic_enum.hpp"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii
{
    /**
     * @brief Forward declaration class Game.
     */
    class Game;
}

namespace mdcii::state
{
    /**
     * @brief Forward declaration class State.
     */
    class State;

    //-------------------------------------------------
    // StateStack
    //-------------------------------------------------

    /**
     * @brief Class to manage all the State objects.
     */
    class StateStack
    {
    public:
        //-------------------------------------------------
        // Action
        //-------------------------------------------------

        /**
         * @brief Possible stack operations.
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
         * @brief Constructs a new StateStack object.
         *
         * @param t_game Pointer to the parent Game.
         */
        explicit StateStack(Game* t_game);

        StateStack(const StateStack& t_other) = delete;
        StateStack(StateStack&& t_other) noexcept = delete;
        StateStack& operator=(const StateStack& t_other) = delete;
        StateStack& operator=(StateStack&& t_other) noexcept = delete;

        ~StateStack() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * @brief Returns whether the stack is empty.
         */
        [[nodiscard]] bool IsEmpty() const { return m_stack.empty(); }

        //-------------------------------------------------
        // Register
        //-------------------------------------------------

        /**
         * @brief Registers a State factory function so that it can be created on demand.
         *
         * @tparam T The derived State class.
         * @param t_id The unique identifier of the State object.
         */
        template<typename T>
        void RegisterState(const StateId t_id)
        {
            MDCII_LOG_DEBUG("[StateStack::RegisterState()] Register factory function for state {}.", magic_enum::enum_name(t_id));

            MDCII_ASSERT(m_game, "[StateStack::RegisterState()] Null pointer.")
            m_factories[t_id] = [this, t_id]() { return std::make_unique<T>(t_id, m_game); };
        }

        //-------------------------------------------------
        // Stack operations
        //-------------------------------------------------

        void PushState(StateId t_id);
        void PopState(StateId t_id);
        void ClearStates();

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void OnUserCreate();
        void OnUserUpdate(float t_elapsedTime);
        void RenderImGui();

    protected:

    private:
        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        /**
         * @brief A pending stack operation.
         */
        struct PendingChange
        {
            PendingChange(const Action t_action, const StateId t_id)
                : action{ t_action }
                , id{ t_id }
            {}

            Action action;
            StateId id;
        };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the parent Game.
         */
        Game* m_game{ nullptr };

        /**
         * @brief The created State objects.
         */
        std::vector<std::unique_ptr<State>> m_stack;

        /**
         * @brief A list with pending stack operations.
         */
        std::vector<PendingChange> m_pendingChanges;

        /**
         * @brief A list of factory functions for each Stack object.
         */
        std::unordered_map<StateId, std::function<std::unique_ptr<State>()>> m_factories;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * @brief Returns a pointer to a newly created object of the corresponding State class.
         *
         * @param t_id The unique identifier of a State.
         *
         * @return The created State object.
         */
        std::unique_ptr<State> CreateState(StateId t_id);

        /**
         * @brief Applies stack operations from a list.
         */
        void ApplyPendingChanges();
    };
}
