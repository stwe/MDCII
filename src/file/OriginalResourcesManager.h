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
         * The loaded wav/mp3 files.
         */
        std::unordered_map<std::string, std::shared_ptr<SoundFile>> soundFiles;

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
        std::string m_paletteFilePath;

        /**
         * The paths to the stadtfld.bsh graphic files.
         */
        std::unordered_map<world::Zoom, std::string> m_stadtfldBshFilesPaths;

        /**
         * The paths to the bauhaus.bsh graphic files.
         */
        std::unordered_map<world::Zoom, std::string> m_bauhausBshFilesPaths;

        /**
         * The path to the haeuser.cod file.
         */
        std::string m_buildingsFilePath;

        /**
         * The col file to get palette values.
         */
        std::unique_ptr<PaletteFile> m_paletteFile;

        /**
         * The paths to the sound files.
         */
        std::vector<std::filesystem::path> m_soundFilesPaths;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Get paths from original game.
         */
        void GetPathsFromOriginal();

        /**
         * Load files from paths.
         */
        void LoadFiles();
    };
}
