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

#include "OriginalResourcesManager.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "MdciiException.h"
#include "BshFile.h"
#include "SoundFile.h"
#include "world/Zoom.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::file::OriginalResourcesManager::OriginalResourcesManager()
{
    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::OriginalResourcesManager()] Create OriginalResourcesManager.");

    GetPathsFromOriginal();
    SetGameType();
    LoadFiles();
}

mdcii::file::OriginalResourcesManager::~OriginalResourcesManager() noexcept
{
    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::~OriginalResourcesManager()] Destruct OriginalResourcesManager.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const std::vector<std::unique_ptr<mdcii::file::BshTexture>>& mdcii::file::OriginalResourcesManager::GetStadtfldBshByZoom(const world::Zoom t_zoom) const
{
    return stadtfldBshFiles.at(t_zoom)->bshTextures;
}

const std::vector<std::unique_ptr<mdcii::file::BshTexture>>& mdcii::file::OriginalResourcesManager::GetBauhausBshByZoom(const world::Zoom t_zoom) const
{
    return bauhausBshFiles.at(t_zoom)->bshTextures;
}

const mdcii::data::Building& mdcii::file::OriginalResourcesManager::GetBuildingById(const int t_id) const
{
    MDCII_ASSERT(t_id >= 0, "[OriginalResourcesManager::GetBuildingById()] Invalid Id given.")

    return buildings->buildingsMap.at(t_id);
}

//-------------------------------------------------
// Path to the original data
//-------------------------------------------------

void mdcii::file::OriginalResourcesManager::GetPathsFromOriginal()
{
    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::GetPathsFromOriginal()] Start get paths from {} ...", Game::ORIGINAL_RESOURCES_FULL_PATH);

    FindStadtfldBshFilePaths();
    FindBauhausBshFilePaths();
    FindPaletteFilePath();
    FindMp3FilePaths();
    FindBuildingsCodFilePath();

    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::GetPathsFromOriginal()] All paths were found successfully.");
}

void mdcii::file::OriginalResourcesManager::IterateFilesystem(
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

void mdcii::file::OriginalResourcesManager::IterateFilesystem(
    const std::vector<std::string>& t_fileNames,
    const std::function<void(const std::filesystem::directory_entry&, const std::string&)>& t_doForEachFileName
)
{
    for (const auto& fileName : t_fileNames)
    {
        IterateFilesystem(fileName, t_doForEachFileName);
    }
}

void mdcii::file::OriginalResourcesManager::FindStadtfldBshFilePaths()
{
    IterateFilesystem("stadtfld.bsh", [this](const std::filesystem::directory_entry& t_entry, const std::string& t_fileName) {
        if (to_upper_case(t_entry.path().filename().string()) == to_upper_case(t_fileName))
        {
            if (const auto zoomOptional{ magic_enum::enum_cast<world::Zoom>(to_upper_case(t_entry.path().parent_path().filename().string())) }; zoomOptional.has_value())
            {
                m_stadtfldBshPaths.try_emplace(zoomOptional.value(), t_entry.path());
            }
        }
    });

    if (m_stadtfldBshPaths.size() != magic_enum::enum_count<world::Zoom>())
    {
        throw MDCII_EXCEPTION("[OriginalResourcesManager::FindStadtfldBshFilePaths()] Error while reading stadtfld bsh files paths.");
    }

    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::FindStadtfldBshFilePaths()] All paths to the stadtfld.bsh files were found successfully.");
}

void mdcii::file::OriginalResourcesManager::FindBauhausBshFilePaths()
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

    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::FindBauhausBshFilePaths()] All paths to the bauhaus.bsh files were found successfully.");
}

void mdcii::file::OriginalResourcesManager::FindPaletteFilePath()
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

    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::FindPaletteFilePath()] The path to the stadtfld.col file was found successfully.");
}

void mdcii::file::OriginalResourcesManager::FindMp3FilePaths()
{
    IterateFilesystem(".mp3", [this](const std::filesystem::directory_entry& t_entry, const std::string& t_fileName) {
        auto extension{ t_entry.path().extension().string() };
        if (to_upper_case(extension) == to_upper_case(t_fileName))
        {
            m_mp3Paths.emplace_back(t_entry.path());
        }
    });

    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::FindMp3FilePaths()] {} mp3 files were found.", m_mp3Paths.size());
}

void mdcii::file::OriginalResourcesManager::FindBuildingsCodFilePath()
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

    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::FindBuildingsCodFilePath()] The path to the haeuser.cod file was found successfully.");
}

//-------------------------------------------------
// Original version
//-------------------------------------------------

void mdcii::file::OriginalResourcesManager::SetGameType() const
{
    m_bauhausBshPaths.size() == 1 ? Game::game_type = Game::GameType::HISTORY : Game::game_type = Game::GameType::NINA;

    Log::MDCII_LOG_INFO("The original data comes from the following game: {}", magic_enum::enum_name(Game::game_type));
}

//-------------------------------------------------
// Load original data
//-------------------------------------------------

void mdcii::file::OriginalResourcesManager::LoadFiles()
{
    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::LoadFiles()] Start loading files...");

    MDCII_ASSERT(Game::game_type != Game::GameType::UNKNOWN, "[OriginalResourcesManager::LoadFiles()] Invalid game type.")

    // load palette
    m_paletteFile = std::make_unique<PaletteFile>(m_palettePath);
    m_paletteFile->ReadDataFromChunks();

    // load stadtfld.bsh graphics
    for (const auto& [zoom, stadtfldBshFilePath] : m_stadtfldBshPaths)
    {
        if (stadtfldBshFilePath.empty())
        {
            throw MDCII_EXCEPTION("[OriginalResourcesManager::LoadFiles()] Error while reading stadtfld.bsh file path.");
        }

        auto stadtfldBshFile{ std::make_unique<BshFile>(stadtfldBshFilePath, m_paletteFile->palette) };
        stadtfldBshFile->ReadDataFromChunks();

        stadtfldBshFiles.try_emplace(zoom, std::move(stadtfldBshFile));
    }

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

    // load buildings
    buildings = std::make_unique<data::Buildings>(m_buildingsPath);

    // load mp3 files from the History Ed.
    if (!m_mp3Paths.empty() && Game::game_type == Game::GameType::HISTORY)
    {
        for (const auto& soundFilePath : m_mp3Paths)
        {
            mp3Files.try_emplace(soundFilePath.filename().string(), std::make_unique<SoundFile>(soundFilePath));
        }
    }

    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::LoadFiles()] All files have been loaded successfully.");
}
