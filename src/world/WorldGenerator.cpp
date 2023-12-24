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

#include "WorldGenerator.h"
#include "Game.h"
#include "MdciiAssert.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::WorldGenerator::WorldGenerator(Game* t_game)
    : m_game{ t_game }
{
    MDCII_LOG_DEBUG("[WorldGenerator::WorldGenerator()] Create WorldGenerator.");

    MDCII_ASSERT(m_game, "[WorldGenerator::WorldGenerator()] Null pointer.")
}

mdcii::world::WorldGenerator::~WorldGenerator() noexcept
{
    MDCII_LOG_DEBUG("[WorldGenerator::~WorldGenerator()] Destruct WorldGenerator.");
}

//-------------------------------------------------
// Json
//-------------------------------------------------

void mdcii::world::to_json(nlohmann::json& t_json, const WorldGenerator& t_worldGenerator)
{
    // version
    t_json["version"] = Game::VERSION;

    // world
    //t_json["world"] = { { "width", t_generatorWorld.width }, { "height", t_generatorWorld.height } };

    // islands
    auto islandsJson = nlohmann::json::array();
    /*
    for (const auto& island : t_generatorWorld.terrain->islands)
    {
        auto islandJson = nlohmann::json::object();
        islandJson = *island;
        islandsJson.push_back(islandJson);
    }
    */

    t_json["islands"] = islandsJson;
}
