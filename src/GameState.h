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

#include "state/State.h"

namespace mdcii
{
    //-------------------------------------------------
    // GameState
    //-------------------------------------------------

    class GameState : public state::State
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        GameState() = delete;

        /**
         * Constructs a new GameState object.
         *
         * @param t_id The unique identifier of the State.
         * @param t_game Pointer to the Game.
         */
        GameState(state::StateId t_id, Game* t_game);

        GameState(const GameState& t_other) = delete;
        GameState(GameState&& t_other) noexcept = delete;
        GameState& operator=(const GameState& t_other) = delete;
        GameState& operator=(GameState&& t_other) noexcept = delete;

        ~GameState() noexcept override;

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        void OnUserCreate() override;
        void OnUserUpdate(float t_elapsedTime) override;
        void RenderImGui() override;

    protected:

    private:

    };
}
