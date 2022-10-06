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

//-------------------------------------------------
// WorldGui
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * Forward declaration class World.
     */
    class World;

    /**
     * ImGui menus for the game.
     */
    class WorldGui
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        WorldGui() = delete;

        /**
         * Constructs a new WorldGui object.
         *
         * @param t_world The parent World object.
         */
        explicit WorldGui(World* t_world);

        WorldGui(const WorldGui& t_other) = delete;
        WorldGui(WorldGui&& t_other) noexcept = delete;
        WorldGui& operator=(const WorldGui& t_other) = delete;
        WorldGui& operator=(WorldGui&& t_other) noexcept = delete;

        ~WorldGui() noexcept;

        //-------------------------------------------------
        // GUIs
        //-------------------------------------------------

        /**
         * Buttons for rotating the world left and right.
         */
        void RotateGui() const;

        /**
         * Buttons to zoom the world in and out.
         */
        void ZoomGui() const;

        /**
         * Shows a menu with the action buttons.
         * These can be used to switch between Build, Status or Options.
         */
        void ShowActionsGui();

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The parent World object.
         */
        World* m_world{ nullptr };
    };
}
