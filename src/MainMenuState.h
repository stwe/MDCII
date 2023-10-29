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
    // MainMenuState
    //-------------------------------------------------

    /**
     * @brief Represents the GameState.
     */
    class MainMenuState : public state::State
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MainMenuState() = delete;

        /**
         * @brief Constructs a new MainMenuState object.
         *
         * @param t_game Pointer to the parent Game.
         */
        explicit MainMenuState(Game* t_game);

        MainMenuState(const MainMenuState& t_other) = delete;
        MainMenuState(MainMenuState&& t_other) noexcept = delete;
        MainMenuState& operator=(const MainMenuState& t_other) = delete;
        MainMenuState& operator=(MainMenuState&& t_other) noexcept = delete;

        ~MainMenuState() noexcept override;

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        [[nodiscard]] bool OnUserCreate() override;
        [[nodiscard]] bool OnUserUpdate(float t_elapsedTime) override;

    protected:

    private:

    };
}
