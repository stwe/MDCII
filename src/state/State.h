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

namespace olc
{
    /**
     * @brief Forward declaration class PixelGameEngine.
     */
    class PixelGameEngine;
}

namespace mdcii::state
{
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
        // Ctors. / Dtor.
        //-------------------------------------------------

        State();

        State(const State& t_other) = delete;
        State(State&& t_other) noexcept = delete;
        State& operator=(const State& t_other) = delete;
        State& operator=(State&& t_other) noexcept = delete;

        virtual ~State() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        virtual void Init() {}
        virtual void Input(olc::PixelGameEngine* t_pge) {}
        virtual void Render(olc::PixelGameEngine* t_pge, float t_elapsedTime) {}

    protected:

    private:

    };
}
