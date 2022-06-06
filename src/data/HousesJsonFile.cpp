#include <fstream>
#include <nlohmann/json.hpp>
#include <magic_enum.hpp>
#include "HousesJsonFile.h"
#include "Game.h"
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
        jsonFile.open(Game::RESOURCES_PATH + HOUSES_JSON_FILE_PATH);
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
        TileAsset tileAsset;

        auto& vars = node["variables"]["variable"];

        for (auto& var : vars)
        {
            auto& name = var["name"];
            if (name == "Id")
            {
                tileAsset.id = var["valueInt"];
                tileAsset.id -= 20000;
            }
            if (name == "Gfx")
            {
                tileAsset.gfx = var["valueInt"];
            }
            if (name == "Blocknr")
            {
                tileAsset.blocknr = var["valueInt"];
            }
            if (name == "Kind")
            {
                const std::string str{ var["valueString"] };
                const auto strOpt{ magic_enum::enum_cast<TileKind>(str) };
                if (strOpt.has_value())
                {
                    tileAsset.kind = strOpt.value();
                }
            }
            if (name == "Posoffs")
            {
                tileAsset.posoffs = var["valueInt"];
            }
            if (name == "Highflg")
            {
                tileAsset.highflg = var["valueInt"];
            }

            // einhoffs
            // maxenergy
            // maxbrand

            if (name == "Size")
            {
                auto& values = var["valueArray"]["value"];
                tileAsset.width = values[0]["valueInt"];
                tileAsset.height = values[1]["valueInt"];
            }
            if (name == "Rotate")
            {
                tileAsset.rotate = var["valueInt"];
            }

            // randAnz
            // randAdd

            if (name == "AnimTime")
            {
                // skip TIMENEVER string nodes
                if (var["valueInt"].is_number())
                {
                    tileAsset.animTime = var["valueInt"];
                }
            }
            if (name == "AnimFrame")
            {
                tileAsset.animFrame = var["valueInt"];
            }
            if (name == "AnimAdd")
            {
                tileAsset.animAdd = var["valueInt"];
            }
            if (name == "Baugfx")
            {
                tileAsset.baugfx = var["valueInt"];
            }

            // placeFlg

            if (name == "AnimAnz")
            {
                tileAsset.animAnz = var["valueInt"];
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

        tileAssets.emplace(tileAsset.id, tileAsset);
        tileAssetsByKind.emplace(tileAsset.kind, tileAsset);
    }

    Log::MDCII_LOG_DEBUG("[HousesJsonFile::ReadFileData()] {} TileAsset objects created successfully.", tileAssets.size());
}
