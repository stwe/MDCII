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

#include <map>
#include <memory>

namespace mdcii::state
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * @brief Forward declaration class State.
     */
    class State;

    //-------------------------------------------------
    // StateId
    //-------------------------------------------------

    /**
     * @brief The unique identifiers of the State objects.
     */
    enum class StateId
    {
        MAIN_MENU,
        ISLAND_GENERATOR,
        WORLD_GENERATOR,
        NEW_GAME,
        LOAD_GAME
    };

    //-------------------------------------------------
    // StateSystem
    //-------------------------------------------------

    /**
     * @brief Manages the states of the game.
     */
    class StateSystem
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief A map of game states with their unique identifiers as keys.
         */
        std::map<StateId, std::unique_ptr<State>> states;

        /**
         * @brief Pointer to the current game state.
         */
        State* currentState{ nullptr };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        StateSystem();

        StateSystem(const StateSystem& t_other) = delete;
        StateSystem(StateSystem&& t_other) noexcept = delete;
        StateSystem& operator=(const StateSystem& t_other) = delete;
        StateSystem& operator=(StateSystem&& t_other) noexcept = delete;

        ~StateSystem() noexcept;

        //-------------------------------------------------
        // Add && Change
        //-------------------------------------------------

        /**
         * @brief Add a new state to the state system.
         *
         * @param t_stateId The unique identifier of the new state.
         * @param t_state The state to be added.
         */
        void AddState(StateId t_stateId, std::unique_ptr<State> t_state);

        /**
         * @brief Changes to the given state.
         *
         * @param t_stateId The state to change to.
         */
        void ChangeState(StateId t_stateId);

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        [[nodiscard]] bool OnUserCreate() const;
        [[nodiscard]] bool OnUserUpdate(float t_elapsedTime) const;

    protected:

    private:

    };
}
