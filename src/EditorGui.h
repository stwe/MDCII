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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#pragma once

#include <memory>
#include "event/Event.h"
#include "map/MapTile.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::map
{
    class Map;
}

//-------------------------------------------------
// EditorGui
//-------------------------------------------------

namespace mdcii
{
    /**
     * Different menus for the game editor.
     */
    class EditorGui
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        EditorGui() = delete;

        /**
         * Constructs a new EditorGui object.
         *
         * @param t_map The Map object to edit.
         */
        explicit EditorGui(std::shared_ptr<map::Map> t_map);

        EditorGui(const EditorGui& t_other) = delete;
        EditorGui(EditorGui&& t_other) noexcept = delete;
        EditorGui& operator=(const EditorGui& t_other) = delete;
        EditorGui& operator=(EditorGui&& t_other) noexcept = delete;

        ~EditorGui() noexcept;

        //-------------------------------------------------
        // GUIs
        //-------------------------------------------------

        /**
         * Buttons for rotating the map left and right.
         */
        void RotateMapGui() const;

        /**
         * Buttons to zoom in and out the map.
         */
        void ZoomMapGui() const;

        /**
         * Shows a menu with the action buttons.
         * These can be used to switch between Build, Status or Options.
         */
        void ShowActionsGui();

        /**
         * Shows a list of all workshops.
         * When a building is selected, an event (BauGfxSelectedEvent) is created.
         */
        void AllWorkshopsGui() const;

        /**
         * Shows a single selected workshop.
         *
         * @parm t_selectedBauGfx The selected building to build.
         */
        void WorkshopGui(event::SelectedBauGfx& t_selectedBauGfx) const;

        /**
         * Shows a mouse clicked MapTile object.
         *
         * @param t_mapTile The MapTile object to show.
         */
        void CurrentSelectedMapTileGui(const map::MapTile& t_mapTile) const;

        /**
         * Shows a menu to save the game, specifically the map.
         */
        void SaveGameGui();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Creates a toggle button.
         *
         * @param t_str An Id.
         * @param t_v Pointer to a bool.
         */
        static void ToggleButton(const char* t_str, bool* t_v);

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The Map object to edit.
         */
        std::shared_ptr<map::Map> m_map;

        /**
         * Used as a flag to print out an info text.
         */
        bool m_info{ false };
    };
}
