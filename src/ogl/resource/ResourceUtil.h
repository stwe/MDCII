#pragma once

#include <fstream>
#include <vector>
#include <map>
#include "MdciiException.h"

//-------------------------------------------------
// ResourceUtil
//-------------------------------------------------

namespace mdcii::ogl::resource
{
    /**
     * Static methods to read in files.
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
         * Reads the Grafiken.txt file to show graphic names.
         *
         * @param t_fileName The path to the file.
         *
         * @return A std::map
         */
        static auto ReadGraphicsFile(const std::string& t_fileName)
        {
            std::map<int, std::string> content;

            int id{ 0 };
            int gfx{ 0 };
            std::string name{};

            try
            {
                std::ifstream inFile;
                inFile.open(t_fileName);

                while (inFile >> id >> gfx >> name)
                {
                    content.emplace(id, name);
                }

                inFile.close();
            }
            catch (const std::ifstream::failure&)
            {
                throw MDCII_EXCEPTION("[ResourceUtil::ReadGraphicsFile()] Exception caught while loading of file " + t_fileName + ".");
            }

            return content;
        }

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
                shaderFile.open(t_fileName);
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
