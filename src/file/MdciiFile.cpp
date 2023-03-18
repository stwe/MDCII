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
    : m_fileName{ std::move(t_fileName) }
{
    Log::MDCII_LOG_DEBUG("[MdciiFile::MdciiFile()] Create MdciiFile.");

    MDCII_ASSERT(!m_fileName.empty(), "[MdciiFile::MdciiFile()] Invalid file name.")
}

mdcii::file::MdciiFile::~MdciiFile() noexcept
{
    Log::MDCII_LOG_DEBUG("[MdciiFile::~MdciiFile()] Destruct MdciiFile.");
}

//-------------------------------------------------
// Save
//-------------------------------------------------

bool mdcii::file::MdciiFile::SaveJsonToFile()
{
    Init();

    Log::MDCII_LOG_DEBUG("[MdciiFile::SaveJsonToFile()] Start saving Json value to file {}.", m_fileName);

    if (std::ofstream file; create_file(Game::RESOURCES_REL_PATH + GetRelPath() + m_fileName, file))
    {
        file << json;
        Log::MDCII_LOG_DEBUG("[MdciiFile::SaveJsonToFile()] The Json value has been successfully saved in file {}.", m_fileName);

        return true;
    }

    Log::MDCII_LOG_WARN("[MdciiFile::SaveJsonToFile()] An error occurred while saving Json value to file {}.", m_fileName);

    return false;
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::file::MdciiFile::ClearJson()
{
    json.clear();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::file::MdciiFile::Init()
{
    if (!m_initialized)
    {
        Log::MDCII_LOG_DEBUG("[MdciiFile::Init()] Initialize class.");

        m_fileName.append(GetFileExtension());
        m_initialized = true;
    }
}
