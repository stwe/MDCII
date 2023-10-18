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

#include "OriginalResourcesManager.h"
#include "Game.h"
#include "MdciiException.h"
#include "MdciiUtils.h"
#include "MdciiAssert.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::resource::OriginalResourcesManager::OriginalResourcesManager()
{
    MDCII_LOG_DEBUG("[OriginalResourcesManager::OriginalResourcesManager()] Create OriginalResourcesManager.");

    GetPathsFromOriginal();
    LoadFiles();
}

mdcii::resource::OriginalResourcesManager::~OriginalResourcesManager() noexcept
{
    MDCII_LOG_DEBUG("[OriginalResourcesManager::~OriginalResourcesManager()] Destruct OriginalResourcesManager.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const mdcii::resource::Building& mdcii::resource::OriginalResourcesManager::GetBuildingById(int t_id) const
{
    MDCII_ASSERT(t_id >= 0, "[OriginalResourcesManager::GetBuildingById()] Invalid Id given.")

    return buildings->buildingsMap.at(t_id);
}

//-------------------------------------------------
// Path to the original data
//-------------------------------------------------

void mdcii::resource::OriginalResourcesManager::GetPathsFromOriginal()
{
    MDCII_LOG_DEBUG("[OriginalResourcesManager::GetPathsFromOriginal()] Start get paths from {} ...", Game::ORIGINAL_RESOURCES_FULL_PATH);

    FindBuildingsCodFilePath();

    MDCII_LOG_DEBUG("[OriginalResourcesManager::GetPathsFromOriginal()] All paths were found successfully.");
}

void mdcii::resource::OriginalResourcesManager::IterateFilesystem(const std::string& t_fileName, const std::function<void(const std::filesystem::directory_entry&, const std::string&)>& t_doForFileName)
{
    for (const auto& entry : std::filesystem::recursive_directory_iterator(Game::ORIGINAL_RESOURCES_FULL_PATH, std::filesystem::directory_options::follow_directory_symlink))
    {
        if (is_directory(entry))
        {
            for (const auto& f : std::filesystem::directory_iterator(entry))
            {
                if (is_regular_file(f.status()))
                {
                    t_doForFileName(f, t_fileName);
                }
            }
        }
        else if (is_regular_file(entry))
        {
            t_doForFileName(entry, t_fileName);
        }
    }
}

void mdcii::resource::OriginalResourcesManager::FindBuildingsCodFilePath()
{
    IterateFilesystem("haeuser.cod", [this](const std::filesystem::directory_entry& t_entry, const std::string& t_fileName) {
        if (to_upper_case(t_entry.path().filename().string()) == to_upper_case(t_fileName))
        {
            m_buildingsPath = t_entry.path();
        }
    });

    if (m_buildingsPath.empty())
    {
        throw MDCII_EXCEPTION("[OriginalResourcesManager::FindBuildingsCodFilePath()] Error while reading buildings file path.");
    }

    MDCII_LOG_DEBUG("[OriginalResourcesManager::FindBuildingsCodFilePath()] The path to the haeuser.cod file was found successfully.");
}

//-------------------------------------------------
// Load original data
//-------------------------------------------------

void mdcii::resource::OriginalResourcesManager::LoadFiles()
{
    MDCII_LOG_DEBUG("[OriginalResourcesManager::LoadFiles()] Start loading files...");

    buildings = std::make_unique<Buildings>(m_buildingsPath);

    MDCII_LOG_DEBUG("[OriginalResourcesManager::LoadFiles()] All files have been loaded successfully.");
}