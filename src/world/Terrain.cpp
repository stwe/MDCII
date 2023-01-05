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
#include "layer/TerrainLayer.h"
#include "file/OriginalResourcesManager.h"

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

bool mdcii::world::Terrain::IsWorldPositionInDeepWater(const int32_t t_x, const int32_t t_y) const
{
    for (const auto& island : islands)
    {
        if (island->IsWorldPositionInAabb(glm::ivec2(t_x, t_y)))
        {
            return false;
        }
    }

    return true;
}

bool mdcii::world::Terrain::IsBuildableOnIslandUnderMouse(const glm::ivec2& t_startWorldPosition, const data::Building& t_building, const Rotation t_buildingRotation) const
{
    if (!currentIslandUnderMouse)
    {
        return false;
    }

    for (auto y{ 0 }; y < t_building.size.h; ++y)
    {
        for (auto x{ 0 }; x < t_building.size.w; ++x)
        {
            // rotate building position
            auto rp{ world::rotate_position(x, y, t_building.size.h, t_building.size.w, t_buildingRotation) };
            if (t_buildingRotation == world::Rotation::DEG0 || t_buildingRotation == world::Rotation::DEG180)
            {
                rp = world::rotate_position(x, y, t_building.size.w, t_building.size.h, t_buildingRotation);
            }

            // calc final world position
            const auto finalWorldPosition{ glm::ivec2(t_startWorldPosition.x + rp.x, t_startWorldPosition.y + rp.y) };

            // is final world position on island
            if (currentIslandUnderMouse->IsWorldPositionInAabb(finalWorldPosition))
            {
                // get position on island from world position
                const auto islandPosition{ currentIslandUnderMouse->GetIslandPositionFromWorldPosition(finalWorldPosition) };

                // get tiles
                const auto& terrainTile{ currentIslandUnderMouse->terrainLayer->GetTile(islandPosition.x, islandPosition.y) };
                const auto& buildingTile{ currentIslandUnderMouse->buildingsLayer->GetTile(islandPosition.x, islandPosition.y) };

                // check if the tile is buildable
                if (!terrainTile.HasBuilding() || buildingTile.HasBuilding() || m_context->originalResourcesManager->GetBuildingById(terrainTile.buildingId).posoffs == 0)
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }

    Log::MDCII_LOG_DEBUG("[Terrain::IsBuildableOnIslandUnderMouse()] The tile is buildable at world position ({}, {}).", t_startWorldPosition.x, t_startWorldPosition.y);

    return true;
}

bool mdcii::world::Terrain::IsCurrentSelectedTileRemovable() const
{
    return currentSelectedIsland &&
        currentSelectedIsland->currentSelectedTile &&
        currentSelectedIsland->currentSelectedTile->HasBuilding() &&
        currentSelectedIsland->currentSelectedTile->layerType == layer::LayerType::BUILDINGS;
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::Terrain::RenderImGui() const
{
    if (currentSelectedIsland)
    {
        ImGui::Separator();
        ImGui::Text("Selected Island");
        ImGui::Separator();
        currentSelectedIsland->RenderImGui();
    }

    if (currentIslandUnderMouse)
    {
        ImGui::Separator();
        ImGui::Text("Island under mouse");
        ImGui::Separator();
        currentIslandUnderMouse->RenderImGui();
    }
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

    currentSelectedIsland = nullptr;
    for (auto const& island : islands)
    {
        if (island->IsWorldPositionInAabb(world->mousePicker->currentPosition))
        {
            currentSelectedIsland = island.get();
            currentSelectedIsland->currentSelectedTile = nullptr;

            auto& terrainTile{ currentSelectedIsland->terrainLayer->GetTile(currentSelectedIsland->GetIslandPositionFromWorldPosition(world->mousePicker->currentPosition)) };
            auto& buildingsTile{ currentSelectedIsland->buildingsLayer->GetTile(currentSelectedIsland->GetIslandPositionFromWorldPosition(world->mousePicker->currentPosition)) };
            auto& coastTile{ currentSelectedIsland->coastLayer->GetTile(currentSelectedIsland->GetIslandPositionFromWorldPosition(world->mousePicker->currentPosition)) };

            if (buildingsTile.HasBuilding())
            {
                currentSelectedIsland->currentSelectedTile = &buildingsTile;
            }
            else if (terrainTile.HasBuilding())
            {
                currentSelectedIsland->currentSelectedTile = &terrainTile;
            }
            else if (coastTile.HasBuilding())
            {
                currentSelectedIsland->currentSelectedTile = &coastTile;
            }
        }
    }
}

void mdcii::world::Terrain::OnMouseMoved()
{
    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    currentIslandUnderMouse = nullptr;
    for (const auto& island : islands)
    {
        if (island->IsWorldPositionInAabb(world->mousePicker->currentPosition))
        {
            currentIslandUnderMouse = island.get();
            currentIslandUnderMouse->currentTileUnderMouse = nullptr;

            auto& terrainTile{ currentIslandUnderMouse->terrainLayer->GetTile(currentIslandUnderMouse->GetIslandPositionFromWorldPosition(world->mousePicker->currentPosition)) };
            auto& buildingsTile{ currentIslandUnderMouse->buildingsLayer->GetTile(currentIslandUnderMouse->GetIslandPositionFromWorldPosition(world->mousePicker->currentPosition)) };
            auto& coastTile{ currentIslandUnderMouse->coastLayer->GetTile(currentIslandUnderMouse->GetIslandPositionFromWorldPosition(world->mousePicker->currentPosition)) };

            if (buildingsTile.HasBuilding())
            {
                currentIslandUnderMouse->currentTileUnderMouse = &buildingsTile;
            }
            else if (terrainTile.HasBuilding())
            {
                currentIslandUnderMouse->currentTileUnderMouse = &terrainTile;
            }
            else if (coastTile.HasBuilding())
            {
                currentIslandUnderMouse->currentTileUnderMouse = &coastTile;
            }
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

    // OnMouseMoved
    m_mouseMoved = event::EventManager::event_dispatcher.appendListener(
        event::MdciiEventType::MOUSE_MOVED,
        eventpp::argumentAdapter<void(const event::MouseMovedEvent&)>(
            [this]([[maybe_unused]] const event::MouseMovedEvent& t_event) {
                OnMouseMoved();
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
    event::EventManager::event_dispatcher.removeListener(event::MdciiEventType::MOUSE_MOVED, m_mouseMoved);
}
