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

#include <filesystem>
#include "OriginalResourcesManager.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "MdciiException.h"
#include "map/Zoom.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::file::OriginalResourcesManager::OriginalResourcesManager()
{
    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::OriginalResourcesManager()] Create OriginalResourcesManager.");

    GetPathsFromOriginal();
    LoadFiles();
}

mdcii::file::OriginalResourcesManager::~OriginalResourcesManager() noexcept
{
    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::~OriginalResourcesManager()] Destruct OriginalResourcesManager.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const std::vector<std::unique_ptr<mdcii::file::BshTexture>>& mdcii::file::OriginalResourcesManager::GetStadtfldBshByZoom(const map::Zoom t_zoom) const
{
    return stadtfldBshFiles.at(t_zoom)->bshTextures;
}

const std::vector<std::unique_ptr<mdcii::file::BshTexture>>& mdcii::file::OriginalResourcesManager::GetBauhausBshByZoom(const map::Zoom t_zoom) const
{
    return bauhausBshFiles.at(t_zoom)->bshTextures;
}

const mdcii::data::Building& mdcii::file::OriginalResourcesManager::GetBuildingById(const int t_id) const
{
    MDCII_ASSERT(t_id >= 0, "[OriginalResourcesManager::GetBuildingById()] Invalid Id given.")

    return buildings->buildingsMap.at(t_id);
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::file::OriginalResourcesManager::GetPathsFromOriginal()
{
    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::GetPathsFromOriginal()] Start get paths from {} ...", Game::ORIGINAL_RESOURCES_FULL_PATH);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(Game::ORIGINAL_RESOURCES_FULL_PATH, std::filesystem::directory_options::follow_directory_symlink))
    {
        if (is_directory(entry))
        {
            for (const auto& f : std::filesystem::directory_iterator(entry))
            {
                if (is_regular_file(f.status()))
                {
                    auto extension{ ToLowerCase(f.path().extension().string()) };

                    // BSH files
                    if (extension == ".bsh")
                    {
                        const auto zoomOptional{ magic_enum::enum_cast<map::Zoom>(ToUpperCase(entry.path().filename().string())) };
                        if (zoomOptional.has_value())
                        {
                            if (ToUpperCase(f.path().filename().string()) == "STADTFLD.BSH")
                            {
                                m_stadtfldBshFilesPaths.emplace(zoomOptional.value(), f.path().string());
                            }
                        }
                        else
                        {
                            if (ToUpperCase(entry.path().filename().string()) == "TOOLGFX")
                            {
                                // Nina & Hist Ed.
                                if (ToUpperCase(f.path().filename().string()) == "BAUHAUS.BSH")
                                {
                                    m_bauhausBshFilesPaths.emplace(map::Zoom::GFX, f.path().string());
                                }

                                // Nina only
                                if (ToUpperCase(f.path().filename().string()) == "BAUHAUS8.BSH")
                                {
                                    m_bauhausBshFilesPaths.emplace(map::Zoom::MGFX, f.path().string());
                                }

                                if (ToUpperCase(f.path().filename().string()) == "BAUHAUS6.BSH")
                                {
                                    m_bauhausBshFilesPaths.emplace(map::Zoom::SGFX, f.path().string());
                                }
                            }
                        }
                    }

                    // COL file
                    if (extension == ".col")
                    {
                        if (ToUpperCase(f.path().filename().string()) == "STADTFLD.COL")
                        {
                            m_paletteFilePath = f.path().string();
                        }
                    }
                }
            }
        }
        else if (is_regular_file(entry))
        {
            auto extension{ ToLowerCase(entry.path().extension().string()) };
            if (extension == ".cod")
            {
                if (ToUpperCase(entry.path().filename().string()) == "HAEUSER.COD")
                {
                    m_buildingsFilePath = entry.path().string();
                }
            }
        }
    }

    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::GetPathsFromOriginal()] All paths were found successfully.");
}

void mdcii::file::OriginalResourcesManager::LoadFiles()
{
    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::LoadFiles()] Start loading files...");

    // palette
    if (m_paletteFilePath.empty())
    {
        throw MDCII_EXCEPTION("[OriginalResourcesManager::LoadFiles()] Error while reading palette file path.");
    }

    m_paletteFile = std::make_unique<PaletteFile>(m_paletteFilePath);
    m_paletteFile->ReadDataFromChunks();

    // stadtfld bsh graphics
    if (m_stadtfldBshFilesPaths.size() != magic_enum::enum_count<map::Zoom>())
    {
        throw MDCII_EXCEPTION("[OriginalResourcesManager::LoadFiles()] Error while reading stadtfld bsh files paths.");
    }

    for (const auto& [zoom, stadtfldBshFilePath] : m_stadtfldBshFilesPaths)
    {
        if (stadtfldBshFilePath.empty())
        {
            throw MDCII_EXCEPTION("[OriginalResourcesManager::LoadFiles()] Error while reading stadtfld bsh file path.");
        }

        auto stadtfldBshFile{ std::make_unique<BshFile>(stadtfldBshFilePath, m_paletteFile->palette) };
        stadtfldBshFile->ReadDataFromChunks();

        stadtfldBshFiles.emplace(zoom, std::move(stadtfldBshFile));
    }

    // bauhaus bsh graphics
    if (m_bauhausBshFilesPaths.empty())
    {
        throw MDCII_EXCEPTION("[OriginalResourcesManager::LoadFiles()] Error while reading bauhaus bsh files paths.");
    }

    for (const auto& [zoom, bauhausBshFilePath] : m_bauhausBshFilesPaths)
    {
        if (bauhausBshFilePath.empty())
        {
            throw MDCII_EXCEPTION("[OriginalResourcesManager::LoadFiles()] Error while reading bauhaus bsh file path.");
        }

        auto bauhausBshFile{ std::make_unique<BshFile>(bauhausBshFilePath, m_paletteFile->palette) };
        bauhausBshFile->ReadDataFromChunks();

        bauhausBshFiles.emplace(zoom, std::move(bauhausBshFile));
    }

    // decrypted buildings
    if (m_buildingsFilePath.empty())
    {
        throw MDCII_EXCEPTION("[OriginalResourcesManager::LoadFiles()] Error while reading buildings file path.");
    }

    buildings = std::make_unique<data::Buildings>(m_buildingsFilePath);

    Log::MDCII_LOG_DEBUG("[OriginalResourcesManager::LoadFiles()] All files have been loaded successfully.");
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

std::string mdcii::file::OriginalResourcesManager::ToLowerCase(const std::string& t_string) const
{
    auto newString{ t_string };
    for (auto& c : newString)
    {
        c = std::tolower(c);
    }

    return newString;
}

std::string mdcii::file::OriginalResourcesManager::ToUpperCase(const std::string& t_string) const
{
    auto newString{ t_string };
    for (auto& c : newString)
    {
        c = std::toupper(c);
    }

    return newString;
}
