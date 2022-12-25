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
#include "World.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "Terrain.h"
#include "TileAtlas.h"
#include "WorldGui.h"
#include "MousePicker.h"
#include "eventpp/utilities/argumentadapter.h"
#include "state/State.h"
#include "state/StateStack.h"
#include "renderer/TerrainRenderer.h"
#include "renderer/GridRenderer.h"
#include "world/Island.h"
#include "layer/GridLayer.h"
#include "layer/WorldLayer.h"
#include "layer/WorldGridLayer.h"
#include "file/OriginalResourcesManager.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::World::World(std::string t_mapFilePath, std::shared_ptr<state::Context> t_context, const state::StateId t_stateId)
    : context{ std::move(t_context) }
    , m_stateId{ t_stateId }
    , m_mapFilePath{ std::move(t_mapFilePath) }
{
    Log::MDCII_LOG_DEBUG("[World::World()] Create World from {}.", m_mapFilePath);

    MDCII_ASSERT(!m_mapFilePath.empty(), "[World::World()] Invalid path given.")
    MDCII_ASSERT(context, "[World::World()] Null pointer.")

    rotation = Rotation::DEG0;
    zoom = Zoom::GFX;
    m_layerType = layer::LayerType::ALL;

    Init();
    AddListeners();
}

mdcii::world::World::~World() noexcept
{
    Log::MDCII_LOG_DEBUG("[World::~World()] Destruct World.");

    CleanUp();
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

bool mdcii::world::World::IsPositionInWorld(const int32_t t_x, const int32_t t_y) const
{
    if (t_x >= 0 && t_x < width && t_y >= 0 && t_y < height)
    {
        return true;
    }

    return false;
}

bool mdcii::world::World::IsPositionInWorld(const glm::ivec2& t_position) const
{
    return IsPositionInWorld(t_position.x, t_position.y);
}

glm::vec2 mdcii::world::World::WorldToScreen(const int32_t t_x, const int32_t t_y, const Zoom t_zoom, const Rotation t_rotation) const
{
    const auto position{ rotate_position(t_x, t_y, width, height, t_rotation) };
    return {
        (position.x - position.y) * get_tile_width_half(t_zoom),
        (position.x + position.y) * get_tile_height_half(t_zoom)
    };
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::World::Update() const
{
    if (m_runAnimations)
    {
        terrainRenderer->Update();
    }
}

void mdcii::world::World::Render() const
{
    for(const auto& island : terrain->islands)
    {
        if (m_layerType != layer::LayerType::NOTHING)
        {
            if (m_layerType == layer::LayerType::COAST)
            {
                terrainRenderer->Render(*island->coastLayer, zoom, rotation);
            }

            if (m_layerType == layer::LayerType::TERRAIN)
            {
                terrainRenderer->Render(*island->terrainLayer, zoom, rotation);
            }

            if (m_layerType == layer::LayerType::BUILDINGS)
            {
                terrainRenderer->Render(*island->buildingsLayer, zoom, rotation);
            }

            if (m_layerType == layer::LayerType::MIXED)
            {
                terrainRenderer->Render(*island->mixedLayer, zoom, rotation);
            }

            if (m_layerType == layer::LayerType::ALL)
            {
                terrainRenderer->Render(*island->coastLayer, zoom, rotation);
                terrainRenderer->Render(*island->mixedLayer, zoom, rotation);
            }
        }

        if (m_renderIslandGridLayers)
        {
            gridRenderer->Render(island->gridLayer->modelMatricesSsbos, island->gridLayer->instancesToRender, zoom, rotation);
        }
    }

    if (m_renderWorldLayer)
    {
        terrainRenderer->Render(
            worldLayer->modelMatricesSsbos,
            *worldLayer->gfxNumbersSsbo,
            *worldLayer->buildingIdsSsbo,
            worldLayer->instancesToRender,
            zoom,
            rotation
        );
    }

    if (m_renderWorldGridLayer)
    {
        gridRenderer->Render(worldGridLayer->modelMatricesSsbos, worldGridLayer->instancesToRender, zoom, rotation);
    }

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

    ImGui::SetNextWindowBgAlpha(0.8f);

    ImGui::Begin("World", nullptr, windowFlags);

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 230, 230, 255));

    ImGui::Separator();
    if (ImGui::Button("Back to main menu"))
    {
        context->stateStack->PopState(m_stateId);
        context->stateStack->PushState(state::StateId::MAIN_MENU);
    }
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Render islands"))
    {
        static int e{ magic_enum::enum_integer(m_layerType) };
        ImGui::RadioButton("Coast", &e, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Terrain", &e, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Buildings", &e, 2);
        ImGui::RadioButton("Mixed", &e, 3);
        ImGui::SameLine();
        ImGui::RadioButton("All", &e, 4);
        ImGui::SameLine();
        ImGui::RadioButton("Nothing", &e, 5);

        ImGui::Checkbox("Island Grids", &m_renderIslandGridLayers);

        if (auto layer{ magic_enum::enum_cast<layer::LayerType>(e) }; layer.has_value())
        {
            const auto l{ layer.value() };
            m_layerType = l;
        }
    }

    if (ImGui::CollapsingHeader("Render World"))
    {
        ImGui::Checkbox("World Deep Water", &m_renderWorldLayer);
        ImGui::Checkbox("World Grid", &m_renderWorldGridLayer);
        ImGui::Checkbox("Animations", &m_runAnimations);
    }

    if (ImGui::CollapsingHeader("Rotate"))
    {
        m_worldGui->RotateGui();
    }

    if (ImGui::CollapsingHeader("Zoom"))
    {
        m_worldGui->ZoomGui();
    }

    if (ImGui::CollapsingHeader("Change action"))
    {
        m_worldGui->ShowActionsGui();
    }

    if (currentAction == Action::BUILD)
    {
        if (ImGui::CollapsingHeader("Buildings"))
        {
            m_demolishTileIndex = -1;

            m_worldGui->ShowBuildingsGui();
        }
    }

    if (currentAction == Action::DEMOLISH)
    {
        if (m_worldGui->selectedBuilding.HasBuilding())
        {
            m_worldGui->selectedBuilding.Reset();
        }

        if (m_demolishTileIndex >= 0)
        {
            /*
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
            */
        }

        m_demolishTileIndex = -1;
    }

    if (currentAction == Action::STATUS)
    {
        if (m_worldGui->selectedBuilding.HasBuilding())
        {
            m_worldGui->selectedBuilding.Reset();
        }

        if (terrain->currentIsland)
        {
            terrain->currentIsland->RenderImGui();

            const auto& terrainTile{ terrain->currentIsland->GetTerrainTileFromCurrentPosition() };
            const auto& buildingsTile{ terrain->currentIsland->GetBuildingTileFromCurrentPosition() };
            const auto& coastTile{ terrain->currentIsland->GetCoastTileFromCurrentPosition() };

            if (buildingsTile.HasBuilding())
            {
                buildingsTile.RenderImGui();
                context->originalResourcesManager->GetBuildingById(buildingsTile.buildingId).RenderImGui();
            }
            else if (terrainTile.HasBuilding())
            {
                terrainTile.RenderImGui();
                context->originalResourcesManager->GetBuildingById(terrainTile.buildingId).RenderImGui();
            }
            else if (coastTile.HasBuilding())
            {
                coastTile.RenderImGui();
                context->originalResourcesManager->GetBuildingById(coastTile.buildingId).RenderImGui();
            }
            else
            {
                Log::MDCII_LOG_WARN("[World::RenderImGui()] Missing Building-Id");
            }
        }
    }

    if (currentAction == Action::OPTIONS)
    {
        m_demolishTileIndex = -1;

        if (m_worldGui->selectedBuilding.HasBuilding())
        {
            m_worldGui->selectedBuilding.Reset();
        }

        m_worldGui->SaveGameGui();
    }

    ImGui::PopStyleColor();

    ImGui::End();

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
// Event handler
//-------------------------------------------------

void mdcii::world::World::OnLeftMouseButtonPressed()
{
    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    //Log::MDCII_LOG_DEBUG("[World::OnLeftMouseButtonPressed()] Left mouse button event handler.");
}

void mdcii::world::World::OnMouseMoved()
{
    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    //Log::MDCII_LOG_DEBUG("[World::OnMouseMoved()] Mouse moved event handler.");
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::World::Init()
{
    Log::MDCII_LOG_DEBUG("[World::Init()] Start initializing the world...");

    terrain = std::make_unique<Terrain>(context, this);
    tileAtlas = std::make_unique<TileAtlas>();
    terrainRenderer = std::make_unique<renderer::TerrainRenderer>(context, tileAtlas);

    nlohmann::json j = read_json_from_file(Game::RESOURCES_REL_PATH + m_mapFilePath);
    for (const auto& [k, v] : j.items())
    {
        if (k == "world")
        {
            width = v.at("width").get<int32_t>();
            height = v.at("height").get<int32_t>();

            if (width < WORLD_MIN_WIDTH || width > WORLD_MAX_WIDTH)
            {
                throw MDCII_EXCEPTION("[World::Init()] Invalid world width given.");
            }

            if (height < WORLD_MIN_HEIGHT || height > WORLD_MAX_HEIGHT)
            {
                throw MDCII_EXCEPTION("[World::Init()] Invalid world height given.");
            }

            Log::MDCII_LOG_DEBUG("[World::Init()] The width of the world is set to: {}.", width);
            Log::MDCII_LOG_DEBUG("[World::Init()] The height of the world is set to: {}.", height);
        }
        if (k == "islands")
        {
            terrain->CreateIslandsFromJson(v);
        }
    }

    worldLayer = std::make_unique<layer::WorldLayer>(context, this);
    worldLayer->PrepareCpuDataForRendering();
    worldLayer->PrepareGpuDataForRendering();

    worldGridLayer = std::make_unique<layer::WorldGridLayer>(context, this);
    worldGridLayer->PrepareCpuDataForRendering();
    worldGridLayer->PrepareGpuDataForRendering();

    gridRenderer = std::make_unique<renderer::GridRenderer>(context);
    m_worldGui = std::make_unique<WorldGui>(this);
    mousePicker = std::make_unique<MousePicker>(this, *context->window, *context->camera);

    MDCII_ASSERT(!terrain->islands.empty(), "[World::Init()] No islands created.")

    Log::MDCII_LOG_DEBUG("[World::Init()] The world was successfully initialized.");
}

void mdcii::world::World::AddListeners()
{
    Log::MDCII_LOG_DEBUG("[World::AddListeners()] Add event listeners.");

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

void mdcii::world::World::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[World::CleanUp()] CleanUp World.");

    event::EventManager::event_dispatcher.removeListener(event::MdciiEventType::MOUSE_BUTTON_PRESSED, m_mouseButtonPressed);
    event::EventManager::event_dispatcher.removeListener(event::MdciiEventType::MOUSE_MOVED, m_mouseMoved);
}
