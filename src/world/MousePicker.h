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
#include "Zoom.h"
#include "vendor/olc/olcPixelGameEngine.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii
{
    /**
     * @brief Forward declaration class GameState.
     */
    class GameState;
}

namespace mdcii::world
{
    //-------------------------------------------------
    // MousePicker
    //-------------------------------------------------

    /**
     * @brief Represents a mouse picker to select tiles.
     */
    class MousePicker
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MousePicker() = delete;

        /**
         * @brief Constructs a new MousePicker object.
         *
         * @param t_game Pointer to the parent GameState.
         */
        explicit MousePicker(GameState* t_gameState);

        MousePicker(const MousePicker& t_other) = delete;
        MousePicker(MousePicker&& t_other) noexcept = delete;
        MousePicker& operator=(const MousePicker& t_other) = delete;
        MousePicker& operator=(MousePicker&& t_other) noexcept = delete;

        ~MousePicker() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Render();

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the parent GameState.
         */
        GameState* m_gameState{ nullptr };

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
         * @brief A rectangle for each zoom level (SGFX, MGFX, GFX).
         */
        std::array<std::unique_ptr<const olc::Renderable>, NR_OF_ZOOMS> m_rectSprites;

        /**
         * @brief Cheat images for each zoom level (SGFX, MGFX, GFX).
         */
        std::array<std::unique_ptr<const olc::Renderable>, NR_OF_ZOOMS> m_cheatSprites;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * @brief Updates the values based on the current mouse position.
         */
        void Update();

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * @brief Initializes the mouse picker.
         */
        void Init();
    };
}
