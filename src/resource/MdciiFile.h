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

#include "vendor/nlohmann/json.hpp"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * @brief Forward declaration enum class ClimateZone.
     */
    enum class ClimateZone;

    /**
     * @brief Forward declaration struct Tile.
     */
    struct Tile;
}

namespace mdcii::resource
{
    //-------------------------------------------------
    // Mdcii file type
    //-------------------------------------------------

    enum class MdciiFileType
    {
        MAP, ISLAND
    };

    //-------------------------------------------------
    // MdciiFile
    //-------------------------------------------------

    /**
     * @brief A class to load MDCII Json files.
     */
    class MdciiFile
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MdciiFile() = delete;

        /**
         * @brief Constructs a new MdciiFile object.
         *
         * @param t_fileName The name of the file.
         */
        explicit MdciiFile(std::string t_fileName);

        MdciiFile(const MdciiFile& t_other) = delete;
        MdciiFile(MdciiFile&& t_other) noexcept = default;
        MdciiFile& operator=(const MdciiFile& t_other) = delete;
        MdciiFile& operator=(MdciiFile&& t_other) noexcept = delete;

        ~MdciiFile() noexcept;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        [[nodiscard]] const std::string& GetFileName() const { return m_fileName; }

        [[nodiscard]] const nlohmann::json& GetJson() const { return m_json; }
        [[nodiscard]] nlohmann::json& GetJson() { return m_json; }

        //-------------------------------------------------
        // To Json
        //-------------------------------------------------

        /**
         * @brief Sets the Json value from a file.
         *
         * @return True if successful, false if an error occurs while loading file.
         */
        [[nodiscard]] bool SetJsonFromFile();

        /**
         * @brief Sets the Json value representing an Island.
         *
         * @param t_width The width of the island.
         * @param t_height The height of the island.
         * @param t_climateZone The climate zone of the island.
         * @param t_terrainTiles The terrain tile of the island.
         * @param t_coastTiles The coast tiles of the island.
         */
        void SetJsonFromIsland(
            int t_width, int t_height,
            world::ClimateZone t_climateZone,
            const std::vector<world::Tile>& t_terrainTiles,
            const std::vector<world::Tile>& t_coastTiles
        );

        //-------------------------------------------------
        // From Json
        //-------------------------------------------------

        /**
         * @brief Saves the Json value in a file.
         *
         * If the file already exists, the method returns false.
         *
         * @param t_mdciiFileType The file type to save.
         *
         * @return True if success or false if error while saving.
         */
        [[nodiscard]] bool CreateFileFromJson(MdciiFileType t_mdciiFileType);

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * @brief The file extension of the island files.
         */
        static constexpr std::string_view ISLAND_FILE_EXTENSION{ ".isl" };

        /**
         * @brief The relative path to the island files.
         */
        static constexpr std::string_view ISLAND_RELATIVE_PATH{ "island/" };

        /**
         * @brief The file extension of the map files.
         */
        static constexpr std::string_view MAP_FILE_EXTENSION{ ".map" };

        /**
         * @brief The relative path to the map files.
         */
        static constexpr std::string_view MAP_RELATIVE_PATH{ "map/" };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The name of the file.
         */
        std::string m_fileName;

        /**
         * @brief The Json value.
         */
        nlohmann::json m_json;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * @brief Initializes the full file path by appending the relative path and file extension.
         *
         * This method checks if the current file name (m_fileName) already contains the complete path,
         * including the relative path and file extension. If not, it appends the relative path (t_relPath),
         * user-specified path, and the file extension (t_fileExtension) to the file name.
         *
         * @param t_relPath The relative path to be appended to the file name.
         * @param t_fileExtension The file extension to be appended to the file name.
         */
        void InitFileName(const std::string& t_relPath, const std::string& t_fileExtension);
    };
}
