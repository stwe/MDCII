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
// WorldState
//-------------------------------------------------

namespace mdcii
{
    class WorldState: public state::State
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        WorldState() = delete;

        /**
         * Constructs a new WorldState object.
         *
         * @param t_id The unique identifier of the State.
         * @param t_stateStack A pointer to the parent StateStack object.
         * @param t_context The holder of shared objects.
         */
        WorldState(Id t_id, state::StateStack* t_stateStack, std::shared_ptr<state::Context> t_context);

        WorldState(const WorldState& t_other) = delete;
        WorldState(WorldState&& t_other) noexcept = delete;
        WorldState& operator=(const WorldState& t_other) = delete;
        WorldState& operator=(WorldState&& t_other) noexcept = delete;

        ~WorldState() noexcept override;

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        void Input() override;
        void Update() override;
        void PreRender() override {}
        void Render() override;
        void RenderImGui() override;

    protected:

    private:
        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init();
    };
}
