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
     * Forward declaration class GameWorld.
     */
    class GameWorld;
}

//-------------------------------------------------
// SavegameFile
//-------------------------------------------------

namespace mdcii::file
{
    /**
     * Represents a save-game file.
     */
    class SavegameFile : public MdciiFile
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        SavegameFile() = delete;

        /**
         * Constructs a new SavegameFile object.
         *
         * @param t_fileName The name of the file.
         */
        explicit SavegameFile(std::string t_fileName);

        SavegameFile(const SavegameFile& t_other) = delete;
        SavegameFile(SavegameFile&& t_other) noexcept = delete;
        SavegameFile& operator=(const SavegameFile& t_other) = delete;
        SavegameFile& operator=(SavegameFile&& t_other) noexcept = delete;

        ~SavegameFile() noexcept override;

        //-------------------------------------------------
        // Json
        //-------------------------------------------------

        /**
         * Adds Json values of the given GameWorld object.
         *
         * @param t_gameWorld The GameWorld to convert.
         */
        void AddGameWorld(world::GameWorld const* t_gameWorld);

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
