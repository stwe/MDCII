#include <fstream>
#include "MapContent.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "MdciiException.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::map::MapContent::MapContent(const std::string& t_filePath)
{
    Log::MDCII_LOG_DEBUG("[MapContent::MapContent()] Create MapContent.");

    CreateTiles(t_filePath);
}

mdcii::map::MapContent::~MapContent() noexcept
{
    Log::MDCII_LOG_DEBUG("[MapContent::~MapContent()] Destruct MapContent.");
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::map::MapContent::CreateTiles(const std::string& t_filePath)
{
    Log::MDCII_LOG_DEBUG("[MapContent::CreateTiles()] Start reading Json data...");

    nlohmann::json j{ ReadJsonFromFile(Game::RESOURCES_PATH + t_filePath) };

    for (const auto& [k, v] : j.items())
    {
        if (k == "width")
        {
            width = v.get<int>();
        }
        else if (k == "height")
        {
            height = v.get<int>();
        }
        else if (k == "map")
        {
            for (const auto& [a, o] : v.items())
            {
                mapTiles.emplace_back(o.get<MapTile>());
            }
        }
    }

    MDCII_ASSERT(!mapTiles.empty(), "[MapContent::CreateTiles()] Empty map.")
    MDCII_ASSERT(width, "[MapContent::CreateTiles()] Invalid width.")
    MDCII_ASSERT(height, "[MapContent::CreateTiles()] Invalid height.")
    MDCII_ASSERT((static_cast<size_t>(width) * static_cast<size_t>(height)) == mapTiles.size(), "[MapContent::CreateTiles()] Invalid size.")

    Log::MDCII_LOG_DEBUG("[MapContent::CreateTiles()] Json data read successfully.");
}

nlohmann::json mdcii::map::MapContent::ReadJsonFromFile(const std::string& t_filePath) const
{
    nlohmann::json j;

    std::ifstream jsonFile;
    jsonFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        jsonFile.open(t_filePath);
        j = nlohmann::json::parse(jsonFile);
        jsonFile.close();
    }
    catch (const std::ifstream::failure&)
    {
        throw MDCII_EXCEPTION("[MapContent::ReadMenus()] Exception caught while loading file " + t_filePath + ".");
    }

    return j;
}
