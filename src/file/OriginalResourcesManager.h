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

#include <filesystem>
#include <unordered_map>
#include <functional>
#include "data/Buildings.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * Forward declaration enum class Zoom.
     */
    enum class Zoom;
}

//-------------------------------------------------
// OriginalResourcesManager
//-------------------------------------------------

namespace mdcii::file
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * Forward declaration struct BshTexture.
     */
    struct BshTexture;

    /**
     * Forward declaration class BshFile.
     */
    class BshFile;

    /**
     * Forward declaration class PaletteFile.
     */
    class PaletteFile;

    /**
     * Forward declaration class SoundFile.
     */
    class SoundFile;

    //-------------------------------------------------
    // OriginalResourcesManager
    //-------------------------------------------------

    /**
     * Reads paths from the original game and loads the required resources.
     */
    class OriginalResourcesManager
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * A bsh file for each zoom to get the Gpu texture Ids.
         */
        std::unordered_map<world::Zoom, std::unique_ptr<BshFile>> stadtfldBshFiles;

        /**
         * A bauhaus bsh file for each zoom to get the Gpu texture Ids.
         * In the History Edition there is only one Bauhaus.bsh.
         * The Nina has two other versions, suffixed by 6 and 8, which
         * correspond to images for small and medium resolutions, respectively.
         */
        std::unordered_map<world::Zoom, std::unique_ptr<BshFile>> bauhausBshFiles;

        /**
         * The decrypted content of the haeuser.cod.
         */
        std::unique_ptr<data::Buildings> buildings;

        /**
         * The loaded mp3 files.
         */
        std::unordered_map<std::string, std::shared_ptr<SoundFile>> mp3Files;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        OriginalResourcesManager();

        OriginalResourcesManager(const OriginalResourcesManager& t_other) = delete;
        OriginalResourcesManager(OriginalResourcesManager&& t_other) noexcept = delete;
        OriginalResourcesManager& operator=(const OriginalResourcesManager& t_other) = delete;
        OriginalResourcesManager& operator=(OriginalResourcesManager&& t_other) noexcept = delete;

        ~OriginalResourcesManager() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * For convenience and better readability. Get a list of BshTexture objects of a given zoom.
         *
         * @param t_zoom The zoom.
         *
         * @return A list with BshTexture objects.
         */
        [[nodiscard]] const std::vector<std::unique_ptr<BshTexture>>& GetStadtfldBshByZoom(world::Zoom t_zoom) const;

        /**
         * For convenience and better readability. Get a list of BshTexture objects of a given zoom.
         *
         * @param t_zoom The zoom.
         *
         * @return A list with BshTexture objects.
         */
        [[nodiscard]] const std::vector<std::unique_ptr<BshTexture>>& GetBauhausBshByZoom(world::Zoom t_zoom) const;

        /**
         * For convenience and better readability. Get a Building object by Id.
         *
         * @param t_id The given Id.
         *
         * @return The Building object.
         */
        [[nodiscard]] const data::Building& GetBuildingById(int t_id) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The path to the stdfld.col palette file.
         */
        std::filesystem::path m_palettePath;

        /**
         * The paths to the stadtfld.bsh graphic files.
         */
        std::unordered_map<world::Zoom, std::filesystem::path> m_stadtfldBshPaths;

        /**
         * The paths to the bauhaus.bsh graphic files.
         */
        std::unordered_map<world::Zoom, std::filesystem::path> m_bauhausBshPaths;

        /**
         * The path to the haeuser.cod file.
         */
        std::filesystem::path m_buildingsPath;

        /**
         * The paths to the mp3 files.
         */
        std::vector<std::filesystem::path> m_mp3Paths;

        /**
         * The loaded palette file.
         */
        std::unique_ptr<PaletteFile> m_paletteFile;

        //-------------------------------------------------
        // Path to the original data
        //-------------------------------------------------

        /**
         * Get all paths to the original data.
         */
        void GetPathsFromOriginal();

        /**
         * Iterates over the file system and calls a function on a condition.
         *
         * @param t_fileName The name of a file.
         * @param t_doForFileName The function to call on the file.
         */
        static void IterateFilesystem(
            const std::string& t_fileName,
            const std::function<void(const std::filesystem::directory_entry&, const std::string&)>& t_doForFileName
        );

        /**
         * Iterates over the file system and calls a function on a condition.
         *
         * @param t_fileNames Some filenames.
         * @param t_doForEachFileName The function to call on each file.
         */
        static void IterateFilesystem(
            const std::vector<std::string>& t_fileNames,
            const std::function<void(const std::filesystem::directory_entry&, const std::string&)>& t_doForEachFileName
        );

        /**
         * Finds the paths to all stadtfld.bsh files.
         */
        void FindStadtfldBshFilePaths();

        /**
         * Finds the paths to all bauhaus.bsh files.
         */
        void FindBauhausBshFilePaths();

        /**
         * Finds the path to the stadtfld.col palette file.
         */
        void FindPaletteFilePath();

        /**
         * Finds the paths to all mp3 files.
         */
        void FindMp3FilePaths();

        /**
         * Finds the path to the haeuser.cod file.
         */
        void FindBuildingsCodFilePath();

        //-------------------------------------------------
        // Original version
        //-------------------------------------------------

        /**
         * Sets the type/version of the original game (History Ed. or NINA).
         */
        void SetGameType() const;

        //-------------------------------------------------
        // Load original data
        //-------------------------------------------------

        /**
         * Load original data from files.
         */
        void LoadFiles();
    };
}
