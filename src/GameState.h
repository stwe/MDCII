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
#include "resource/MdciiFile.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * @brief Forward declaration class World.
     */
    class World;
}

namespace mdcii
{
    //-------------------------------------------------
    // GameState
    //-------------------------------------------------

    /**
     * @brief Represents the GameState.
     */
    class GameState : public state::State
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        GameState() = delete;

        /**
         * @brief Constructs a new GameState object.
         *
         * @param t_game Pointer to the parent Game object.
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

        [[nodiscard]] bool OnUserCreate(void* t_data) override;
        [[nodiscard]] bool OnUserUpdate(float t_elapsedTime) override;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        std::unique_ptr<world::World> m_world;

        //-------------------------------------------------
        // Create world
        //-------------------------------------------------

        /**
         * @brief Creates the world from a given MDCII Json file.
         *
         * @param t_mdciiFile The MDCII Json file.
         *
         * @return Returns true if no error occurred, false otherwise.
         */
        [[nodiscard]] bool CreateWorldFromFile(resource::MdciiFile& t_mdciiFile);

        //-------------------------------------------------
        // ImGui
        //-------------------------------------------------

        [[nodiscard]] bool RenderImGui() const;
    };
}
