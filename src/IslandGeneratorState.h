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
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * Forward declaration class IslandGenerator.
     */
    class IslandGenerator;
}

//-------------------------------------------------
// IslandGeneratorState
//-------------------------------------------------

namespace mdcii
{
    /**
     * A state to generate an island.
     */
    class IslandGeneratorState: public state::State
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        IslandGeneratorState() = delete;

        /**
         * Constructs a new IslandGeneratorState object.
         *
         * @param t_id The unique identifier of the State.
         * @param t_context The holder of shared objects.
         */
        IslandGeneratorState(state::StateId t_id, std::shared_ptr<state::Context> t_context);

        IslandGeneratorState(const IslandGeneratorState& t_other) = delete;
        IslandGeneratorState(IslandGeneratorState&& t_other) noexcept = delete;
        IslandGeneratorState& operator=(const IslandGeneratorState& t_other) = delete;
        IslandGeneratorState& operator=(IslandGeneratorState&& t_other) noexcept = delete;

        ~IslandGeneratorState() noexcept override;

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
         * The IslandGenerator object.
         */
        std::unique_ptr<world::IslandGenerator> m_islandGenerator;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes the state.
         */
        void Init();
    };
}
