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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#include "MdciiFile.h"
#include "MdciiAssert.h"
#include "MdciiUtils.h"
#include "Game.h"
#include "world/Tile.h"
#include "vendor/enum/magic_enum.hpp"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::resource::MdciiFile::MdciiFile(std::string t_fileName)
    : m_fileName{ std::move(t_fileName) }
{
    MDCII_LOG_DEBUG("[MdciiFile::MdciiFile()] Create MdciiFile.");

    MDCII_ASSERT(!m_fileName.empty(), "[MdciiFile::MdciiFile()] Invalid file name.")
}

mdcii::resource::MdciiFile::~MdciiFile() noexcept
{
    MDCII_LOG_DEBUG("[MdciiFile::~MdciiFile()] Destruct MdciiFile.");
}

//-------------------------------------------------
// To Json
//-------------------------------------------------

bool mdcii::resource::MdciiFile::SetJsonFromFile()
{
    MDCII_LOG_DEBUG("[MdciiFile::SetJsonFromFile()] Start loading Json value from file {}.", m_fileName);

    MDCII_ASSERT(m_json.empty(), "[MdciiFile::SetJsonFromFile()] Invalid Json value.")

    if (std::filesystem::exists(m_fileName))
    {
        m_json = read_json_from_file(m_fileName);
        MDCII_LOG_DEBUG("[MdciiFile::SetJsonFromFile()] The Json value was successfully read from file {}.", m_fileName);

        return true;
    }

    MDCII_LOG_WARN("[MdciiFile::SetJsonFromFile()] File {} does not exist.", m_fileName);

    return false;
}

void mdcii::resource::MdciiFile::SetJsonFromIsland(
    const int t_width, const int t_height,
    const world::ClimateZone t_climateZone,
    const std::vector<world::Tile>& t_terrainTiles,
    const std::vector<world::Tile>& t_coastTiles
)
{
    MDCII_LOG_DEBUG("[MdciiFile::SetJsonFromIsland()] Set new values for the Json keys.");

    MDCII_ASSERT(m_json.empty(), "[MdciiFile::SetJsonFromIsland()] Invalid Json value.")

    // island
    m_json["width"] = t_width;
    m_json["height"] = t_height;
    m_json["x"] = -1;
    m_json["y"] = -1;
    m_json["climate"] = std::string(magic_enum::enum_name(t_climateZone));

    // layers
    m_json["layers"] = nlohmann::json::array();

    auto c = nlohmann::json::object();
    c["coast"] = t_terrainTiles;

    auto t = nlohmann::json::object();
    t["terrain"] = t_coastTiles;

    m_json["layers"].push_back(c);
    m_json["layers"].push_back(t);
}

//-------------------------------------------------
// From Json
//-------------------------------------------------

bool mdcii::resource::MdciiFile::CreateFileFromJson(const MdciiFileType t_mdciiFileType)
{
    if (t_mdciiFileType == MdciiFileType::ISLAND)
    {
        InitFileName(std::string(ISLAND_RELATIVE_PATH), std::string(ISLAND_FILE_EXTENSION));
    }
    else
    {
        InitFileName(std::string(MAP_RELATIVE_PATH), std::string(MAP_FILE_EXTENSION));
    }

    MDCII_LOG_DEBUG("[MdciiFile::CreateFileFromJson()] Start saving Json value to file {}.", m_fileName);

    if (std::ofstream file; create_file(m_fileName, file))
    {
        file << m_json;
        MDCII_LOG_DEBUG("[MdciiFile::CreateFileFromJson()] The Json value has been successfully saved in file {}.", m_fileName);

        return true;
    }

    MDCII_LOG_WARN("[MdciiFile::CreateFileFromJson()] An error occurred while saving Json value to file {}.", m_fileName);

    return false;
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::resource::MdciiFile::InitFileName(const std::string& t_relPath, const std::string& t_fileExtension)
{
    if (m_fileName.find(fmt::format("{}{}", Game::RESOURCES_REL_PATH, t_relPath)) == std::string::npos)
    {
        m_fileName = fmt::format("{}{}{}", Game::RESOURCES_REL_PATH, t_relPath, m_fileName.append(t_fileExtension));

        MDCII_LOG_DEBUG("[MdciiFile::InitFileName()] The full path to the file was initialized to {}.", m_fileName);
    }
}
