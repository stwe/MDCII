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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#pragma once

#include "state/State.h"
#include "Game.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * Forward declaration class World.
     */
    class World;
}

//-------------------------------------------------
// GameState
//-------------------------------------------------

namespace mdcii
{
    /**
     * A state to run the game.
     */
    class GameState: public state::State
    {
    public:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * The file extension for map files.
         */
        const std::string MAP_FILE_EXTENSION{ ".map" };

        /**
         * The file extension for save game files.
         */
        const std::string SAVE_GAME_FILE_EXTENSION{ ".sav" };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        GameState() = delete;

        /**
         * Constructs a new GameState object.
         *
         * @param t_id The unique identifier of the State.
         * @param t_context The holder of shared objects.
         */
        GameState(state::StateId t_id, std::shared_ptr<state::Context> t_context);

        GameState(const GameState& t_other) = delete;
        GameState(GameState&& t_other) noexcept = delete;
        GameState& operator=(const GameState& t_other) = delete;
        GameState& operator=(GameState&& t_other) noexcept = delete;

        ~GameState() noexcept override;

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        void Input() override;
        void Update() override;
        void Render() override;
        void RenderImGui() override;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The World object.
         */
        std::shared_ptr<world::World> m_world;

        /**
         * A list of map files.
         */
        std::vector<std::string> m_mapFiles;

        /**
         * A list of saved game files.
         */
        std::vector<std::string> m_savedGameFiles;

        //-------------------------------------------------
        // ImGui
        //-------------------------------------------------

        /**
         * Provides a simple mechanism to choose a file.
         *
         * @param t_files A list of files.
         */
        void RenderFileChooser(std::vector<std::string>& t_files);

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes the state.
         */
        void Init();
    };
}
