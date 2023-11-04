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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#include "MdciiFile.h"
#include "MdciiAssert.h"
#include "MdciiUtils.h"
#include "world/Island.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::resource::MdciiFile::MdciiFile(std::string t_fileName)
    : fileName{ std::move(t_fileName) }
{
    MDCII_LOG_DEBUG("[MdciiFile::MdciiFile()] Create MdciiFile.");

    MDCII_ASSERT(!fileName.empty(), "[MdciiFile::MdciiFile()] Invalid file name.")
}

mdcii::resource::MdciiFile::~MdciiFile() noexcept
{
    MDCII_LOG_DEBUG("[MdciiFile::~MdciiFile()] Destruct MdciiFile.");
}

//-------------------------------------------------
// Save && load
//-------------------------------------------------

bool mdcii::resource::MdciiFile::LoadJsonFromFile()
{
    MDCII_LOG_DEBUG("[MdciiFile::LoadJsonFromFile()] Start loading Json value from file {}.", fileName);

    if (std::filesystem::exists(fileName))
    {
        json = read_json_from_file(fileName);
        MDCII_LOG_DEBUG("[MdciiFile::LoadJsonFromFile()] The Json value was successfully read from file {}.", fileName);

        return true;
    }

    MDCII_LOG_WARN("[MdciiFile::LoadJsonFromFile()] File {} does not exist.", fileName);

    return false;
}

std::vector<std::unique_ptr<mdcii::world::Island>> mdcii::resource::MdciiFile::CreateWorldContent(int& t_worldWidth, int& t_worldHeight) const
{
    std::vector<std::unique_ptr<world::Island>> is;

    t_worldWidth = json.at("world").at("width").get<int32_t>();
    t_worldHeight = json.at("world").at("height").get<int32_t>();

    for (const auto& [islandKeys, islandVars] : json["islands"].items())
    {
        auto island{ std::make_unique<world::Island>() };
        island->width = islandVars.at("width").get<int32_t>();
        island->height = islandVars.at("height").get<int32_t>();
        island->startX = islandVars.at("x").get<int32_t>();
        island->startY = islandVars.at("y").get<int32_t>();

        for (auto layers = islandVars.at("layers").items(); const auto& [k, v] : layers)
        {
            for (const auto& [layerNameJson, layerTilesJson] : v.items())
            {
                if (layerNameJson == "terrain")
                {
                    for (const auto& [k2, t] : layerTilesJson.items())
                    {
                        world::Tile tile;

                        if (t.count("id"))
                        {
                            tile.buildingId = t.at("id");
                        }

                        if (t.count("rotation"))
                        {
                            tile.rotation = t.at("rotation");
                        }

                        if (t.count("x"))
                        {
                            tile.x = t.at("x");
                        }

                        if (t.count("y"))
                        {
                            tile.y = t.at("y");
                        }

                        island->tiles.push_back(tile);
                    }

                }
            }
        }

        is.push_back(std::move(island));
    }

    return is;
}
