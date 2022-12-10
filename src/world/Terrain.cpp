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

#include "Terrain.h"
#include "MdciiAssert.h"
#include "Island.h"
#include "state/State.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::Terrain::Terrain(std::shared_ptr<state::Context> t_context)
    : m_context{ std::move(t_context) }
{
    Log::MDCII_LOG_DEBUG("[Terrain::Terrain()] Create Terrain.");

    MDCII_ASSERT(m_context, "[Terrain::Terrain()] Null pointer.")
}

mdcii::world::Terrain::~Terrain() noexcept
{
    Log::MDCII_LOG_DEBUG("[Terrain::~Terrain()] Destruct Terrain.");

    CleanUp();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::Terrain::CreateIslandsFromJson(const nlohmann::json& t_json)
{
    Log::MDCII_LOG_DEBUG("[Terrain::CreateIslandsFromJson()] Start creating islands...");

    for (const auto& [k, v] : t_json.items())
    {
        auto island{ std::make_unique<Island>(m_context) };
        island->InitValuesFromJson(v);

        islands.emplace_back(std::move(island));
    }

    MDCII_ASSERT(!islands.empty(), "[Terrain::CreateIslandsFromJson()] Missing islands.")

    Log::MDCII_LOG_DEBUG("[Terrain::CreateIslandsFromJson()] The islands have been created successfully.");
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::world::Terrain::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[Terrain::CleanUp()] CleanUp Terrain.");
}
