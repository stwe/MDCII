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

#include "OriginalResourcesManager.h"
#include "Game.h"
#include "MdciiException.h"
#include "MdciiUtils.h"
#include "MdciiAssert.h"
#include "PaletteFile.h"
#include "BshFile.h"

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

const mdcii::resource::Building& mdcii::resource::OriginalResourcesManager::GetBuildingById(const int t_id) const
{
    MDCII_ASSERT(t_id >= 0 && buildings->buildingsMap.count(t_id), "[OriginalResourcesManager::GetBuildingById()] Invalid Id given.")

    return buildings->buildingsMap.at(t_id);
}

const mdcii::resource::Figure& mdcii::resource::OriginalResourcesManager::GetFigureById(const FigureId t_figureId) const
{
    MDCII_ASSERT(t_figureId != FigureId::UNSET, "[OriginalResourcesManager::GetFigureById()] Invalid Id given.")

    return figures->figuresMap.at(t_figureId);
}

//-------------------------------------------------
// Path to the original data
//-------------------------------------------------

void mdcii::resource::OriginalResourcesManager::GetPathsFromOriginal()
{
    MDCII_LOG_DEBUG("[OriginalResourcesManager::GetPathsFromOriginal()] Start get paths from {} ...", Game::ORIGINAL_RESOURCES_FULL_PATH);

    FindBauhausBshFilePaths();
    FindPaletteFilePath();
    FindBuildingsCodFilePath();
    FindFiguresCodFilePath();

    MDCII_LOG_DEBUG("[OriginalResourcesManager::GetPathsFromOriginal()] All paths were found successfully.");
}

void mdcii::resource::OriginalResourcesManager::IterateFilesystem(
    const std::string& t_fileName,
    const std::function<void(const std::filesystem::directory_entry&, const std::string&)>& t_doForFileName
)
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

void mdcii::resource::OriginalResourcesManager::IterateFilesystem(
    const std::vector<std::string>& t_fileNames,
    const std::function<void(const std::filesystem::directory_entry&, const std::string&)>& t_doForEachFileName
)
{
    for (const auto& fileName : t_fileNames)
    {
        IterateFilesystem(fileName, t_doForEachFileName);
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

void mdcii::resource::OriginalResourcesManager::FindFiguresCodFilePath()
{
    IterateFilesystem("figuren.cod", [this](const std::filesystem::directory_entry& t_entry, const std::string& t_fileName) {
        if (to_upper_case(t_entry.path().filename().string()) == to_upper_case(t_fileName))
        {
            m_figuresPath = t_entry.path();
        }
    });

    if (m_figuresPath.empty())
    {
        throw MDCII_EXCEPTION("[OriginalResourcesManager::FindFiguresCodFilePath()] Error while reading figures file path.");
    }

    MDCII_LOG_DEBUG("[OriginalResourcesManager::FindFiguresCodFilePath()] The path to the figuren.cod file was found successfully.");
}

void mdcii::resource::OriginalResourcesManager::FindBauhausBshFilePaths()
{
    IterateFilesystem({ "bauhaus.bsh", "bauhaus8.bsh", "bauhaus6.bsh " }, [this](const std::filesystem::directory_entry& t_entry, const std::string& t_fileName) {
        // Nina & History Ed.
        if (to_upper_case(t_entry.path().filename().string()) == "BAUHAUS.BSH")
        {
            m_bauhausBshPaths.try_emplace(world::Zoom::GFX, t_entry.path());
        }

        // Nina only
        if (to_upper_case(t_entry.path().filename().string()) == "BAUHAUS8.BSH")
        {
            m_bauhausBshPaths.try_emplace(world::Zoom::MGFX, t_entry.path());
        }

        if (to_upper_case(t_entry.path().filename().string()) == "BAUHAUS6.BSH")
        {
            m_bauhausBshPaths.try_emplace(world::Zoom::SGFX, t_entry.path());
        }
    });

    if (m_bauhausBshPaths.empty())
    {
        throw MDCII_EXCEPTION("[OriginalResourcesManager::FindBauhausBshFilePaths()] Error while reading bauhaus bsh files paths.");
    }

    MDCII_LOG_DEBUG("[OriginalResourcesManager::FindBauhausBshFilePaths()] All paths to the bauhaus.bsh files were found successfully.");
}

void mdcii::resource::OriginalResourcesManager::FindPaletteFilePath()
{
    IterateFilesystem("stadtfld.col", [this](const std::filesystem::directory_entry& t_entry, const std::string& t_fileName) {
        if (to_upper_case(t_entry.path().filename().string()) == to_upper_case(t_fileName))
        {
            m_palettePath = t_entry.path();
        }
    });

    if (m_palettePath.empty())
    {
        throw MDCII_EXCEPTION("[OriginalResourcesManager::FindPaletteFilePath()] Error while reading palette file path.");
    }

    MDCII_LOG_DEBUG("[OriginalResourcesManager::FindPaletteFilePath()] The path to the stadtfld.col file was found successfully.");
}

//-------------------------------------------------
// Load original data
//-------------------------------------------------

void mdcii::resource::OriginalResourcesManager::LoadFiles()
{
    MDCII_LOG_DEBUG("[OriginalResourcesManager::LoadFiles()] Start loading files ...");

    m_paletteFile = std::make_unique<PaletteFile>(m_palettePath);
    m_paletteFile->ReadDataFromChunks();

    // load bauhaus.bsh graphics
    for (const auto& [zoom, bauhausBshFilePath] : m_bauhausBshPaths)
    {
        if (bauhausBshFilePath.empty())
        {
            throw MDCII_EXCEPTION("[OriginalResourcesManager::LoadFiles()] Error while reading bauhaus.bsh file path.");
        }

        auto bauhausBshFile{ std::make_unique<BshFile>(bauhausBshFilePath, m_paletteFile->palette) };
        bauhausBshFile->ReadDataFromChunks();

        bauhausBshFiles.try_emplace(zoom, std::move(bauhausBshFile));
    }

    buildings = std::make_unique<const Buildings>(m_buildingsPath);
    figures = std::make_unique<const Figures>(m_figuresPath);

    MDCII_LOG_DEBUG("[OriginalResourcesManager::LoadFiles()] All files have been loaded successfully.");
}
