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

namespace mdcii::world
{
    class Island;
}

namespace mdcii::resource
{
    //-------------------------------------------------
    // MdciiFile
    //-------------------------------------------------

    /**
     * @brief Class for all MDCII Json file formats.
     */
    class MdciiFile
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The name of the file.
         */
        std::string fileName;

        /**
         * @brief The current Json value.
         */
        nlohmann::json json;

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
        MdciiFile(MdciiFile&& t_other) noexcept = delete;
        MdciiFile& operator=(const MdciiFile& t_other) = delete;
        MdciiFile& operator=(MdciiFile&& t_other) noexcept = delete;

        ~MdciiFile() noexcept;

        //-------------------------------------------------
        // Save && load
        //-------------------------------------------------

        /**
         * @brief Loads the Json value from a file.
         *
         * @return True if success or false if error while loading.
         */
        [[nodiscard]] bool LoadJsonFromFile();

        [[nodiscard]] std::vector<std::unique_ptr<world::Island>> CreateIslands() const;

    protected:

    private:

    };
}
