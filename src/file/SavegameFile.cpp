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

#include "SavegameFile.h"
#include "Log.h"
#include "world/GameWorld.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::file::SavegameFile::SavegameFile(std::string t_fileName)
    : MdciiFile(std::move(t_fileName))
{
    Log::MDCII_LOG_DEBUG("[SavegameFile::SavegameFile()] Create SavegameFile.");
}

mdcii::file::SavegameFile::~SavegameFile() noexcept
{
    Log::MDCII_LOG_DEBUG("[SavegameFile::~SavegameFile()] Destruct SavegameFile.");
}

//-------------------------------------------------
// Json
//-------------------------------------------------

void mdcii::file::SavegameFile::AddGameWorld(world::GameWorld const* t_gameWorld)
{
    json = *t_gameWorld;
}

//-------------------------------------------------
// Override
//-------------------------------------------------

bool mdcii::file::SavegameFile::ValidateJson() const
{
    Log::MDCII_LOG_DEBUG("[SavegameFile::ValidateJson()] Check Json value in save-game file {}.", fileName);

    if (!json.empty() &&
        json.contains("version") &&
        json.contains("world") &&
        json["world"].contains("width") &&
        json["world"].contains("height") &&
        json.contains("islands"))
    {
        Log::MDCII_LOG_DEBUG("[SavegameFile::ValidateJson()] The Json value in save-game file {} is valid.", fileName);
        return true;
    }

    Log::MDCII_LOG_WARN("[SavegameFile::ValidateJson()] Found invalid Json value in save-game file {}.", fileName);

    return false;
}

bool mdcii::file::SavegameFile::ValidateObject() const
{
    Log::MDCII_LOG_DEBUG("[SavegameFile::ValidateObject()] Check object values in save-game file {}.", fileName);

    const auto w = json.at("world").at("width").get<int32_t>();
    const auto h = json.at("world").at("height").get<int32_t>();

    if (w < world::World::WORLD_MIN_WIDTH || w > world::World::WORLD_MAX_WIDTH ||
        h < world::World::WORLD_MIN_HEIGHT || h > world::World::WORLD_MAX_HEIGHT)
    {
        Log::MDCII_LOG_WARN("[SavegameFile::ValidateObject()] Found invalid object values in save-game file {}.", fileName);
        return false;
    }

    Log::MDCII_LOG_DEBUG("[SavegameFile::ValidateObject()] The object values in save-game file {} are valid.", fileName);

    return true;
}

std::string mdcii::file::SavegameFile::GetFileExtension() const
{
    return ".sav";
}

std::string mdcii::file::SavegameFile::GetRelPath() const
{
    return "save/";
}
