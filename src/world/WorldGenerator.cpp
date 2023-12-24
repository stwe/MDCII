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
#include "World.h"
#include "MdciiUtils.h"
#include "state/State.h"
#include "resource/MdciiResourcesManager.h"
#include "resource/MdciiFile.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::WorldGenerator::WorldGenerator(state::State* t_state)
    : m_state{ t_state }
{
    MDCII_LOG_DEBUG("[WorldGenerator::WorldGenerator()] Create WorldGenerator.");

    MDCII_ASSERT(m_state, "[WorldGenerator::WorldGenerator()] Null pointer.")

    world = std::make_unique<world::World>(m_state);
    world->worldWidth = 24;
    world->worldHeight = 24;
    world->CreateCoreObjects();
}

mdcii::world::WorldGenerator::~WorldGenerator() noexcept
{
    MDCII_LOG_DEBUG("[WorldGenerator::~WorldGenerator()] Destruct WorldGenerator.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::WorldGenerator::OnUserUpdate(const float t_elapsedTime)
{
    if (m_state->game->mdciiResourcesManager->islandFiles.empty())
    {
        ImGui::Text("MissingFiles");
    }
    else
    {
        m_island_file_index = render_file_chooser(m_state->game->mdciiResourcesManager->islandFiles);
    }

    if (m_island_file_index >= 0)
    {
        const auto& fileName{ m_state->game->mdciiResourcesManager->islandFiles.at(m_island_file_index) };

        MDCII_LOG_DEBUG("[WorldGenerator::OnUserUpdate()] Add island {}.", m_state->game->mdciiResourcesManager->islandFiles.at(m_island_file_index));

        if (resource::MdciiFile mdciiFile{ fileName }; mdciiFile.SetJsonFromFile())
        {
            // todo
        }
    }
}

//-------------------------------------------------
// Json
//-------------------------------------------------

void mdcii::world::to_json(nlohmann::json& t_json, const WorldGenerator& t_worldGenerator)
{
    // version
    t_json["version"] = Game::VERSION;

    // world
    t_json["world"] = { { "width", t_worldGenerator.world->worldWidth }, { "height", t_worldGenerator.world->worldHeight } };

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
