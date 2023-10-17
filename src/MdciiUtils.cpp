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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#include <fstream>
#include "MdciiUtils.h"
#include "MdciiException.h"
#include "Log.h"

//-------------------------------------------------
// Files
//-------------------------------------------------

nlohmann::json mdcii::read_json_from_file(const std::string& t_filePath)
{
    MDCII_LOG_DEBUG("[read_json_from_file()] Starts creating Json value from file {}...", t_filePath);

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
        throw MDCII_EXCEPTION("[read_json_from_file()] Exception caught while loading file " + t_filePath + ".");
    }

    MDCII_LOG_DEBUG("[read_json_from_file()] The Json value was created successfully.");

    return j;
}
