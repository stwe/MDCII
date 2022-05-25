#pragma once

#include <string>
#include "Building.h"

namespace mdcii::data
{
    class HousesJsonFile
    {
    public:

        std::unordered_map<int, Building> buildings;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        HousesJsonFile() = delete;

        explicit HousesJsonFile(const std::string& t_filePath);

        HousesJsonFile(const HousesJsonFile& t_other) = delete;
        HousesJsonFile(HousesJsonFile&& t_other) noexcept = delete;
        HousesJsonFile& operator=(const HousesJsonFile& t_other) = delete;
        HousesJsonFile& operator=(HousesJsonFile&& t_other) noexcept = delete;

        ~HousesJsonFile();

        //-------------------------------------------------
        // Read
        //-------------------------------------------------

        void ReadFileData(const std::string& t_filePath);

    protected:

    private:

    };
}
