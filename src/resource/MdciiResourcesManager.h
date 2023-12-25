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

#include "MdciiFile.h"

namespace mdcii::resource
{
    //-------------------------------------------------
    // MdciiResourcesManager
    //-------------------------------------------------

    /**
     * @brief Manages MDCII Json files.
     */
    class MdciiResourcesManager
    {
    public:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * @brief The file extension for map files.
         */
        const std::string MAP_FILE_EXTENSION{ ".map" };

        /**
         * @brief The relative path to the map files.
         */
        const std::string MAP_FILE_REL_PATH{ "map/" };

        /**
         * @brief The file extension for save-game files.
         */
        const std::string SAVE_GAME_FILE_EXTENSION{ ".sav" };

        /**
         * @brief The relative path to the save game files.
         */
        const std::string SAVE_GAME_FILE_REL_PATH{ "save/" };

        /**
         * @brief The file extension for island files.
         */
        const std::string ISLAND_FILE_EXTENSION{ ".isl" };

        /**
         * @brief The relative path to the island files.
         */
        const std::string ISLAND_FILE_REL_PATH{ "island/" };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief A list of map files.
         */
        std::vector<MdciiFile> mapFiles;

        /**
         * @brief A list of save-game files.
         */
        std::vector<MdciiFile> saveGameFiles;

        /**
         * @brief A list of island files.
         */
        std::vector<MdciiFile> islandFiles;

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
         * @brief Reads the paths to all MDCII files.
         */
        void ReadPaths();
    };
}
