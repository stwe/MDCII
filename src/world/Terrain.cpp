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

#include <imgui.h>
#include "Terrain.h"
#include "World.h"
#include "MdciiAssert.h"
#include "Island.h"
#include "MousePicker.h"
#include "state/State.h"
#include "eventpp/utilities/argumentadapter.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::Terrain::Terrain(std::shared_ptr<state::Context> t_context, World* t_world)
    : world{ t_world }
    , m_context{ std::move(t_context) }
{
    Log::MDCII_LOG_DEBUG("[Terrain::Terrain()] Create Terrain.");

    MDCII_ASSERT(world, "[Terrain::Terrain()] Null pointer.")
    MDCII_ASSERT(m_context, "[Terrain::Terrain()] Null pointer.")

    AddListeners();
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
        auto island{ std::make_unique<Island>(m_context, this) };
        island->InitValuesFromJson(v);

        islands.emplace_back(std::move(island));
    }

    MDCII_ASSERT(!islands.empty(), "[Terrain::CreateIslandsFromJson()] Missing islands.")

    Log::MDCII_LOG_DEBUG("[Terrain::CreateIslandsFromJson()] {} islands have been created successfully.", islands.size());
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

bool mdcii::world::Terrain::IsPositionOnAnIsland(const glm::ivec2& t_position) const
{
    for (const auto& island : islands)
    {
        if (island->IsWorldPositionInAabb(t_position))
        {
            return true;
        }
    }

    return false;
}

//-------------------------------------------------
// Event handler
//-------------------------------------------------

void mdcii::world::Terrain::OnLeftMouseButtonPressed()
{
    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    currentIsland = nullptr;
    for (const auto& island : islands)
    {
        if (island->IsWorldPositionInAabb(world->mousePicker->currentPosition))
        {
            currentIsland = island.get();
            Log::MDCII_LOG_DEBUG("[Terrain::OnLeftMouseButtonPressed()] New Island selected. The selected Island starts on world {}, {}.", island->worldX, island->worldY);

            island->currentPosition = island->GetIslandPositionFromWorldPosition(world->mousePicker->currentPosition);
            Log::MDCII_LOG_DEBUG("[Terrain::OnLeftMouseButtonPressed()] The Island is selected on {}, {}.", island->currentPosition.x, island->currentPosition.y);
        }
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::Terrain::AddListeners()
{
    Log::MDCII_LOG_DEBUG("[Terrain::AddListeners()] Add event listeners.");

    // OnLeftMouseButtonPressed
    m_mouseButtonPressed = event::EventManager::event_dispatcher.appendListener(
        event::MdciiEventType::MOUSE_BUTTON_PRESSED,
        eventpp::argumentAdapter<void(const event::MouseButtonPressedEvent&)>(
            [this](const event::MouseButtonPressedEvent& t_event) {
                if (t_event.button == 0)
                {
                    OnLeftMouseButtonPressed();
                }
            }
        )
    );
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::world::Terrain::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[Terrain::CleanUp()] CleanUp Terrain.");

    event::EventManager::event_dispatcher.removeListener(event::MdciiEventType::MOUSE_BUTTON_PRESSED, m_mouseButtonPressed);
}
