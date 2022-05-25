#include <fstream>
#include <nlohmann/json.hpp>
#include "HousesJsonFile.h"
#include "Log.h"
#include "MdciiException.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::data::HousesJsonFile::HousesJsonFile(const std::string& t_filePath)
{
    Log::MDCII_LOG_DEBUG("[HousesJsonFile::HousesJsonFile()] Create HousesJsonFile.");

    ReadFileData(t_filePath);
}

mdcii::data::HousesJsonFile::~HousesJsonFile()
{
    Log::MDCII_LOG_DEBUG("[HousesJsonFile::~HousesJsonFile()] Destruct HousesJsonFile.");
}

//-------------------------------------------------
// Read
//-------------------------------------------------

void mdcii::data::HousesJsonFile::ReadFileData(const std::string& t_filePath)
{
    Log::MDCII_LOG_DEBUG("[HousesJsonFile::ReadFileData()] Start reading Json data...");

    nlohmann::basic_json json;

    std::ifstream jsonFile;
    jsonFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        jsonFile.open(t_filePath);
        json = nlohmann::json::parse(jsonFile);
        jsonFile.close();
    }
    catch (const std::ifstream::failure&)
    {
        throw MDCII_EXCEPTION("[HousesJsonFile::ReadFileData()] Exception caught while loading of file " + t_filePath + ".");
    }

    auto& nodes = json["object"][2]["objects"];

    for (auto& node : nodes)
    {
        Building building;

        auto& vars = node["variables"]["variable"];

        for (auto& var : vars)
        {
            auto& name = var["name"];
            if (name == "Id")
            {
                building.id = var["valueInt"];
                building.id -= 20000;
            }
            if (name == "Gfx")
            {
                building.gfx = var["valueInt"];
            }
            if (name == "Blocknr")
            {
                building.blocknr = var["valueInt"];
            }
            if (name == "Posoffs")
            {
                building.posoffs = var["valueInt"];
            }
            if (name == "Size")
            {
                auto& values = var["valueArray"]["value"];
                building.width = values[0]["valueInt"];
                building.height = values[1]["valueInt"];
            }
            if (name == "Rotate")
            {
                building.rotate = var["valueInt"];
            }
            if (name == "AnimFrame")
            {
                building.animFrame = var["valueInt"];
            }
            if (name == "AnimAnz")
            {
                building.animAnz = var["valueInt"];
            }
        }

        if (building.id > 0)
        {
            buildings.emplace(building.id, building);
        }
    }

    Log::MDCII_LOG_DEBUG("[HousesJsonFile::ReadFileData()] Json data read successfully.");
}
