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

#include "IslandFile.h"
#include "Log.h"
#include "layer/TerrainLayer.h"
#include "world/Island.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::file::IslandFile::IslandFile(std::string t_fileName)
    : MdciiFile(std::move(t_fileName))
{
    Log::MDCII_LOG_DEBUG("[IslandFile::IslandFile()] Create IslandFile.");
}

mdcii::file::IslandFile::~IslandFile() noexcept
{
    Log::MDCII_LOG_DEBUG("[IslandFile::~IslandFile()] Destruct IslandFile.");
}

//-------------------------------------------------
// Json
//-------------------------------------------------

void mdcii::file::IslandFile::SetData(
    const int32_t t_width, const int32_t t_height, const world::ClimateZone t_climateZone,
    const std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles,
    const std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles
)
{
    Log::MDCII_LOG_DEBUG("[IslandFile::SetData()] Set new values for the Json keys.");

    json["width"] = t_width;
    json["height"] = t_height;
    json["x"] = -1;
    json["y"] = -1;
    json["climate"] = std::string(magic_enum::enum_name(t_climateZone));

    json["layers"] = nlohmann::json::array();
    nlohmann::json t = nlohmann::json::object();
    nlohmann::json c = nlohmann::json::object();
    nlohmann::json b = nlohmann::json::object();
    t["terrain"] = t_terrainTiles;
    c["coast"] = t_coastTiles;

    std::vector<std::shared_ptr<layer::Tile>> buildingsTiles;
    buildingsTiles.resize(t_width * t_height);
    std::generate(buildingsTiles.begin(), buildingsTiles.end(), []() { return std::make_unique<layer::Tile>(); } );
    b["buildings"] = buildingsTiles;

    json["layers"].push_back(t);
    json["layers"].push_back(c);
    json["layers"].push_back(b);
}

bool mdcii::file::IslandFile::CheckJson(const nlohmann::json& t_json, const std::string& t_fileName)
{
    if (!t_json.empty() &&
        t_json.contains("width") &&
        t_json.contains("height") &&
        t_json.contains("x") &&
        t_json.contains("y") &&
        t_json.contains("climate") &&
        t_json.contains("layers")
    )
    {
        Log::MDCII_LOG_DEBUG("[IslandFile::ValidateJson()] The island Json value is valid in file {}.", t_fileName);

        return true;
    }

    Log::MDCII_LOG_WARN("[IslandFile::ValidateJson()] An invalid island Json value was found in file {}.", t_fileName);

    return false;
}

//-------------------------------------------------
// Override
//-------------------------------------------------

bool mdcii::file::IslandFile::ValidateJson() const
{
    Log::MDCII_LOG_DEBUG("[IslandFile::ValidateJson()] Check Json value in island file {}.", fileName);

    return CheckJson(json, fileName);
}

bool mdcii::file::IslandFile::ValidateObject() const
{
    Log::MDCII_LOG_DEBUG("[IslandFile::ValidateObject()] Check object values in island file {}.", fileName);

    // an island file is for multiple maps and should not store a start position
    if (json.at("x").get<int32_t>() != -1 || json.at("y").get<int32_t>() != -1 ||
        json.at("width").get<int32_t>() <= 0 || json.at("height").get<int32_t>() <= 0)
    {
        Log::MDCII_LOG_WARN("[IslandFile::ValidateObject()] Found invalid object values in island file {}.", fileName);

        return false;
    }

    Log::MDCII_LOG_DEBUG("[IslandFile::ValidateObject()] The object values in island file {} are valid.", fileName);

    return true;
}

std::string mdcii::file::IslandFile::GetFileExtension() const
{
    return ".isl";
}

std::string mdcii::file::IslandFile::GetRelPath() const
{
    return "island/";
}
