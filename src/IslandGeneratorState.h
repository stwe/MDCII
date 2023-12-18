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

#include <memory>
#include "state/State.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * @brief Forward declaration class IslandGenerator.
     */
    class IslandGenerator;
}

namespace mdcii
{
    //-------------------------------------------------
    // IslandGeneratorState
    //-------------------------------------------------

    /**
     * @brief Represents the island generator.
     */
    class IslandGeneratorState : public state::State
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        IslandGeneratorState() = delete;

        /**
         * @brief Constructs a new IslandGeneratorState object.
         *
         * @param t_game Pointer to the parent Game.
         */
        explicit IslandGeneratorState(Game* t_game);

        IslandGeneratorState(const IslandGeneratorState& t_other) = delete;
        IslandGeneratorState(IslandGeneratorState&& t_other) noexcept = delete;
        IslandGeneratorState& operator=(const IslandGeneratorState& t_other) = delete;
        IslandGeneratorState& operator=(IslandGeneratorState&& t_other) noexcept = delete;

        ~IslandGeneratorState() noexcept override;

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        [[nodiscard]] bool OnUserCreate() override;
        [[nodiscard]] bool OnUserUpdate(float t_elapsedTime) override;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The IslandGenerator object.
         */
        std::unique_ptr<world::IslandGenerator> m_islandGenerator;
    };
}
