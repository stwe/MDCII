// This file is part of the MDCII project.
//
// Copyright (c) 2024. stwe <https://github.com/stwe/MDCII>
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

#include "vendor/olc/olcPixelGameEngine.h"

namespace mdcii::world
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * Forward declaration class World.
     */
    class World;

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
        // Member
        //-------------------------------------------------

        /**
         * @brief The current world position under the mouse.
         */
        olc::vi2d selected;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MousePicker() = delete;

        /**
         * @brief Constructs a new MousePicker object.
         *
         * @param t_world Pointer to the parent World object.
         * @param t_renderCursor The tile under the mouse is highlighted.
         */
        explicit MousePicker(World* t_world, bool t_renderCursor = true);

        MousePicker(const MousePicker& t_other) = delete;
        MousePicker(MousePicker&& t_other) noexcept = delete;
        MousePicker& operator=(const MousePicker& t_other) = delete;
        MousePicker& operator=(MousePicker&& t_other) noexcept = delete;

        ~MousePicker() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void OnUserUpdate();

        //-------------------------------------------------
        // ImGui
        //-------------------------------------------------

        void RenderImGui();

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Specifies whether the mouse is calculated for terrain.
         */
        bool m_calcForTerrain{ true };

        /**
         * @brief Pointer to the parent World object.
         */
        World* m_world{ nullptr };

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
         * @brief The tile under the mouse is highlighted.
         */
        bool m_renderCursor{ true };

        /**
         * @brief The normalized tile under the mouse is highlighted.
         *
         * To do this, the increase for the terrain is taken out again.
         */
        bool m_renderNormalizedCursor{ false };

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * @brief Updated the mouse position.
         *
         * This method calculates the mouse position, the cell the mouse is in,
         * and the mouse position offset inside the cell, given the game state's zoom level.
         */
        void UpdateMousePosition();

        /**
         * @brief Calculates the selected cell.
         *
         * @return The x and y coordinates of the selected cell.
         */
        [[nodiscard]] olc::vi2d CalculateSelected() const;

        /**
         * @brief Calculates the move offset due to cheat color.
         *
         * @param t_cheatColor The current cheat color.
         *
         * @return The x and y coordinates of movement.
         */
        [[nodiscard]] olc::vi2d CalculateSelectedCheatColorOffset(const olc::Pixel& t_cheatColor) const;
    };
}
