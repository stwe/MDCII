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

#include "MdciiResourcesManager.h"
#include "Log.h"
#include "MdciiUtils.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::file::MdciiResourcesManager::MdciiResourcesManager()
{
    Log::MDCII_LOG_DEBUG("[MdciiResourcesManager::MdciiResourcesManager()] Create MdciiResourcesManager.");

    LoadFiles();
}

mdcii::file::MdciiResourcesManager::~MdciiResourcesManager() noexcept
{
    Log::MDCII_LOG_DEBUG("[MdciiResourcesManager::~MdciiResourcesManager()] Destruct MdciiResourcesManager.");
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::file::MdciiResourcesManager::LoadFiles()
{
    Log::MDCII_LOG_DEBUG("[MdciiResourcesManager::LoadFiles()] Start loading files...");

    mapFiles = get_files_list(MAP_FILE_REL_PATH, MAP_FILE_EXTENSION);
    saveGameFiles = get_files_list(SAVE_GAME_FILE_REL_PATH, SAVE_GAME_FILE_EXTENSION);
    islandFiles = get_files_list(ISLAND_FILE_REL_PATH, ISLAND_FILE_EXTENSION);

    Log::MDCII_LOG_DEBUG("[MdciiResourcesManager::LoadFiles()] All files have been loaded successfully.");
}
