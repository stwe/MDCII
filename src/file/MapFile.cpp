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

#include "MapFile.h"
#include "IslandFile.h"
#include "Log.h"
#include "Game.h"
#include "layer/TerrainLayer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::file::MapFile::MapFile(std::string t_fileName)
    : MdciiFile(std::move(t_fileName))
{
    Log::MDCII_LOG_DEBUG("[MapFile::MapFile()] Create MapFile.");
}

mdcii::file::MapFile::~MapFile() noexcept
{
    Log::MDCII_LOG_DEBUG("[MapFile::~MapFile()] Destruct MapFile.");
}

//-------------------------------------------------
// Json
//-------------------------------------------------

void mdcii::file::MapFile::AddWorldData(const int32_t t_width, const int32_t t_height)
{
    json["version"] = Game::VERSION;
    json["world"] = { { "width", t_width }, { "height", t_height } };
}

void mdcii::file::MapFile::AddIsland(
    const int32_t t_startMapX, const int32_t t_startMapY,
    const int32_t t_width, const int32_t t_height,
    const std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles,
    const std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles
)
{
    nlohmann::json islandJson;
    file::IslandFile::AddStartPosition(islandJson, t_startMapX, t_startMapY);
    file::IslandFile::AddData(islandJson, t_width, t_height, t_terrainTiles, t_coastTiles);

    json["islands"].push_back(islandJson);
}

bool mdcii::file::MapFile::AddIslandFromFile(const int32_t t_startMapX, const int32_t t_startMapY, const std::string& t_islandFileName)
{
    if (IslandFile islandFile{ t_islandFileName }; islandFile.LoadJsonFromFile())
    {
        islandFile.AddStartPosition(t_startMapX, t_startMapY);
        json["islands"].push_back(islandFile.json);

        return true;
    }

    return false;

    /*
    // newMapFile.AddIslandFromFile(1, 1, Game::RESOURCES_REL_PATH + "island/" + "Island24x24.isl");
    nlohmann::json j = read_json_from_file(t_islandFileName); // todo throws exception
    j["x"] = t_startMapX;
    j["y"] = t_startMapY;
    json["islands"].push_back(j);
    */
}

//-------------------------------------------------
// Override
//-------------------------------------------------

bool mdcii::file::MapFile::CheckFileFormat() const
{
    Log::MDCII_LOG_DEBUG("[MapFile::CheckFileFormat()] Check file format for map file {}.", fileName);

    if (json.contains("version") && json.contains("world") && json.contains("islands"))
    {
        Log::MDCII_LOG_DEBUG("[MapFile::CheckFileFormat()] The file {} is a valid map file.", fileName);
        return true;
    }

    Log::MDCII_LOG_WARN("[MapFile::CheckFileFormat()] Invalid map file format found in file {}.", fileName);

    return false;
}

std::string mdcii::file::MapFile::GetFileExtension() const
{
    return ".map";
}

std::string mdcii::file::MapFile::GetRelPath() const
{
    return "map/";
}
