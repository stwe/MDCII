// This file is part of the MDCII project.
//
// Copyright (c) 2024. stwe <https://github.com/stwe/MDCII>
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

#include "World.h"
#include "Island.h"
#include "MousePicker.h"
#include "DeepWater.h"
#include "Gui.h"
#include "resource/TileAtlas.h"
#include "resource/AnimalsTileAtlas.h"
#include "renderer/Renderer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::World::World(state::State* t_state, const int t_width, const int t_height)
    : state{ t_state }
    , worldWidth{ t_width }
    , worldHeight{ t_height }
{
    MDCII_LOG_DEBUG("[World::World()] Create World.");

    MDCII_ASSERT(state, "[World::World()] Null pointer.")

    MDCII_LOG_DEBUG("[World::World()] The size of the world is set to ({}, {}).", worldWidth, worldHeight);

    CreateCoreObjects();
}

mdcii::world::World::World(state::State* t_state, const nlohmann::json& t_json)
    : state{ t_state }
{
    MDCII_LOG_DEBUG("[World::World()] Create World.");

    MDCII_ASSERT(state, "[World::World()] Null pointer.")

    Init(t_json);
}

mdcii::world::World::~World() noexcept
{
    MDCII_LOG_DEBUG("[World::~World()] Destruct World.");
}

//-------------------------------------------------
// Create core objects
//-------------------------------------------------

