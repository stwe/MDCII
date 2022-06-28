#include <fstream>
#include <magic_enum.hpp>
#include "Text.h"
#include "Game.h"
#include "Log.h"
#include "MdciiException.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::data::Text::Text()
{
    Log::MDCII_LOG_DEBUG("[Text::Text()] Create Text.");

    ReadTexts();
    ReadMenus();
}

mdcii::data::Text::~Text() noexcept
{
    Log::MDCII_LOG_DEBUG("[Text::~Text()] Destruct Text.");
}

//-------------------------------------------------
// Read
//-------------------------------------------------

void mdcii::data::Text::ReadTexts()
{
    Log::MDCII_LOG_DEBUG("[Text::ReadFileData()] Start reading Json data...");

    nlohmann::json j{ ReadJsonFromFile(Game::RESOURCES_PATH + "data/Texts.json") };

    for (const auto& [s, v] : j[0].items())
    {
        // e.g.: s = WORKSHOPS
        auto section{ magic_enum::enum_cast<Section>(s) };
        if (section.has_value())
        {
            // e.g.: l = de
            for (const auto& [l, o] : v.items())
            {
                std::map<std::string, std::string> e;

                for (const auto& [buildingId, text] : o.items())
                {
                    e.emplace(buildingId, text);
                }

                m_buildings.emplace(
                    std::make_pair(section.value(), l),
                    e
                );
            }
        }
    }

    Log::MDCII_LOG_DEBUG("[Text::ReadFileData()] Json data read successfully.");
}

void mdcii::data::Text::ReadMenus()
{
    Log::MDCII_LOG_DEBUG("[Text::ReadMenus()] Start reading Json data...");

    nlohmann::json j{ ReadJsonFromFile(Game::RESOURCES_PATH + "data/Menus.json") };

    for (const auto& [l, o] : j[0].items())
    {
        std::map<std::string, std::string> e;

        for (const auto& [k, v] : o.items())
        {
            e.emplace(k, v);
        }

        m_menus.emplace(l, e);
    }

    Log::MDCII_LOG_DEBUG("[Text::ReadMenus()] Json data read successfully.");
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

nlohmann::json mdcii::data::Text::ReadJsonFromFile(const std::string& t_filePath) const
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
        throw MDCII_EXCEPTION("[Text::ReadJsonFromFile()] Exception caught while loading file " + t_filePath + ".");
    }

    return j;
}
