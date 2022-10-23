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

//-------------------------------------------------
// WorldGeneratorState
//-------------------------------------------------

namespace mdcii
{
    /**
     * A state to generate a world map.
     */
    class WorldGeneratorState: public state::State
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        WorldGeneratorState() = delete;

        /**
         * Constructs a new WorldGeneratorState object.
         *
         * @param t_id The unique identifier of the State.
         * @param t_context The holder of shared objects.
         */
        WorldGeneratorState(state::StateId t_id, std::shared_ptr<state::Context> t_context);

        WorldGeneratorState(const WorldGeneratorState& t_other) = delete;
        WorldGeneratorState(WorldGeneratorState&& t_other) noexcept = delete;
        WorldGeneratorState& operator=(const WorldGeneratorState& t_other) = delete;
        WorldGeneratorState& operator=(WorldGeneratorState&& t_other) noexcept = delete;

        ~WorldGeneratorState() noexcept override;

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
        // Init
        //-------------------------------------------------

        /**
         * Initializes the state.
         */
        void Init();
    };
}
