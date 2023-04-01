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

#include <vector>
#include <string>

//-------------------------------------------------
// MdciiResourcesManager
//-------------------------------------------------

namespace mdcii::file
{
    class MdciiResourcesManager
    {
    public:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * The file extension for map files.
         */
        const std::string MAP_FILE_EXTENSION{ ".map" };

        /**
         * The relative path to the map files.
         */
        const std::string MAP_FILE_REL_PATH{ "map/" };

        /**
         * The file extension for save-game files.
         */
        const std::string SAVE_GAME_FILE_EXTENSION{ ".sav" };

        /**
         * The relative path to the save game files.
         */
        const std::string SAVE_GAME_FILE_REL_PATH{ "save/" };

        /**
         * The file extension for island files.
         */
        const std::string ISLAND_FILE_EXTENSION{ ".isl" };

        /**
         * The relative path to the island files.
         */
        const std::string ISLAND_FILE_REL_PATH{ "island/" };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * A list of map files.
         */
        std::vector<std::string> mapFiles;

        /**
         * A list of save-game files.
         */
        std::vector<std::string> saveGameFiles;

        /**
         * A list of island files.
         */
        std::vector<std::string> islandFiles;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MdciiResourcesManager();

        MdciiResourcesManager(const MdciiResourcesManager& t_other) = delete;
        MdciiResourcesManager(MdciiResourcesManager&& t_other) noexcept = delete;
        MdciiResourcesManager& operator=(const MdciiResourcesManager& t_other) = delete;
        MdciiResourcesManager& operator=(MdciiResourcesManager&& t_other) noexcept = delete;

        ~MdciiResourcesManager() noexcept;

    protected:

    private:
        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Load files.
         */
        void LoadFiles();
    };
}
