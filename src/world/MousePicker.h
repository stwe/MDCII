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
#include "vendor/olc/olcPixelGameEngine.h"

namespace mdcii
{
    class Game;
}

namespace mdcii::world
{
    class MousePicker
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MousePicker();

        ~MousePicker() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Update(const Game* t_game);
        void Render(Game* t_game) const;

    protected:

    private:
        /**
         * @brief The mouse in the world.
         */
        olc::vi2d m_mousePosition;

        /**
         * @brief The active cell.
         */
        olc::vi2d m_cell;

        /**
         * @brief The mouse offset into cell.
         */
        olc::vi2d m_cellOffset;

        /**
         * @brief
         */
        std::unique_ptr<olc::Renderable> m_rect;
    };
}
