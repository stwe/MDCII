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
#include "world/GeneratorWorld.h"

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

void mdcii::file::MapFile::AddGeneratorWorld(const world::GeneratorWorld* t_generatorWorld)
{
    json = *t_generatorWorld;
}

bool mdcii::file::MapFile::AddIslandFromFile(const int32_t t_startMapX, const int32_t t_startMapY, const std::string& t_islandFileName)
{
    if (IslandFile islandFile{ t_islandFileName }; islandFile.LoadJsonFromFile())
    {
        auto islandJson = islandFile.json;
        islandJson["x"] = t_startMapX;
        islandJson["y"] = t_startMapY;

        json["islands"].push_back(islandJson);

        return true;
    }

    return false;
}

//-------------------------------------------------
// Override
//-------------------------------------------------

bool mdcii::file::MapFile::ValidateJson() const
{
    Log::MDCII_LOG_DEBUG("[MapFile::ValidateJson()] Check Json value in map file {}.", fileName);

    if (!json.empty() &&
        json.contains("version") &&
        json.contains("world") &&
        json.contains("width") &&
        json.contains("height") &&
        json.contains("islands"))
    {
        Log::MDCII_LOG_DEBUG("[MapFile::ValidateJson()] The Json value in map file {} is valid.", fileName);
        return true;
    }

    Log::MDCII_LOG_WARN("[MapFile::ValidateJson()] Found invalid Json value in map file {}.", fileName);

    return false;
}

bool mdcii::file::MapFile::ValidateObject() const
{
    Log::MDCII_LOG_DEBUG("[MapFile::ValidateObject()] Check object values in map file {}.", fileName);

    const auto w = json.at("width").get<int32_t>();
    const auto h = json.at("height").get<int32_t>();

    if (w < world::World::WORLD_MIN_WIDTH || w > world::World::WORLD_MAX_WIDTH ||
        h < world::World::WORLD_MIN_HEIGHT || h > world::World::WORLD_MAX_HEIGHT)
    {
        Log::MDCII_LOG_WARN("[MapFile::ValidateObject()] Found invalid object values in map file {}.", fileName);
        return false;
    }

    Log::MDCII_LOG_DEBUG("[MapFile::ValidateObject()] The object values in map file {} are valid.", fileName);

    return true;
}

std::string mdcii::file::MapFile::GetFileExtension() const
{
    return ".map";
}

std::string mdcii::file::MapFile::GetRelPath() const
{
    return "map/";
}
