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

#pragma once

#include <memory>
#include <filesystem>
#include "Buildings.h"
#include "world/Zoom.h"

namespace mdcii::resource
{
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * @brief Forward declaration class PaletteFile.
     */
    class PaletteFile;

    /**
     * @brief Forward declaration class BshFile.
     */
    class BshFile;

    //-------------------------------------------------
    // OriginalResourcesManager
    //-------------------------------------------------

    /**
     * @brief Managing original assets, including loading them from files.
     */
    class OriginalResourcesManager
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief A bauhaus bsh file for each zoom to get the Gpu texture Ids.
         *
         * In the History Edition there is only one Bauhaus.bsh.
         * The Nina has two other versions, suffixed by 6 and 8, which
         * correspond to images for small and medium resolutions, respectively.
         */
        std::unordered_map<world::Zoom, std::unique_ptr<BshFile>> bauhausBshFiles;

        /**
         * @brief The decrypted content of the haeuser.cod.
         */
        std::unique_ptr<const Buildings> buildings;

        /**
         * @brief The decrypted content of the figuren.cod.
         */
        std::unique_ptr<const Figures> figures;

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
         * @brief For convenience and better readability. Get a Building object by Id.
         *
         * @param t_id The given Id.
         *
         * @return The Building object.
         */
        [[nodiscard]] const Building& GetBuildingById(int t_id) const;

        /**
         * @brief For convenience and better readability. Get a Figure object by Id.
         *
         * @param t_figureId The Id of the Figure object to return.
         *
         * @return The Figure object.
         */
        [[nodiscard]] const Figure& GetFigureById(FigureId t_figureId) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The path to the haeuser.cod file.
         */
        std::filesystem::path m_buildingsPath;

        /**
         * @brief The path to the figuren.cod file.
         */
        std::filesystem::path m_figuresPath;

        /**
         * @brief The path to the palette file.
         */
        std::filesystem::path m_palettePath;

        /**
         * @brief The loaded palette file.
         */
        std::unique_ptr<PaletteFile> m_paletteFile;

        /**
         * @brief The paths to the bauhaus.bsh graphic files.
         */
        std::unordered_map<world::Zoom, std::filesystem::path> m_bauhausBshPaths;

        //-------------------------------------------------
        // Path to the original data
        //-------------------------------------------------

        /**
         * @brief Get all paths to the original data.
         */
        void GetPathsFromOriginal();

        /**
         * @brief Iterates over the file system and calls a function on a condition.
         *
         * @param t_fileName The name of a file.
         * @param t_doForFileName The function to call on the file.
         */
        static void IterateFilesystem(
            const std::string& t_fileName,
            const std::function<void(const std::filesystem::directory_entry&, const std::string&)>& t_doForFileName
        );

        /**
         * @brief Iterates over the file system and calls a function on a condition.
         *
         * @param t_fileNames Some filenames.
         * @param t_doForEachFileName The function to call on each file.
         */
        static void IterateFilesystem(
            const std::vector<std::string>& t_fileNames,
            const std::function<void(const std::filesystem::directory_entry&, const std::string&)>& t_doForEachFileName
        );

        /**
         * @brief Finds the path to the haeuser.cod file.
         */
        void FindBuildingsCodFilePath();

        /**
         * @brief Finds the path to the figuren.cod file.
         */
        void FindFiguresCodFilePath();

        /**
         * @brief Finds the paths to all bauhaus.bsh files.
         */
        void FindBauhausBshFilePaths();

        /**
         * @brief Finds the path to the stadtfld.col palette file.
         */
        void FindPaletteFilePath();

        //-------------------------------------------------
        // Load original data
        //-------------------------------------------------

        /**
         * @brief Load original data from files.
         */
        void LoadFiles();
    };
}
