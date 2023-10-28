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

#include "world/Zoom.h"
#include "world/Rotation.h"
#include "state/State.h"

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

namespace mdcii::world
{
    /**
     * @brief Forward declaration class Island.
     */
    class Island;
}

namespace mdcii::renderer
{
    /**
     * @brief Forward declaration class Renderer.
     */
    class Renderer;
}

namespace mdcii
{
    //-------------------------------------------------
    // GameState
    //-------------------------------------------------

    class GameState : public state::State
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        world::Zoom zoom{ world::Zoom::GFX };
        world::Rotation rotation{ world::Rotation::DEG0 };
        std::vector<std::unique_ptr<world::Island>> islands;
        std::unique_ptr<renderer::Renderer> renderer;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        GameState() = delete;

        /**
         * @brief Constructs a new GameState object.
         *
         * @param t_game Pointer to the parent Game.
         */
        explicit GameState(Game* t_game);

        GameState(const GameState& t_other) = delete;
        GameState(GameState&& t_other) noexcept = delete;
        GameState& operator=(const GameState& t_other) = delete;
        GameState& operator=(GameState&& t_other) noexcept = delete;

        ~GameState() noexcept override;

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        [[nodiscard]] bool OnUserCreate() override;
        [[nodiscard]] bool OnUserUpdate(float t_elapsedTime) override;

    protected:

    private:

    };
}
