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

void mdcii::file::MapFile::AddIslandFromFile(const std::string& t_fileName)
{
    // todo
}

//-------------------------------------------------
// Override
//-------------------------------------------------

std::string mdcii::file::MapFile::GetFileExtension() const
{
    return ".map";
}

std::string mdcii::file::MapFile::GetRelPath() const
{
    return "map/";
}
