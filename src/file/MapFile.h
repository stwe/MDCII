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
     * Forward declaration class GeneratorWorld.
     */
    class GeneratorWorld;
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

        /**
         * Adds Json values of the given GeneratorWorld object.
         *
         * @param t_generatorWorld The GeneratorWorld to convert.
         */
        void AddGeneratorWorld(world::GeneratorWorld const* t_generatorWorld);

        /**
         * Adds Json values of an island from a file.
         *
         * @param t_startMapX The start x position in the world.
         * @param t_startMapY The start y position in the world.
         * @param t_islandFileName The filename of the island.
         *
         * @return True if success, otherwise false.
         */
        [[nodiscard]] bool AddIslandFromFile(int32_t t_startMapX, int32_t t_startMapY, const std::string& t_islandFileName);

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
