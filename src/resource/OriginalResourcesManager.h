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

#include <memory>
#include <filesystem>
#include <functional>
#include "Buildings.h"

namespace mdcii::resource
{
    //-------------------------------------------------
    // OriginalResourcesManager
    //-------------------------------------------------

    class OriginalResourcesManager
    {
    public:
        /**
         * @brief The decrypted content of the haeuser.cod.
         */
        std::unique_ptr<Buildings> buildings;

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
         * @brief For convenience and better readability. Get a Building object by Id.
         *
         * @param t_id The given Id.
         *
         * @return The Building object.
         */
        [[nodiscard]] const Building& GetBuildingById(int t_id) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The path to the haeuser.cod file.
         */
        std::filesystem::path m_buildingsPath;

        //-------------------------------------------------
        // Path to the original data
        //-------------------------------------------------

        /**
         * @brief Get all paths to the original data.
         */
        void GetPathsFromOriginal();

        /**
         * @brief Iterates over the file system and calls a function on a condition.
         *
         * @param t_fileName The name of a file.
         * @param t_doForFileName The function to call on the file.
         */
        static void IterateFilesystem(
            const std::string& t_fileName,
            const std::function<void(const std::filesystem::directory_entry&, const std::string&)>& t_doForFileName
        );

        /**
         * @brief Finds the path to the haeuser.cod file.
         */
        void FindBuildingsCodFilePath();

        //-------------------------------------------------
        // Load original data
        //-------------------------------------------------

        /**
         * @brief Load original data from files.
         */
        void LoadFiles();
    };
}
