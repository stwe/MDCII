#include <fstream>
#include <nlohmann/json.hpp>
#include "HousesJsonFile.h"
#include "Log.h"
#include "MdciiException.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::data::HousesJsonFile::HousesJsonFile()
{
    Log::MDCII_LOG_DEBUG("[HousesJsonFile::HousesJsonFile()] Create HousesJsonFile.");

    ReadFileData();
}

mdcii::data::HousesJsonFile::~HousesJsonFile() noexcept
{
    Log::MDCII_LOG_DEBUG("[HousesJsonFile::~HousesJsonFile()] Destruct HousesJsonFile.");
}

//-------------------------------------------------
// Read
//-------------------------------------------------

void mdcii::data::HousesJsonFile::ReadFileData()
{
    Log::MDCII_LOG_DEBUG("[HousesJsonFile::ReadFileData()] Start reading HAUS Json data...");

    nlohmann::basic_json json;

    std::ifstream jsonFile;
    jsonFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        jsonFile.open(HOUSES_JSON_FILE_PATH);
        json = nlohmann::json::parse(jsonFile);
        jsonFile.close();
    }
    catch (const std::ifstream::failure&)
    {
        throw MDCII_EXCEPTION("[HousesJsonFile::ReadFileData()] Exception caught while loading of file " + HOUSES_JSON_FILE_PATH + ".");
    }

    auto& nodes = json["object"][2]["objects"];

    for (auto& node : nodes)
    {
        TileAssetProperties properties;

        auto& vars = node["variables"]["variable"];

        for (auto& var : vars)
        {
            auto& name = var["name"];
            if (name == "Id")
            {
                properties.id = var["valueInt"];
                properties.id -= 20000;
            }
            if (name == "Gfx")
            {
                properties.gfx = var["valueInt"];
            }
            if (name == "Blocknr")
            {
                properties.blocknr = var["valueInt"];
            }
            if (name == "Kind")
            {
                properties.kind = var["valueString"];
            }
            if (name == "Posoffs")
            {
                properties.posoffs = var["valueInt"];
            }
            if (name == "Highflg")
            {
                properties.highflg = var["valueInt"];
            }

            // einhoffs
            // maxenergy
            // maxbrand

            if (name == "Size")
            {
                auto& values = var["valueArray"]["value"];
                properties.width = values[0]["valueInt"];
                properties.height = values[1]["valueInt"];
            }
            if (name == "Rotate")
            {
                properties.rotate = var["valueInt"];
            }

            // randAnz
            // randAdd

            if (name == "AnimTime")
            {
                // skip TIMENEVER string nodes
                if (var["valueInt"].is_number())
                {
                    properties.animTime = var["valueInt"];
                }
            }
            if (name == "AnimFrame")
            {
                properties.animFrame = var["valueInt"];
            }
            if (name == "AnimAdd")
            {
                // todo: sometimes missing
                properties.animAdd = var["valueInt"];
            }
            if (name == "Baugfx")
            {
                properties.baugfx = var["valueInt"];
            }

            // placeFlg

            if (name == "AnimAnz")
            {
                properties.animAnz = var["valueInt"];
            }

            // randwachs
            // strandoff
            // kreuzBase
            // noShotFlg
            // strandflg
            // ausbauflg
            // tuerflg
            // destroyflg
        }

        tileAssetPropertiesMap.emplace(properties.id, properties);
    }

    Log::MDCII_LOG_DEBUG("[HousesJsonFile::ReadFileData()] {} TileAssetProperties objects created successfully.", tileAssetPropertiesMap.size());
}
