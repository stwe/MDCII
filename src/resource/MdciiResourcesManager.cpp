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

#include "MdciiResourcesManager.h"
#include "Log.h"
#include "MdciiUtils.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::resource::MdciiResourcesManager::MdciiResourcesManager()
{
    MDCII_LOG_DEBUG("[MdciiResourcesManager::MdciiResourcesManager()] Create MdciiResourcesManager.");

    ReadPaths();
}

mdcii::resource::MdciiResourcesManager::~MdciiResourcesManager() noexcept
{
    MDCII_LOG_DEBUG("[MdciiResourcesManager::~MdciiResourcesManager()] Destruct MdciiResourcesManager.");
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::resource::MdciiResourcesManager::ReadPaths()
{
    MDCII_LOG_DEBUG("[MdciiResourcesManager::ReadPaths()] Start reading file paths ...");

    mapFiles = get_files_list(MAP_FILE_REL_PATH, MAP_FILE_EXTENSION);
    saveGameFiles = get_files_list(SAVE_GAME_FILE_REL_PATH, SAVE_GAME_FILE_EXTENSION);
    islandFiles = get_files_list(ISLAND_FILE_REL_PATH, ISLAND_FILE_EXTENSION);

    MDCII_LOG_DEBUG("[MdciiResourcesManager::ReadPaths()] All file paths have been read successfully.");
}
