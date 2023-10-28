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

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace olc
{
    /**
     * @brief Forward declaration class PixelGameEngine.
     */
    class PixelGameEngine;
}

namespace mdcii::state
{
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
        LOADED_GAME
    };

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
        std::map<StateId, State*> states;

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
        // Add && Init
        //-------------------------------------------------

        /**
         * @brief Add a new state to the state system.
         *
         * @param t_stateId The unique identifier of the new state.
         * @param t_state The state to be added.
         */
        void AddState(StateId t_stateId, State* t_state);

        /**
         * @brief Initialize a state.
         *
         * @param t_stateId The unique identifier of the state to be initialized.
         */
        void InitState(StateId t_stateId);

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * @brief Process input events in the current state.
         *
         * @param t_pge The game engine which handles the input events.
         */
        void Input(olc::PixelGameEngine* t_pge) const;

        /**
         * @brief Render the current state to the given game engine.
         *
         * @param t_pge The game engine which renders the game.
         * @param t_elapsedTime Time elapsed since the last render.
         */
        void Render(olc::PixelGameEngine* t_pge, float t_elapsedTime) const;

    protected:

    private:

    };
}
