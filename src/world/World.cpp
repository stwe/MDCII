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

#include "World.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "TileAtlas.h"
#include "WorldGui.h"
#include "MousePicker.h"
#include "eventpp/utilities/argumentadapter.h"
#include "state/State.h"
#include "state/StateStack.h"
#include "renderer/TerrainRenderer.h"
#include "renderer/GridRenderer.h"
#include "layer/GridLayer.h"
#include "layer/WorldLayer.h"
#include "layer/WorldGridLayer.h"

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
    m_layerTypeToRender = layer::LayerType::ALL;

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
        if (context->camera->IsIslandNotInCamera(zoom, rotation, *island))
        {
            continue;
        }

        if (m_layerTypeToRender != layer::LayerType::NOTHING)
        {
            if (m_layerTypeToRender == layer::LayerType::COAST)
            {
                terrainRenderer->Render(*island->coastLayer, zoom, rotation);
            }

            if (m_layerTypeToRender == layer::LayerType::TERRAIN)
            {
                terrainRenderer->Render(*island->terrainLayer, zoom, rotation);
            }

            if (m_layerTypeToRender == layer::LayerType::BUILDINGS)
            {
                terrainRenderer->Render(*island->buildingsLayer, zoom, rotation);
            }

            if (m_layerTypeToRender == layer::LayerType::MIXED)
            {
                terrainRenderer->Render(*island->mixedLayer, zoom, rotation);
            }

            if (m_layerTypeToRender == layer::LayerType::ALL)
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
    auto winW{ static_cast<float>(context->window->width) };

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
    m_worldGui->ShowActionsGui();
    ImGui::Separator();

    ImGui::Checkbox("Select on terrain", &mousePicker->calcForIslandTerrain);
    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(66, 104, 188, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::SetTooltip("%s", "This must be enabled to select buildings on the island.");
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Islands"))
    {
        static int e{ magic_enum::enum_integer(m_layerTypeToRender) };
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
            m_layerTypeToRender = l;
        }

        terrain->RenderImGui(); // render selected island && island under mouse
    }

    if (ImGui::CollapsingHeader("World"))
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

    if (ImGui::CollapsingHeader("Camera"))
    {
        context->camera->RenderImGui();
    }

    if (ImGui::CollapsingHeader("Culling"))
    {
        for (const auto& island : terrain->islands)
        {
            ImGui::Text("Island (%d, %d)", island->startWorldX, island->startWorldY);
            ImGui::SameLine();
            ImGui::Text(" rendered: %s", context->camera->IsIslandNotInCamera(zoom, rotation, *island) ? "no" : "yes");
        }
    }

    if (currentAction == Action::BUILD && ImGui::CollapsingHeader("Buildings"))
    {
        m_worldGui->ShowBuildingsGui();
    }

    if (currentAction == Action::DEMOLISH && terrain->IsCurrentSelectedTileRemovable())
    {
        if (terrain->currentSelectedIsland->currentSelectedTile->connectedTiles.empty())
        {
            terrainRenderer->DeleteBuildingFromGpu(*terrain->currentSelectedIsland, *terrain->currentSelectedIsland->currentSelectedTile);
            renderer::TerrainRenderer::DeleteBuildingFromCpu(*terrain->currentSelectedIsland->currentSelectedTile);
        }
        else
        {
            terrainRenderer->DeleteBuildingFromGpu(*terrain->currentSelectedIsland, terrain->currentSelectedIsland->currentSelectedTile->connectedTiles);
            renderer::TerrainRenderer::DeleteBuildingFromCpu(*terrain->currentSelectedIsland, terrain->currentSelectedIsland->currentSelectedTile->connectedTiles);
        }
    }

    if (currentAction == Action::STATUS && terrain->currentSelectedIsland && terrain->currentSelectedIsland->currentSelectedTile)
    {
        terrain->currentSelectedIsland->currentSelectedTile->RenderImGui();
    }

    if (currentAction == Action::OPTIONS)
    {
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
        break;
    case ChangeZoom::ZOOM_OUT:
        --zoom;
        break;
    }

    context->camera->zoom = zoom;
    context->camera->position.x = static_cast<float>(context->camera->worldPosition.x) * static_cast<float>(get_tile_width(zoom));
    context->camera->position.y = static_cast<float>(context->camera->worldPosition.y) * static_cast<float>(get_tile_height(zoom));
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

    // build
    if (IsPositionInWorld(mousePicker->currentPosition) &&
        currentAction == Action::BUILD &&
        !terrain->tilesToAdd.tiles.empty()
    )
    {
        renderer::TerrainRenderer::AddBuildingToCpu(*terrain);
    }

    MDCII_ASSERT(terrain->tilesToAdd.tiles.empty(), "[World::OnLeftMouseButtonPressed()] Invalid number of tiles to add.")

    m_worldGui->selectedBuildingTile.Reset();
}

void mdcii::world::World::OnMouseMoved()
{
    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    if (currentAction == Action::BUILD &&
        IsPositionInWorld(mousePicker->currentPosition) &&
        m_worldGui->selectedBuildingTile.HasBuilding() &&
        mousePicker->tilePositionHasChanged
    )
    {
        if (!terrain->tilesToAdd.tiles.empty())
        {
            terrainRenderer->DeleteBuildingFromGpu(*terrain);
        }
        if (terrain->tilesToAdd.tiles.empty())
        {
            terrainRenderer->AddBuildingToGpu(m_worldGui->selectedBuildingTile, mousePicker->currentPosition, *terrain);
        }
    }

    if (currentAction == Action::BUILD && !IsPositionInWorld(mousePicker->currentPosition) && !terrain->tilesToAdd.tiles.empty())
    {
        terrainRenderer->DeleteBuildingFromGpu(*terrain);
    }
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
        if (k == "version" && v.get<std::string>() != Game::VERSION)
        {
            throw MDCII_EXCEPTION("[World::Init()] Invalid map file format.");
        }

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

//-------------------------------------------------
// Json
//-------------------------------------------------

void mdcii::world::to_json(nlohmann::json& t_json, const World& t_world)
{
    t_json = nlohmann::json{
        { "width", t_world.width },
        { "height", t_world.height }
    };
}
