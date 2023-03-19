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

        void AddStartPosition(int32_t t_startMapX, int32_t t_startMapY);
        void AddData(
            int32_t t_width, int32_t t_height,
            const std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles,
            const std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles
        );

        static void AddStartPosition(nlohmann::json& t_json, int32_t t_startMapX, int32_t t_startMapY);
        static void AddWidthAndHeight(nlohmann::json& t_json, int32_t t_width, int32_t t_height);
        static void AddData(
            nlohmann::json& t_json,
            int32_t t_width, int32_t t_height,
            const std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles,
            const std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles
        );

    protected:

    private:
        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        [[nodiscard]] bool CheckFileFormat() const override;
        [[nodiscard]] std::string GetFileExtension() const override;
        [[nodiscard]] std::string GetRelPath() const override;
    };
}
