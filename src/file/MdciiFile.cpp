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

#include "MdciiFile.h"
#include "MdciiAssert.h"
#include "MdciiUtils.h"
#include "Game.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::file::MdciiFile::MdciiFile(std::string t_fileName)
    : fileName{ std::move(t_fileName) }
{
    Log::MDCII_LOG_DEBUG("[MdciiFile::MdciiFile()] Create MdciiFile.");

    MDCII_ASSERT(!fileName.empty(), "[MdciiFile::MdciiFile()] Invalid file name.")
}

mdcii::file::MdciiFile::~MdciiFile() noexcept
{
    Log::MDCII_LOG_DEBUG("[MdciiFile::~MdciiFile()] Destruct MdciiFile.");
}

//-------------------------------------------------
// Save && load
//-------------------------------------------------

bool mdcii::file::MdciiFile::SaveJsonToFile()
{
    InitFileName();

    Log::MDCII_LOG_DEBUG("[MdciiFile::SaveJsonToFile()] Start saving Json value to file {}.", fileName);

    if (!ValidateJson() || !ValidateObject())
    {
        return false;
    }

    if (std::ofstream file; create_file(fileName, file))
    {
        file << json;
        Log::MDCII_LOG_DEBUG("[MdciiFile::SaveJsonToFile()] The Json value has been successfully saved in file {}.", fileName);

        return true;
    }

    Log::MDCII_LOG_WARN("[MdciiFile::SaveJsonToFile()] An error occurred while saving Json value to file {}.", fileName);

    return false;
}

bool mdcii::file::MdciiFile::LoadJsonFromFile()
{
    InitFileName();

    Log::MDCII_LOG_DEBUG("[MdciiFile::LoadJsonFromFile()] Start loading Json value from file {}.", fileName);

    if (std::filesystem::exists(fileName))
    {
        json = read_json_from_file(fileName);
        Log::MDCII_LOG_DEBUG("[MdciiFile::LoadJsonFromFile()] The Json value was successfully read from file {}.", fileName);

        return ValidateJson() && ValidateObject();
    }

    Log::MDCII_LOG_WARN("[MdciiFile::LoadJsonFromFile()] File {} does not exist.", fileName);

    return false;
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::file::MdciiFile::InitFileName()
{
    if (!m_initializedFile)
    {
        fileName = Game::RESOURCES_REL_PATH + GetRelPath() + fileName.append(GetFileExtension());
        m_initializedFile = true;

        Log::MDCII_LOG_DEBUG("[MdciiFile::InitFileName()] The full path to the file was initialized to {}.", fileName);
    }
}
