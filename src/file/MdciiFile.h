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

#include "data/json.hpp"

//-------------------------------------------------
// MdciiFile
//-------------------------------------------------

namespace mdcii::file
{
    /**
     * Base class for all own Json file formats.
     */
    class MdciiFile
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The name of the file.
         */
        std::string fileName;

        /**
         * The current Json value.
         */
        nlohmann::json json;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        MdciiFile() = delete;

        /**
         * Constructs a new MdciiFile object.
         *
         * @param t_fileName The name of the file.
         */
        explicit MdciiFile(std::string t_fileName);

        MdciiFile(const MdciiFile& t_other) = delete;
        MdciiFile(MdciiFile&& t_other) noexcept = delete;
        MdciiFile& operator=(const MdciiFile& t_other) = delete;
        MdciiFile& operator=(MdciiFile&& t_other) noexcept = delete;

        virtual ~MdciiFile() noexcept;

        //-------------------------------------------------
        // Save && load
        //-------------------------------------------------

        /**
         * Saves the Json value in a file.
         * If the file already exists, the method returns false.
         *
         * @return True if success or false if error while saving.
         */
        [[nodiscard]] bool SaveJsonToFile();

        /**
         * Loads the Json value from a file. The method checks whether the file exists.
         * If the file doesn't exist, the method returns false.
         *
         * @return True if success or false if error while loading.
         */
        [[nodiscard]] bool LoadJsonFromFile();

    protected:

    private:
        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        /**
         * Checks the Json value for specific entries.
         *
         * @return True if valid, false otherwise.
         */
        [[nodiscard]] virtual bool ValidateJson() const = 0;

        /**
         * Converts certain Json values into objects and checks whether these object values are valid.
         *
         * @return True if valid, false otherwise.
         */
        [[nodiscard]] virtual bool ValidateObject() const { return true; }

        /**
         * Returns the file extension.
         *
         * @return The file extension (e.g. ".map").
         */
        [[nodiscard]] virtual std::string GetFileExtension() const = 0;

        /**
         * Returns the relative path to the file.
         *
         * @return The relative path (e.g. "map/").
         */
        [[nodiscard]] virtual std::string GetRelPath() const = 0;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes the file name.
         */
        void InitFileName();
    };
}
