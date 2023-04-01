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

bool mdcii::file::SavegameFile::CheckFileFormat() const
{
    Log::MDCII_LOG_DEBUG("[SavegameFile::CheckFileFormat()] Check file format for save-game file {}.", fileName);

    if (json.contains("version") && json.contains("world") && json.contains("islands"))
    {
        Log::MDCII_LOG_DEBUG("[SavegameFile::CheckFileFormat()] The file {} is a valid save-game file.", fileName);
        return true;
    }

    Log::MDCII_LOG_WARN("[SavegameFile::CheckFileFormat()] Invalid save-game file format found in file {}.", fileName);

    return false;
}

std::string mdcii::file::SavegameFile::GetFileExtension() const
{
    return ".sav";
}

std::string mdcii::file::SavegameFile::GetRelPath() const
{
    return "save/";
}
