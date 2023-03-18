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
        // Save
        //-------------------------------------------------

        /**
         * Saves the Json value in a file.
         *
         * @return True if success or false if error while saving.
         */
        [[nodiscard]] bool SaveJsonToFile();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        void ClearJson();

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The name of the file.
         */
        std::string m_fileName;

        /**
         * Init flag.
         */
        bool m_initialized{ false };

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        [[nodiscard]] virtual std::string GetFileExtension() const = 0;
        [[nodiscard]] virtual std::string GetRelPath() const = 0;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes the class.
         */
        void Init();
    };
}
