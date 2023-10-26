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
     * @brief Forward declaration enum class StateId.
     */
    enum class StateId;

    //-------------------------------------------------
    // State
    //-------------------------------------------------

    /**
     * @brief The base class for all States.
     */
    class State
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The unique identifier of this State.
         */
        StateId id;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        State() = delete;

        /**
         * @brief Constructs a new State object.
         *
         * @param t_id The unique identifier.
         * @param t_game Pointer to the parent Game.
         */
        State(StateId t_id, Game* t_game);

        State(const State& t_other) = delete;
        State(State&& t_other) noexcept = delete;
        State& operator=(const State& t_other) = delete;
        State& operator=(State&& t_other) noexcept = delete;

        virtual ~State() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        virtual void OnUserCreate() = 0;
        virtual void OnUserUpdate(float t_elapsedTime) = 0;
        virtual void RenderImGui() = 0;

    protected:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the parent Game.
         */
        Game* m_game{ nullptr };

    private:

    };
}
