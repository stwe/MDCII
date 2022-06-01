#pragma once

#include <fstream>
#include <map>
#include "MdciiException.h"

//-------------------------------------------------
// GraphicsFile
//-------------------------------------------------

namespace mdcii::data
{
    /**
     * Read in Grafiken.txt
     */
    class GraphicsFile
    {
    public:
        //-------------------------------------------------
        // Read
        //-------------------------------------------------

        /**
         * Reads the Grafiken.txt file to show graphic names.
         *
         * @param t_fileName The path to the Grafiken.txt.
         *
         * @return A std::map
         */
        static auto ReadGraphicsFile(const std::string& t_fileName) -> std::map<int, std::string>
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
                throw MDCII_EXCEPTION("[GraphicsFile::ReadGraphicsFile()] Exception caught while loading of file " + t_fileName + ".");
            }

            return content;
        }

    protected:

    private:

    };
}