void mdcii::world::World::CreateCoreObjects()
{
    MDCII_LOG_DEBUG("[World::CreateCoreObjects()] Create core objects.");

    if (worldWidth <= 0 || worldHeight <= 0)
    {
        throw MDCII_EXCEPTION("[World::CreateCoreObjects()] Invalid world size.");
    }

    deepWater = std::make_unique<DeepWater>(this);
    camera = std::make_unique<camera::Camera>(this);
    mousePicker = std::make_unique<MousePicker>(this, true);

    tileAtlas = std::make_unique<resource::TileAtlas>(this);
    animalsTileAtlas = std::make_unique<resource::AnimalsTileAtlas>(this);
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::World::OnUserUpdate(const float t_elapsedTime)
{
    // first access
    if (m_flag)
    {
        MDCII_LOG_DEBUG("[World::OnUserUpdate()] First world update.");

        MDCII_ASSERT(deepWater->layer->currentTiles.empty(), "[World::OnUserUpdate()] Invalid number of tiles.")

        FindVisibleIslands();
        FindVisibleDeepWaterTiles();

        m_flag = false;
    }

    // on key pressed
    if (camera->OnUserUpdate(t_elapsedTime) && !m_flag)
    {
        FindVisibleIslands();
        FindVisibleDeepWaterTiles();
    }

    // render world
    HasRenderLayerOption(RenderLayer::RENDER_ALL) ?
        renderer::Renderer::RenderWorld(this, t_elapsedTime) :     // should be used in the release
        renderer::Renderer::RenderWorldParts(this, t_elapsedTime); // ability to switch layers on and off

    // update current island && tiles
    SetCurrentIslandUnderMouse();
    SetCurrentTilesUnderMouse();

    // ---------------------------------
    // todo: Rendering depends on ImGui
    // ---------------------------------

    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    // render the selected building to add it
    if (Gui::select_building.building && m_currentIslandUnderMouse.island)
    {
        auto canAdd{ false };

        for (const auto& [offsetX, offsetY] : Gui::select_building.building->CreateBuildingStartOffsets())
        {
            canAdd = m_currentIslandUnderMouse.island->CreateNewBuildingTiles(
                Gui::select_building.building,
                Gui::select_building.rotation,
                { m_currentIslandUnderMouse.position.x - offsetX, m_currentIslandUnderMouse.position.y - offsetY }
            );

            if (canAdd)
            {
                m_currentIslandUnderMouse.island->PreviewNewBuildingTiles();
                break;
            }
        }

        // left mouse button
        if (state->game->GetMouse(0).bPressed && canAdd)
        {
            m_currentIslandUnderMouse.island->AddNewBuildingTiles();
        }

        // right mouse button
        if (state->game->GetMouse(1).bHeld)
        {
            Gui::select_building.building = nullptr;
            Gui::select_building.rotation = Rotation::DEG0;
        }
    }

    mousePicker->OnUserUpdate();
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::world::World::RenderImGui()
{
    Gui::SaveGameGui(this);

    ImGui::Separator();

    if (camera->RenderImGui())
    {
        FindVisibleIslands();
        FindVisibleDeepWaterTiles();
    }

    ImGui::Begin("Tiles");

    if (m_currentIslandUnderMouse.island && m_currentTilesUnderMouse.terrainTile)
    {
        ImGui::Separator();

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
        ImGui::Text("Terrain Layer");
        ImGui::Separator();
        ImGui::PopStyleColor();

        m_currentTilesUnderMouse.terrainTile->RenderImGui(m_currentIslandUnderMouse.island->GetTerrainLayer(layer::LayerType::TERRAIN));
    }

    if (m_currentIslandUnderMouse.island && m_currentTilesUnderMouse.buildingTile)
    {
        ImGui::NewLine();

        ImGui::Separator();

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
        ImGui::Text("Building Layer");
        ImGui::Separator();
        ImGui::PopStyleColor();

        m_currentTilesUnderMouse.buildingTile->RenderImGui(m_currentIslandUnderMouse.island->GetTerrainLayer(layer::LayerType::BUILDINGS));
    }

    ImGui::End();

    mousePicker->RenderImGui();

    static auto renderCoastLayer{ false };
    static auto renderTerrainLayer{ false };
    static auto renderBuildingsLayer{ false };
    static auto renderMixedLayer{ false };
    static auto renderDeepWaterLayer{ false };
    static auto renderFiguresLayer{ false };

    static auto renderAll{ true };

    ImGui::Checkbox("Render islands terrain grid", &renderIslandsGrid);
    ImGui::Checkbox("Render deep water grid", &renderDeepWaterGrid);

    ImGui::Separator();

    if (ImGui::Checkbox("Render Coast Layer", &renderCoastLayer))
    {
        ToggleRenderLayer(RENDER_COAST_LAYER);

        DisableRenderLayer(RENDER_MIXED_LAYER, renderMixedLayer);
        DisableRenderLayer(RENDER_ALL, renderAll);

        FindVisibleIslands();
    }
    if (ImGui::Checkbox("Render Terrain Layer", &renderTerrainLayer))
    {
        ToggleRenderLayer(RENDER_TERRAIN_LAYER);

        DisableRenderLayer(RENDER_MIXED_LAYER, renderMixedLayer);
        DisableRenderLayer(RENDER_ALL, renderAll);

        FindVisibleIslands();
    }
    if (ImGui::Checkbox("Render Buildings Layer", &renderBuildingsLayer))
    {
        ToggleRenderLayer(RENDER_BUILDINGS_LAYER);

        DisableRenderLayer(RENDER_MIXED_LAYER, renderMixedLayer);
        DisableRenderLayer(RENDER_ALL, renderAll);

        FindVisibleIslands();
    }
    if (ImGui::Checkbox("Render Figures Layer", &renderFiguresLayer))
    {
        ToggleRenderLayer(RENDER_FIGURES_LAYER);

        DisableRenderLayer(RENDER_ALL, renderAll);

        FindVisibleIslands();
    }
    if (ImGui::Checkbox("Render Mixed Layer", &renderMixedLayer))
    {
        ToggleRenderLayer(RENDER_MIXED_LAYER);

        DisableRenderLayer(RENDER_COAST_LAYER, renderCoastLayer);
        DisableRenderLayer(RENDER_TERRAIN_LAYER, renderTerrainLayer);
        DisableRenderLayer(RENDER_BUILDINGS_LAYER, renderBuildingsLayer);
        DisableRenderLayer(RENDER_ALL, renderAll);

        FindVisibleIslands();
    }
    if (ImGui::Checkbox("Render Deep-Water Layer", &renderDeepWaterLayer))
    {
        ToggleRenderLayer(RENDER_DEEP_WATER_LAYER);

        DisableRenderLayer(RENDER_ALL, renderAll);

        FindVisibleDeepWaterTiles();
    }
    if (ImGui::Checkbox("Render All", &renderAll))
    {
        ToggleRenderLayer(RENDER_ALL);

        DisableRenderLayer(RENDER_COAST_LAYER, renderCoastLayer);
        DisableRenderLayer(RENDER_TERRAIN_LAYER, renderTerrainLayer);
        DisableRenderLayer(RENDER_BUILDINGS_LAYER, renderBuildingsLayer);
        DisableRenderLayer(RENDER_FIGURES_LAYER, renderFiguresLayer);
        DisableRenderLayer(RENDER_MIXED_LAYER, renderMixedLayer);

        FindVisibleIslands();
        FindVisibleDeepWaterTiles();
    }

    ImGui::Separator();

    Gui::RenderAddBuildingsGui(state->game);
}

//-------------------------------------------------
// World / screen positions
//-------------------------------------------------

olc::vi2d mdcii::world::World::ToScreen(const int t_x, const int t_y) const
{
    const auto position{ rotate_position(t_x, t_y, worldWidth, worldHeight, camera->rotation) };

    return olc::vi2d{
        camera->screenPosition.x + (position.x - position.y) * get_tile_width_half(camera->zoom),
        camera->screenPosition.y + (position.x + position.y) * get_tile_height_half(camera->zoom)
    };
}

bool mdcii::world::World::IsWorldPositionOverAnyIsland(const int t_x, const int t_y) const
{
    return std::ranges::any_of(islands, [t_x, t_y](auto const& t_island)
    {
        return t_island->IsWorldPositionOverIsland({ t_x, t_y });
    });
}

void mdcii::world::World::SetCurrentIslandUnderMouse()
{
    CurrentIsland currentIsland{};

    for (const auto& island : currentIslands)
    {
        if (const auto mouseOverIsland{ island->IsMouseOverIsland() }; mouseOverIsland.has_value())
        {
            currentIsland = { island, mouseOverIsland.value() };
            break;
        }
    }

    if (currentIsland.island)
    {
        m_currentIslandUnderMouse = currentIsland;
    }
}

void mdcii::world::World::SetCurrentTilesUnderMouse()
{
    m_currentTilesUnderMouse.terrainTile = nullptr;
    m_currentTilesUnderMouse.buildingTile = nullptr;

    if (m_currentIslandUnderMouse.island)
    {
        auto& terrainTile{ m_currentIslandUnderMouse.island->GetTerrainLayer(layer::LayerType::TERRAIN)->GetSortedTile(
                m_currentIslandUnderMouse.position.x, m_currentIslandUnderMouse.position.y, camera->rotation) };

        auto& buildingTile{ m_currentIslandUnderMouse.island->GetTerrainLayer(layer::LayerType::BUILDINGS)->GetSortedTile(
                m_currentIslandUnderMouse.position.x, m_currentIslandUnderMouse.position.y, camera->rotation) };

        if (terrainTile.HasBuilding())
        {
            m_currentTilesUnderMouse.terrainTile = &terrainTile;
        }

        if (buildingTile.HasBuilding())
        {
            m_currentTilesUnderMouse.buildingTile = &buildingTile;
        }
    }
}

bool mdcii::world::World::IsWorldPositionOutsideScreen(const int t_x, const int t_y) const
{
    const auto screenPosition{ ToScreen(t_x, t_y) };
    return screenPosition.x > Game::INI.Get<int>("window", "width") + get_tile_width(camera->zoom) ||
           screenPosition.x < -get_tile_width_half(camera->zoom) ||
           screenPosition.y > Game::INI.Get<int>("window", "height") + get_tile_height(camera->zoom) ||
           screenPosition.y < -get_tile_height_half(camera->zoom);
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::World::Init(const nlohmann::json& t_json)
{
    MDCII_LOG_DEBUG("[World::Init()] Start initializing the world ...");

    worldWidth = t_json.at("world").at("width").get<int>();
    worldHeight = t_json.at("world").at("height").get<int>();

    if (worldWidth <= 0 || worldHeight <= 0)
    {
        throw MDCII_EXCEPTION("[World::Init()] Invalid world size.");
    }

    MDCII_LOG_DEBUG("[World::Init()] The size of the world is set to ({}, {}).", worldWidth, worldHeight);

    std::vector<std::unique_ptr<Island>> newIslands;
    for (const auto& [islandKeys, islandVars] : t_json["islands"].items())
    {
        newIslands.emplace_back(std::make_unique<Island>(this, islandVars));
    }

    islands = std::move(newIslands);

    CreateCoreObjects();

    MDCII_LOG_DEBUG("[World::Init()] The world were initialized successfully.");
}

//-------------------------------------------------
// Render helper
//-------------------------------------------------

void mdcii::world::World::AddIslandToRenderList(Island* t_island)
{
    currentIslands.insert(t_island);
}

void mdcii::world::World::FindVisibleIslands()
{
    currentIslands.clear();

    for (const auto& island : islands)
    {
        if (HasRenderLayerOption(RenderLayer::RENDER_FIGURES_LAYER))
        {
            if (island->GetFiguresLayer()->UpdateCurrentTiles(island->startX, island->startY))
            {
                AddIslandToRenderList(island.get());
            }
        }

        if (HasRenderLayerOption(RenderLayer::RENDER_MIXED_LAYER))
        {
            if (island->GetTerrainLayer(layer::LayerType::MIXED)->UpdateCurrentTiles(island->startX, island->startY))
            {
                AddIslandToRenderList(island.get());
            }
        }
        else if (HasRenderLayerOption(RenderLayer::RENDER_COAST_LAYER) ||
                 HasRenderLayerOption(RenderLayer::RENDER_TERRAIN_LAYER) ||
                 HasRenderLayerOption(RenderLayer::RENDER_BUILDINGS_LAYER))
        {
            if (HasRenderLayerOption(RenderLayer::RENDER_COAST_LAYER))
            {
                if (island->GetTerrainLayer(layer::LayerType::COAST)->UpdateCurrentTiles(island->startX, island->startY))
                {
                    AddIslandToRenderList(island.get());
                }
            }
            if (HasRenderLayerOption(RenderLayer::RENDER_TERRAIN_LAYER))
            {
                if (island->GetTerrainLayer(layer::LayerType::TERRAIN)->UpdateCurrentTiles(island->startX, island->startY))
                {
                    AddIslandToRenderList(island.get());
                }
            }
            if (HasRenderLayerOption(RenderLayer::RENDER_BUILDINGS_LAYER))
            {
                if (island->GetTerrainLayer(layer::LayerType::BUILDINGS)->UpdateCurrentTiles(island->startX, island->startY))
                {
                    AddIslandToRenderList(island.get());
                }
            }
        }

        if (HasRenderLayerOption(RenderLayer::RENDER_ALL))
        {
            const auto f{ island->GetFiguresLayer()->UpdateCurrentTiles(island->startX, island->startY) };
            const auto m{ island->GetTerrainLayer(layer::LayerType::MIXED)->UpdateCurrentTiles(island->startX, island->startY) };
            if (f || m)
            {
                AddIslandToRenderList(island.get());
            }
        }
    }

    MDCII_LOG_DEBUG("[World::FindVisibleIslands()] Render {} islands.", currentIslands.size());
}

void mdcii::world::World::FindVisibleDeepWaterTiles() const
{
    deepWater->layer->UpdateCurrentTiles(0, 0);

    MDCII_LOG_DEBUG("[World::FindVisibleDeepWaterTiles()] Render {} deep water tiles.", deepWater->layer->currentTiles.size());
}

//-------------------------------------------------
//  Render layer bits
//-------------------------------------------------

void mdcii::world::World::ToggleRenderLayer(const RenderLayer t_renderLayer)
{
    renderLayer ^= t_renderLayer;
}

void mdcii::world::World::DisableRenderLayer(const RenderLayer t_renderLayer, bool& t_toFalse)
{
    renderLayer &= ~t_renderLayer;
    t_toFalse = false;
}

//-------------------------------------------------
// Serializing World into Json
//-------------------------------------------------

void mdcii::world::to_json(nlohmann::json& t_json, const World& t_world)
{
    // version
    t_json["version"] = Game::VERSION;

    // world size
    t_json["world"] = { { "width", t_world.worldWidth }, { "height", t_world.worldHeight } };

    // islands
    auto islandsJson = nlohmann::json::array();
    for (const auto& island : t_world.islands)
    {
        auto islandJson = nlohmann::json::object();
        islandJson = *island;
        islandsJson.push_back(islandJson);
    }

    t_json["islands"] = islandsJson;
}
