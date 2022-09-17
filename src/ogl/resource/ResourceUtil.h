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

#include <fstream>
#include <vector>
#include "MdciiException.h"
#include "Game.h"

//-------------------------------------------------
// ResourceUtil
//-------------------------------------------------

namespace mdcii::ogl::resource
{
    /**
     * Static methods to read in shader files.
     */
    class ResourceUtil
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        ResourceUtil(ResourceUtil&& t_other) noexcept = delete;
        ResourceUtil(const ResourceUtil& t_other) = delete;
        ResourceUtil& operator=(const ResourceUtil& t_other) = delete;
        ResourceUtil& operator=(ResourceUtil&& t_other) noexcept = delete;

        //-------------------------------------------------
        // Utils
        //-------------------------------------------------

        /**
         * Reads a shader file into a string.
         *
         * @param t_fileName The path to the shader file.
         *
         * @return A string with the shader code.
         */
        static auto ReadShaderFile(const std::string& t_fileName)
        {
            std::string content;
            std::ifstream shaderFile;

            // Ensure ifstream objects can throw exceptions.
            shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

            try
            {
                // Open file.
                shaderFile.open(Game::RESOURCES_REL_PATH + t_fileName);
                std::stringstream shaderStream;

                // Read file buffer content into stream.
                shaderStream << shaderFile.rdbuf();

                // Close file.
                shaderFile.close();

                // Convert stream into string.
                content = shaderStream.str();
            }
            catch (const std::ifstream::failure&)
            {
                throw MDCII_EXCEPTION("[ResourceUtil::ReadShaderFile()] Exception caught while loading of file " + t_fileName + ".");
            }

            return content;
        }

        /**
         * Find occurrences of a search value in another string.
         *
         * @param t_positions The search results.
         * @param t_data The string in which to search.
         * @param t_toSearch The search string.
         */
        static void FindAllOccurances(std::vector<size_t>& t_positions, const std::string& t_data, const std::string& t_toSearch)
        {
            // Get the first occurrence.
            auto pos{ t_data.find(t_toSearch) };

            // Repeat till end is reached.
            while (pos != std::string::npos)
            {
                // Add position to the vector.
                t_positions.push_back(pos);

                // Get the next occurrence from the current position.
                pos = t_data.find(t_toSearch, pos + t_toSearch.size());
            }
        }

    protected:

    private:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        ResourceUtil() = default;
        ~ResourceUtil() noexcept = default;
    };
}
