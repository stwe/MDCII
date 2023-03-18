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

void mdcii::file::IslandFile::AddStartPosition(const int32_t t_startMapX, const int32_t t_startMapY)
{
    AddStartPosition(json, t_startMapX, t_startMapY);
}

void mdcii::file::IslandFile::AddData(
    const int32_t t_width, const int32_t t_height,
    const std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles,
    const std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles
)
{
    AddData(json, t_width, t_height, t_terrainTiles, t_coastTiles);
}

void mdcii::file::IslandFile::AddStartPosition(nlohmann::json& t_json, const int32_t t_startMapX, const int32_t t_startMapY)
{
    t_json["x"] = t_startMapX;
    t_json["y"] = t_startMapY;
}

void mdcii::file::IslandFile::AddWidthAndHeight(nlohmann::json& t_json, const int32_t t_width, const int32_t t_height)
{
    t_json["width"] = t_width;
    t_json["height"] = t_height;
}

void mdcii::file::IslandFile::AddData(
    nlohmann::json& t_json,
    const int32_t t_width, const int32_t t_height,
    const std::vector<std::shared_ptr<layer::Tile>>& t_terrainTiles, const std::vector<std::shared_ptr<layer::Tile>>& t_coastTiles
)
{
    AddWidthAndHeight(t_json, t_width, t_height);

    t_json["layers"] = nlohmann::json::array();
    nlohmann::json t = nlohmann::json::object();
    nlohmann::json c = nlohmann::json::object();
    nlohmann::json b = nlohmann::json::object();
    t["terrain"] = t_terrainTiles;
    c["coast"] = t_coastTiles;

    std::vector<std::shared_ptr<layer::Tile>> buildingsTiles;
    buildingsTiles.resize(t_width * t_height);
    std::generate(buildingsTiles.begin(), buildingsTiles.end(), []() { return std::make_unique<layer::Tile>(); } );
    b["buildings"] = buildingsTiles;

    t_json["layers"].push_back(t);
    t_json["layers"].push_back(c);
    t_json["layers"].push_back(b);
}

//-------------------------------------------------
// Override
//-------------------------------------------------

std::string mdcii::file::IslandFile::GetFileExtension() const
{
    return ".isl";
}

std::string mdcii::file::IslandFile::GetRelPath() const
{
    return "island/";
}
