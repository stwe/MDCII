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
     * @brief Forward declaration class Game.
     */
    class Game;
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

        void Render(Game* t_game);

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        static olc::vi2d ToScreen(int t_x, int t_y, float t_startX, float t_startY, world::Zoom t_zoom);

        static int GetAtlasIndex(int t_gfx, int t_rows);

        static olc::vi2d GetOffset(int t_gfx, int t_rows);

        static int GetMapIndex(int t_x, int t_y, int t_width, int t_height, world::Rotation t_rotation);

    protected:

    private:

    };
}
