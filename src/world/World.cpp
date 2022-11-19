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

#include <glm/gtx/hash.hpp>
#include <imgui.h>
#include "World.h"
#include "Game.h"
#include "TileAtlas.h"
#include "MdciiAssert.h"
#include "MousePicker.h"
#include "WorldGui.h"
#include "eventpp/utilities/argumentadapter.h"
#include "state/State.h"
#include "state/StateStack.h"
#include "renderer/WorldRenderer.h"
#include "file/OriginalResourcesManager.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::World::World(std::string t_mapFilePath, std::shared_ptr<state::Context> t_context, const state::StateId t_stateId)
    : context{ std::move(t_context) }
    , m_stateId{ t_stateId }
    , m_mapFilePath{ std::move(t_mapFilePath) }
{
    Log::MDCII_LOG_DEBUG("[World::World()] Create World.");

    Init();
    AddListeners();
}

mdcii::world::World::~World() noexcept
{
    Log::MDCII_LOG_DEBUG("[World::~World()] Destruct World.");

    CleanUp();
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const mdcii::world::WorldLayer& mdcii::world::World::GetLayer(WorldLayerType t_layerType) const
{
    const auto& layer{ *layers.at(magic_enum::enum_integer(t_layerType)) };
    MDCII_ASSERT(layer.layerType == t_layerType, "[World::GetLayer()] Invalid layer type.")

    return layer;
}

mdcii::world::WorldLayer& mdcii::world::World::GetLayer(const WorldLayerType t_layerType)
{
    auto& layer{ *layers.at(magic_enum::enum_integer(t_layerType)) };
    MDCII_ASSERT(layer.layerType == t_layerType, "[World::GetLayer()] Invalid layer type.")

    return layer;
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::World::Render() const
{
    // terrain, buildings
    worldRenderer->Render(m_renderLayerType, zoom, rotation, *context->window, *context->camera);

    // grid
    if (m_renderGridLayer)
    {
        worldRenderer->Render(zoom, rotation, *context->window, *context->camera);
    }

    // mouse
    mousePicker->Render(*context->window, *context->camera);
}

void mdcii::world::World::RenderImGui()
{
    auto winW{ static_cast<float>(context->window->GetWidth()) };

    ImGui::SetNextWindowSize(ImVec2(290.0f, 675.0f), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Pos.x + (winW / 1.4f), 4.0f + ImGui::GetMainViewport()->Pos.y), ImGuiCond_Once);

    int windowFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;
        //ImGuiWindowFlags_NoBackground;

    ImGui::SetNextWindowBgAlpha(0.4f);

    ImGui::Begin("World", nullptr, windowFlags);

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(8, 8, 8, 255));

    ImGui::Separator();

    if (ImGui::Button("Back to main menu"))
    {
        context->stateStack->PopState(m_stateId);
        context->stateStack->PushState(state::StateId::MAIN_MENU);
    }

    ImGui::Separator();

    // select layer to render
    static int e{ magic_enum::enum_integer(m_renderLayerType) };
    ImGui::RadioButton("Terrain", &e, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Buildings", &e, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Both", &e, 2);

    if (auto layer{ magic_enum::enum_cast<WorldLayerType>(e) }; layer.has_value())
    {
        const auto l{ layer.value() };
        m_renderLayerType = l;
    }

    // toggle grid
    ImGui::Checkbox("Grid", &m_renderGridLayer);

    ImGui::Separator();

    // world gui
    m_worldGui->RotateGui();
    ImGui::Separator();

    m_worldGui->ZoomGui();
    ImGui::Separator();

    m_worldGui->ShowActionsGui();
    ImGui::Separator();

    if (currentAction == Action::BUILD)
    {
        m_statusTileIndex = -1;
        m_demolishTileIndex = -1;

        m_worldGui->ShowBuildingsGui();
    }

    if (currentAction == Action::DEMOLISH)
    {
        if (m_worldGui->selectedBuilding.HasBuilding())
        {
            m_worldGui->selectedBuilding.Reset();
        }

        if (m_demolishTileIndex >= 0)
        {
            const auto& buildingsLayer{ GetLayer(WorldLayerType::BUILDINGS) };
            auto& buildingsTile{ *buildingsLayer.tiles.at(m_demolishTileIndex) };

            // remove all building tiles from Gpu/Cpu
            if (buildingsTile.HasBuilding())
            {
                if (buildingsTile.connectedTiles.empty())
                {
                    worldRenderer->DeleteBuildingFromGpu(buildingsTile);
                    renderer::WorldRenderer::DeleteBuildingFromCpu(buildingsTile);
                }
                else
                {
                    worldRenderer->DeleteBuildingFromGpu(buildingsTile.connectedTiles);
                    worldRenderer->DeleteBuildingFromCpu(buildingsTile.connectedTiles);
                }
            }
        }

        m_demolishTileIndex = -1;
    }

    if (currentAction == Action::STATUS)
    {
        if (m_worldGui->selectedBuilding.HasBuilding())
        {
            m_worldGui->selectedBuilding.Reset();
        }

        if (m_statusTileIndex >= 0)
        {
            const auto& terrainTile{ *GetLayer(WorldLayerType::TERRAIN).tiles.at(m_statusTileIndex) };
            const auto& buildingsTile{ *GetLayer(WorldLayerType::BUILDINGS).tiles.at(m_statusTileIndex) };

            if (buildingsTile.HasBuilding())
            {
                buildingsTile.RenderImGui();
                context->originalResourcesManager->GetBuildingById(buildingsTile.buildingId).RenderImGui();
            }
            else
            {
                terrainTile.RenderImGui();
                context->originalResourcesManager->GetBuildingById(terrainTile.buildingId).RenderImGui();
            }
        }
    }

    if (currentAction == Action::OPTIONS)
    {
        m_statusTileIndex = -1;
        m_demolishTileIndex = -1;

        if (m_worldGui->selectedBuilding.HasBuilding())
        {
            m_worldGui->selectedBuilding.Reset();
        }

        m_worldGui->SaveGameGui();
    }

    ImGui::PopStyleColor();

    ImGui::End();

    // MousePicker Gui
    mousePicker->RenderImGui();
}

