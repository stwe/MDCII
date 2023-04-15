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

#include "MdciiFile.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * Forward declaration enum class ClimateZone.
     */
    enum class ClimateZone;
}

namespace mdcii::layer
{
    /**
     * Forward declaration struct Tile.
     */
    struct Tile;
}

//-------------------------------------------------
// IslandFile
//-------------------------------------------------

namespace mdcii::file
{
    /**
     * Represents an island file.
     */
    class IslandFile : public MdciiFile
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        IslandFile() = delete;

        /**
         * Constructs a new IslandFile object.
         *
         * @param t_fileName The name of the file.
         */
        explicit IslandFile(std::string t_fileName);

        IslandFile(const IslandFile& t_other) = delete;
        IslandFile(IslandFile&& t_other) noexcept = delete;
        IslandFile& operator=(const IslandFile& t_other) = delete;
        IslandFile& operator=(IslandFile&& t_other) noexcept = delete;

        ~IslandFile() noexcept override;

        //-------------------------------------------------
        // Json
        //-------------------------------------------------

        /**
         * Sets the Json value.
         *
         * @param t_width The island width
         * @param t_height The island height.
         * @param t_climateZone The climate zone of the island.
         * @param t_terrainTiles The terrain tiles of the island.
         * @param t_coastTiles The coast tiles of the island.
         */
        void SetData(
            int32_t t_width, int32_t t_height, world::ClimateZone t_climateZone,
            const std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles,
            const std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles
        );

        /**
         * Method to check Json values for an island.
         *
         * @param t_json The Json value to validate.
         * @param t_fileName The name of the file.
         *
         * @return True if valid, false otherwise.
         */
        static bool CheckJson(const nlohmann::json& t_json, const std::string& t_fileName);

    protected:

    private:
        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        [[nodiscard]] bool ValidateJson() const override;
        [[nodiscard]] bool ValidateObject() const override;

        [[nodiscard]] std::string GetFileExtension() const override;
        [[nodiscard]] std::string GetRelPath() const override;
    };
}
