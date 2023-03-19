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
// MapFile
//-------------------------------------------------

namespace mdcii::file
{
    /**
     * Represents a map file.
     */
    class MapFile : public MdciiFile
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MapFile() = delete;

        /**
         * Constructs a new MapFile object.
         *
         * @param t_fileName The name of the file.
         */
        explicit MapFile(std::string t_fileName);

        MapFile(const MapFile& t_other) = delete;
        MapFile(MapFile&& t_other) noexcept = delete;
        MapFile& operator=(const MapFile& t_other) = delete;
        MapFile& operator=(MapFile&& t_other) noexcept = delete;

        ~MapFile() noexcept override;

        //-------------------------------------------------
        // Json
        //-------------------------------------------------

        void AddWorldData(int32_t t_width, int32_t t_height);

        void AddIsland(
            int32_t t_startMapX, int32_t t_startMapY,
            int32_t t_width, int32_t t_height,
            const std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles,
            const std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles
        );

        [[nodiscard]] bool AddIslandFromFile(int32_t t_startMapX, int32_t t_startMapY, const std::string& t_islandFileName);

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
