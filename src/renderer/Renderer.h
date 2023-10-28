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
    /**
     * @brief Forward declaration enum class Zoom.
     */
    enum class Zoom;

    /**
     * @brief Forward declaration enum class Rotation.
     */
    enum class Rotation;
}

namespace mdcii::renderer
{
    //-------------------------------------------------
    // Renderer
    //-------------------------------------------------

    /**
     * @brief Represents a Renderer.
     */
    class Renderer
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Renderer();

        Renderer(const Renderer& t_other) = delete;
        Renderer(Renderer&& t_other) noexcept = delete;
        Renderer& operator=(const Renderer& t_other) = delete;
        Renderer& operator=(Renderer&& t_other) noexcept = delete;

        ~Renderer() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * @brief Render the State.
         *
         * @param t_game Pointer to the Game State.
         */
        void Render(GameState* t_gameState);

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * @brief Projects a position into an isometric position on the screen.
         *
         * @param t_x The x position.
         * @param t_y The y position.
         * @param t_zoom The zoom to get the tile sizes.
         * @param t_rotation The position is previously rotated by the specified value.
         *
         * @return The isometric coordinates on the screen.
         */
        static olc::vi2d ToScreen(
            int t_x, int t_y,
            float t_startX, float t_startY,
            world::Zoom t_zoom, world::Rotation t_rotation
        );

        /**
         * @brief Returns the number of the Texture Atlas on which the given gfx is located.
         *
         * @param t_gfx The sprite index.
         * @param t_rows The number of rows.
         *
         * @return The number of the Texture Atlas.
         */
        static int GetAtlasIndex(int t_gfx, int t_rows);

        /**
         * @brief Determines where a sprite is located on an atlas image/texture.
         *
         * @param t_gfx The sprite index.
         * @param t_rows The number of rows.
         *
         * @return The offset coordinates.
         */
        static olc::vi2d GetOffset(int t_gfx, int t_rows);

        /**
         * @brief 2D/1D - mapping of a position.
         *
         * @param t_x The 2D x position.
         * @param t_y The 2D y position.
         * @param t_width The width of the 2D array.
         * @param t_height The height of the 2D array.
         * @param t_rotation The given position is previously rotated by the specified value.
         *
         * @return The 1D index.
         */
        static int GetMapIndex(int t_x, int t_y, int t_width, int t_height, world::Rotation t_rotation);

    protected:

    private:

    };
}