//-------------------------------------------------
// Rotate && Zoom
//-------------------------------------------------

void mdcii::world::World::RotateWorld(const ChangeRotation t_changeRotation)
{
    switch (t_changeRotation)
    {
    case ChangeRotation::LEFT:
        --rotation;
        break;
    case ChangeRotation::RIGHT:
        ++rotation;
        break;
    }
}

void mdcii::world::World::ZoomWorld(const ChangeZoom t_changeZoom)
{
    switch (t_changeZoom)
    {
    case ChangeZoom::ZOOM_IN:
        ++zoom;
        context->camera->zoom = zoom;
        break;
    case ChangeZoom::ZOOM_OUT:
        --zoom;
        context->camera->zoom = zoom;
        break;
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

bool mdcii::world::World::IsPositionInWorld(const int t_x, const int t_y) const
{
    if (t_x >= 0 && t_x < width &&
        t_y >= 0 && t_y < height)
    {
        return true;
    }

    return false;
}

int mdcii::world::World::GetMapIndex(const int t_x, const int t_y) const
{
    MDCII_ASSERT(IsPositionInWorld(t_x, t_y), "[World::GetMapIndex()] Invalid world position given.")

    return t_y * width + t_x;
}

int mdcii::world::World::GetMapIndex(const int t_x, const int t_y, const Rotation t_rotation) const
{
    MDCII_ASSERT(IsPositionInWorld(t_x, t_y), "[World::GetMapIndex()] Invalid world position given.")

    const auto position{ RotatePosition(t_x, t_y, t_rotation) };

    if (t_rotation == Rotation::DEG0 || t_rotation == Rotation::DEG180)
    {
        return position.y * width + position.x;
    }

    return position.y * height + position.x;
}

glm::vec2 mdcii::world::World::WorldToScreen(const int t_x, const int t_y, const Zoom t_zoom, const Rotation t_rotation) const
{
    MDCII_ASSERT(IsPositionInWorld(t_x, t_y), "[World::WorldToScreen()] Invalid world position given.")

    const auto position{ RotatePosition(t_x, t_y, t_rotation) };

    return {
        (position.x - position.y) * get_tile_width_half(t_zoom),
        (position.x + position.y) * get_tile_height_half(t_zoom)
    };
}

glm::ivec2 mdcii::world::World::RotatePosition(const int t_x, const int t_y, const Rotation t_rotation) const
{
    return rotate_position(t_x, t_y, width, height, t_rotation);
}

void mdcii::world::World::PreCalcTile(Tile& t_tile, const int t_x, const int t_y) const
{
    // set world position for Deg0
    t_tile.worldXDeg0 = t_x;
    t_tile.worldYDeg0 = t_y;

    // pre-calculate the position on the screen for each zoom and each rotation
    magic_enum::enum_for_each<Zoom>([this, &t_x, &t_y, &t_tile](const Zoom t_zoom) {
        std::array<glm::vec2, NR_OF_ROTATIONS> positions{};

        positions[0] = WorldToScreen(t_x, t_y, t_zoom, Rotation::DEG0);
        positions[1] = WorldToScreen(t_x, t_y, t_zoom, Rotation::DEG90);
        positions[2] = WorldToScreen(t_x, t_y, t_zoom, Rotation::DEG180);
        positions[3] = WorldToScreen(t_x, t_y, t_zoom, Rotation::DEG270);

        t_tile.screenPositions.at(magic_enum::enum_integer(t_zoom)) = positions;
    });

    // pre-calculate the index for each rotation for sorting
    t_tile.indices[0] = GetMapIndex(t_x, t_y, Rotation::DEG0);
    t_tile.indices[1] = GetMapIndex(t_x, t_y, Rotation::DEG90);
    t_tile.indices[2] = GetMapIndex(t_x, t_y, Rotation::DEG180);
    t_tile.indices[3] = GetMapIndex(t_x, t_y, Rotation::DEG270);

    // pre-calculate a gfx for each rotation
    if (t_tile.HasBuilding())
    {
        const auto building{ context->originalResourcesManager->GetBuildingById(t_tile.buildingId) };
        const auto gfx0{ building.gfx };

        t_tile.gfxs.push_back(gfx0);
        if (building.rotate > 0)
        {
            t_tile.gfxs.push_back(gfx0 + (1 * building.rotate));
            t_tile.gfxs.push_back(gfx0 + (2 * building.rotate));
            t_tile.gfxs.push_back(gfx0 + (3 * building.rotate));
        }
    }
}

//-------------------------------------------------
// Event handler
//-------------------------------------------------

void mdcii::world::World::OnLeftMouseButtonPressed()
{
    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    // get current mouse position
    const auto& mousePosition{ mousePicker->currentPosition };

    // status
    if (IsPositionInWorld(mousePosition.x, mousePosition.y) && currentAction == Action::STATUS)
    {
        m_statusTileIndex = GetMapIndex(mousePosition.x, mousePosition.y);
    }

    // demolish
    if (IsPositionInWorld(mousePosition.x, mousePosition.y) && currentAction == Action::DEMOLISH)
    {
        m_demolishTileIndex = GetMapIndex(mousePosition.x, mousePosition.y);
    }

    // build
    if (IsPositionInWorld(mousePosition.x, mousePosition.y) && currentAction == Action::BUILD && !m_tilesToAdd.empty())
    {
        // reset Tile pointers and replace with new Tile
        // todo: method AddToCpu
        auto& buildingsLayer{ GetLayer(WorldLayerType::BUILDINGS) };
        for (auto& tile : m_tilesToAdd)
        {
            buildingsLayer.ResetTilePointersAt(tile->instanceIds);
            buildingsLayer.StoreTile(std::move(tile));
        }

        // clear m_tilesToAdd vector
        std::vector<std::unique_ptr<Tile>>().swap(m_tilesToAdd);
    }

    MDCII_ASSERT(m_tilesToAdd.empty(), "[World::OnLeftMouseButtonPressed()] Invalid number of tiles to add.")

    // reset selected building on left mouse button clicked
    m_worldGui->selectedBuilding.Reset();
}

void mdcii::world::World::OnMouseMoved()
{
    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    // get current mouse position
    const auto& currentMousePosition{ mousePicker->currentPosition };

    // show building to create
    if (currentAction == Action::BUILD &&
        m_worldGui->selectedBuilding.HasBuilding() &&
        IsPositionInWorld(currentMousePosition.x, currentMousePosition.y) &&
        mousePicker->newTilePosition)
    {
        const auto& buildingsLayer{ GetLayer(WorldLayerType::BUILDINGS) };
        const auto& building{ context->originalResourcesManager->GetBuildingById(m_worldGui->selectedBuilding.buildingId) };

        // only add if the entire building fits on the world
        if (IsBuildingOutsideTheWorld(currentMousePosition.x, currentMousePosition.y, building, m_worldGui->selectedBuilding.rotation))
        {
            //Log::MDCII_LOG_DEBUG("[World::OnMouseMoved()] The building is outside the world on x: {}, y: {}.", currentMousePosition.x, currentMousePosition.y);
            return;
        }

        // only add it if there is no other building on the position
        if (buildingsLayer.IsAlreadyBuildingOnPosition(currentMousePosition.x, currentMousePosition.y, building, m_worldGui->selectedBuilding.rotation))
        {
            //Log::MDCII_LOG_DEBUG("[World::OnMouseMoved()] There is an other building on the position x: {}, y: {}.", currentMousePosition.x, currentMousePosition.y);
            return;
        }

        // check coast
        if (const auto it{ data::WATER_RELATED_BUILDING_IDS.find( m_worldGui->selectedBuilding.buildingId) }; it != data::WATER_RELATED_BUILDING_IDS.end())
        {
            // built on the coast
            if (!IsBuildingOnWaterOrCoast(currentMousePosition.x, currentMousePosition.y, building, m_worldGui->selectedBuilding.rotation))
            {
                //Log::MDCII_LOG_DEBUG("[World::OnMouseMoved()] The building can only be built on the coast. Position: ({}, {}).", currentMousePosition.x, currentMousePosition.y);
                return;
            }
        }
        else
        {
            // don't build on the coast
            if (IsBuildingOnWaterOrCoast(currentMousePosition.x, currentMousePosition.y, building, m_worldGui->selectedBuilding.rotation))
            {
                //Log::MDCII_LOG_DEBUG("[World::OnMouseMoved()] The building cannot be built on the coast. Position: ({}, {}).", currentMousePosition.x, currentMousePosition.y);
                return;
            }
        }

        // add (after delete) building data to Gpu
        if (!m_tilesToAdd.empty())
        {
            worldRenderer->DeleteBuildingFromGpu(m_tilesToAdd);
        }
        if (m_tilesToAdd.empty())
        {
            worldRenderer->AddBuildingToGpu(building, m_worldGui->selectedBuilding.rotation, currentMousePosition.x, currentMousePosition.y, m_tilesToAdd);
        }
    }

    if (currentAction == Action::BUILD && !IsPositionInWorld(currentMousePosition.x, currentMousePosition.y) && !m_tilesToAdd.empty())
    {
        worldRenderer->DeleteBuildingFromGpu(m_tilesToAdd);
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::World::Init()
{
    Log::MDCII_LOG_DEBUG("[World::Init()] Start initializing the world...");

    CreateTerrainAndBuildingsLayers(); // create Terrain && Buildings Layer
    PrepareRendering();                // add some pre-calculations to the tiles
    MergeTerrainAndBuildingsLayers();  // merge Terrain && Buildings Layer into a new Layer
    CreateGridLayer();                 // create Grid Layer

    tileAtlas = std::make_unique<TileAtlas>();
    worldRenderer = std::make_unique<renderer::WorldRenderer>(this);
    mousePicker = std::make_unique<MousePicker>(this, *context->window, *context->camera);
    m_worldGui = std::make_unique<WorldGui>(this);

    Log::MDCII_LOG_DEBUG("[World::Init()] The world was successfully initialized.");
}

void mdcii::world::World::AddListeners()
{
    Log::MDCII_LOG_DEBUG("[World::AddListeners()] Add listeners.");

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

void mdcii::world::World::CreateTerrainAndBuildingsLayers()
{
    MDCII_ASSERT(layers.empty(), "[World::CreateTerrainAndBuildingsLayers()] Invalid number of Layers.")

    // read file
    Log::MDCII_LOG_DEBUG("[World::CreateTerrainAndBuildingsLayers()] Starts creating Json value from file {}...", m_mapFilePath);
    nlohmann::json j = read_json_from_file(Game::RESOURCES_REL_PATH + m_mapFilePath);
    Log::MDCII_LOG_DEBUG("[World::CreateTerrainAndBuildingsLayers()] The Json value was created successfully.");

    // set width && height of the world
    for (const auto& [k, v] : j.items())
    {
        if (k == "width")
        {
            width = v.get<int>();
        }
        else if (k == "height")
        {
            height = v.get<int>();
        }
    }

    if (width < 0 || height < 0)
    {
        throw MDCII_EXCEPTION("[World::CreateTerrainAndBuildingsLayers()] Invalid width or height given.");
    }

    Log::MDCII_LOG_DEBUG("[World::CreateTerrainAndBuildingsLayers()] World width: {}", width);
    Log::MDCII_LOG_DEBUG("[World::CreateTerrainAndBuildingsLayers()] World height: {}", height);

    // create terrain && buildings layer
    Log::MDCII_LOG_DEBUG("[World::CreateTerrainAndBuildingsLayers()] Starts creating Terrain and Buildings Layer...");
    for (const auto& [k, v] : j.items())
    {
        if (k == "layers")
        {
            for (const auto& [a, o] : v.items())
            {
                auto layer{ std::make_unique<WorldLayer>(this) };

                for (const auto& [layerName, layerTiles] : o.items())
                {
                    layer->SetLayerTypeByString(layerName);
                    MDCII_ASSERT(layer->layerType != WorldLayerType::NONE, "[World::CreateTerrainAndBuildingsLayers()] Invalid Layer type.")

                    for (const auto& [i, tileJson] : layerTiles.items())
                    {
                        layer->AddTileFromJson(tileJson);
                    }
                }

                layers.emplace_back(std::move(layer));
            }
        }
    }

    MDCII_ASSERT(layers.size() == 2, "[World::CreateTerrainAndBuildingsLayers()] Invalid number of Layers.")
    MDCII_ASSERT((static_cast<size_t>(width) * static_cast<size_t>(height)) == layers.at(0)->tiles.size(), "[World::CreateTerrainAndBuildingsLayers()] Invalid number of tiles.")
    MDCII_ASSERT((static_cast<size_t>(width) * static_cast<size_t>(height)) == layers.at(1)->tiles.size(), "[World::CreateTerrainAndBuildingsLayers()] Invalid number of tiles.")

    Log::MDCII_LOG_DEBUG("[World::CreateTerrainAndBuildingsLayers()] The Terrain and Buildings Layer has been created successfully.");
}

void mdcii::world::World::PrepareRendering()
{
    MDCII_ASSERT(layers.size() == 2, "[World::PrepareRendering()] Invalid number of layers.")

    Log::MDCII_LOG_DEBUG("[World::PrepareRendering()] Prepares the Terrain and Buildings Layer for rendering.");

    auto& terrainLayer{ GetLayer(WorldLayerType::TERRAIN) };
    auto& buildingsLayer{ GetLayer(WorldLayerType::BUILDINGS) };

    // pre-calc tiles
    for (auto y{ 0 }; y < height; ++y)
    {
        for (auto x{ 0 }; x < width; ++x)
        {
            PreCalcTile(*terrainLayer.tiles.at(GetMapIndex(x, y)), x, y);
            PreCalcTile(*buildingsLayer.tiles.at(GetMapIndex(x, y)), x, y);
        }
    }

    terrainLayer.instancesToRender = static_cast<int32_t>(terrainLayer.tiles.size());
    buildingsLayer.instancesToRender = static_cast<int32_t>(buildingsLayer.tiles.size());

    terrainLayer.PrepareRendering();
    buildingsLayer.PrepareRendering();
}

void mdcii::world::World::MergeTerrainAndBuildingsLayers()
{
    Log::MDCII_LOG_DEBUG("[World::MergeTerrainAndBuildingsLayers()] Merge Terrain Layer with Buildings Layer into a new Layer.");

    MDCII_ASSERT(layers.size() == 2, "[World::MergeLayer()] Invalid number of Layers.")

    // get the existing layers
    const auto& terrainLayer{ GetLayer(WorldLayerType::TERRAIN) };
    const auto& buildingsLayer{ GetLayer(WorldLayerType::BUILDINGS) };

    // create a new layer
    auto newLayer{ std::make_unique<WorldLayer>(this) };

    // set type and number of instances
    newLayer->layerType = WorldLayerType::TERRAIN_AND_BUILDINGS;
    newLayer->instancesToRender = terrainLayer.instancesToRender;

    // copy Gpu data
    newLayer->modelMatrices = terrainLayer.modelMatrices;
    newLayer->heights = terrainLayer.heights;
    newLayer->gfxNumbers = terrainLayer.gfxNumbers;

    // merge Gpu data
    magic_enum::enum_for_each<Zoom>([&newLayer, &buildingsLayer](const Zoom t_zoom) {
        magic_enum::enum_for_each<Rotation>([&t_zoom, &newLayer, &buildingsLayer](const Rotation t_rotation) {
            const auto z{ magic_enum::enum_integer(t_zoom) };
            const auto r{ magic_enum::enum_integer(t_rotation) };

            auto& mt{ newLayer->modelMatrices.at(z).at(r) };
            const auto& mb{ buildingsLayer.modelMatrices.at(z).at(r) };

            auto& ht{ newLayer->heights.at(z) };
            const auto& hb{ buildingsLayer.heights.at(z) };

            auto& gt{ newLayer->gfxNumbers.at(z) };
            const auto& gb{ buildingsLayer.gfxNumbers.at(z) };

            // for each tile
            auto i{ 0 };
            for (const auto& mapTile : buildingsLayer.sortedTiles.at(r))
            {
                if (mapTile->HasBuilding())
                {
                    mt.at(i) = mb.at(i);
                    ht.at(i)[r] = hb.at(i)[r];
                    gt.at(i)[r] = gb.at(i)[r];
                }

                i++;
            }
        });
    });

    // store new layer
    layers.emplace_back(std::move(newLayer));

    MDCII_ASSERT(layers.size() == 3, "[World::MergeLayer()] Invalid number of Layers.")
}

void mdcii::world::World::CreateGridLayer()
{
    Log::MDCII_LOG_DEBUG("[World::CreateGridLayer()] Starts creating Grid Layer...");

    MDCII_ASSERT(layers.size() == 3, "[World::CreateGridLayer()] Invalid number of Layers.")

    // get the Terrain Layer
    const auto& terrainLayer{ GetLayer(WorldLayerType::TERRAIN) };

    // create a new Layer
    auto layer{ std::make_unique<WorldLayer>(this) };
    layer->layerType = WorldLayerType::GRID;
    layer->instancesToRender = terrainLayer.instancesToRender;
    layer->modelMatrices = terrainLayer.modelMatrices;
    layers.emplace_back(std::move(layer));

    MDCII_ASSERT(layers.size() == 4, "[World::CreateGridLayer()] Invalid number of Layers.")

    Log::MDCII_LOG_DEBUG("[World::CreateGridLayer()] The Grid Layer has been created successfully.");
}

//-------------------------------------------------
// Add building
//-------------------------------------------------

bool mdcii::world::World::IsBuildingOutsideTheWorld(const int t_x, const int t_y, const data::Building& t_building, const Rotation t_buildingRotation) const
{
    for (auto y{ 0 }; y < t_building.size.h; ++y)
    {
        for (auto x{ 0 }; x < t_building.size.w; ++x)
        {
            auto rp{ world::rotate_position(x, y, t_building.size.h, t_building.size.w, t_buildingRotation) };
            if (t_buildingRotation == world::Rotation::DEG0 || t_buildingRotation == world::Rotation::DEG180)
            {
                rp = world::rotate_position(x, y, t_building.size.w, t_building.size.h, t_buildingRotation);
            }

            if (!IsPositionInWorld(t_x + rp.x, t_y + rp.y))
            {
                return true;
            }
        }
    }

    return false;
}

bool mdcii::world::World::IsBuildingOnWaterOrCoast(const int t_x, const int t_y, const data::Building& t_building, const Rotation t_buildingRotation) const
{
    for (auto y{ 0 }; y < t_building.size.h; ++y)
    {
        for (auto x{ 0 }; x < t_building.size.w; ++x)
        {
            auto rp{ world::rotate_position(x, y, t_building.size.h, t_building.size.w, t_buildingRotation) };
            if (t_buildingRotation == world::Rotation::DEG0 || t_buildingRotation == world::Rotation::DEG180)
            {
                rp = world::rotate_position(x, y, t_building.size.w, t_building.size.h, t_buildingRotation);
            }

            const auto& terrainTile{ GetLayer(WorldLayerType::TERRAIN).GetTile(t_x + rp.x, t_y + rp.y) };
            if (context->originalResourcesManager->GetBuildingById(terrainTile.buildingId).posoffs == 0)
            {
                return true;
            }
        }
    }

    return false;
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::world::World::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[World::CleanUp()] CleanUp World.");

    event::EventManager::event_dispatcher.removeListener(event::MdciiEventType::MOUSE_BUTTON_PRESSED, m_mouseButtonPressed);
    event::EventManager::event_dispatcher.removeListener(event::MdciiEventType::MOUSE_MOVED, m_mouseMoved);
}
