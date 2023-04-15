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
#include "GameWorld.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "TileAtlas.h"
#include "WorldGui.h"
#include "MousePicker.h"
#include "state/State.h"
#include "state/StateStack.h"
#include "renderer/TerrainRenderer.h"
#include "renderer/GridRenderer.h"
#include "layer/GridLayer.h"
#include "layer/WorldLayer.h"
#include "layer/WorldGridLayer.h"
#include "data/Text.h"
#include "file/SavegameFile.h"
#include "file/MapFile.h"
#include "vendor/eventpp/utilities/argumentadapter.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::GameWorld::GameWorld(std::shared_ptr<state::Context> t_context, const state::StateId t_stateId, std::string t_filePath)
    : World(std::move(t_context), t_stateId)
    , m_filePath{ std::move(t_filePath) }
{
    Log::MDCII_LOG_DEBUG("[GameWorld::GameWorld()] Create GameWorld from {}.", m_filePath);

    MDCII_ASSERT(!m_filePath.empty(), "[GameWorld::GameWorld()] Invalid file path given.")

    m_layerTypeToRender = layer::LayerType::ALL;

    Init();
    AddListeners();
}

mdcii::world::GameWorld::~GameWorld() noexcept
{
    Log::MDCII_LOG_DEBUG("[GameWorld::~GameWorld()] Destruct GameWorld.");

    CleanUp();
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::GameWorld::Update()
{
    if (m_runAnimations)
    {
        terrainRenderer->Update();
    }
}

void mdcii::world::GameWorld::Render()
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
            *worldLayer->selectedInstancesSsbo,
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

void mdcii::world::GameWorld::RenderImGui()
{
    begin_right("GameWorld", 342.0f);

    ImGui::SetWindowSize({ 341.0f, 600.0f });

    ImGui::Separator();

    auto bt{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "BackTo") };
    if (ImGui::Button(bt.append(" ").append(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "MainMenu")).c_str()))
    {
        context->stateStack->PopState(stateId);
        context->stateStack->PushState(state::StateId::MAIN_MENU);
    }
    ImGui::Separator();
    m_worldGui->ShowActionsGui();
    ImGui::Separator();

    ImGui::Checkbox(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "TerrainMode").c_str(), &mousePicker->calcForIslandTerrain);
    if (ImGui::IsItemHovered())
    {
        ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(66, 104, 188, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::SetTooltip("%s", data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "TerrainModeTooltip").c_str());
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    ImGui::Separator();

    if (ImGui::CollapsingHeader(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Islands").c_str()))
    {
        static int e{ magic_enum::enum_integer(m_layerTypeToRender) };
        ImGui::RadioButton(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Coast").c_str(), &e, 0);
        ImGui::SameLine();
        ImGui::RadioButton(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Terrain").c_str(), &e, 1);
        ImGui::SameLine();
        ImGui::RadioButton(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Buildings").c_str(), &e, 2);
        ImGui::RadioButton(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Mixed").c_str(), &e, 3);
        ImGui::SameLine();
        ImGui::RadioButton(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "All").c_str(), &e, 4);
        ImGui::SameLine();
        ImGui::RadioButton(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Nothing").c_str(), &e, 5);

        ImGui::Checkbox(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "IslandGrids").c_str(), &m_renderIslandGridLayers);

        if (const auto layer{ magic_enum::enum_cast<layer::LayerType>(e) }; layer.has_value())
        {
            const auto l{ layer.value() };
            m_layerTypeToRender = l;
        }

        terrain->RenderImGui(); // render selected island && island under mouse
    }

    if (ImGui::CollapsingHeader(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "World").c_str()))
    {
        ImGui::Checkbox(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "WorldDeepWater").c_str(), &m_renderWorldLayer);
        ImGui::Checkbox(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "WorldGrid").c_str(), &m_renderWorldGridLayer);
        ImGui::Checkbox(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Animations").c_str(), &m_runAnimations);
    }

    if (ImGui::CollapsingHeader(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Rotation").c_str()))
    {
        m_worldGui->RotateGui();
    }

    if (ImGui::CollapsingHeader(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Zoom").c_str()))
    {
        m_worldGui->ZoomGui();
    }

    if (ImGui::CollapsingHeader(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Camera").c_str()))
    {
        context->camera->RenderImGui();
    }

    if (ImGui::CollapsingHeader(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Culling").c_str()))
    {
        for (const auto& island : terrain->islands)
        {
            auto i{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Island") };
            auto c{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Culling") };

            i.append(": (").append(std::to_string(island->startWorldX)).append(", ").append(std::to_string(island->startWorldY)).append(")");

            c.append(": ");
            if (context->camera->IsIslandNotInCamera(zoom, rotation, *island))
            {
                c.append(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "No"));
            }
            else
            {
                c.append(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Yes"));
            }

            ImGui::TextUnformatted(i.c_str());
            ImGui::SameLine();
            ImGui::TextUnformatted(c.c_str());
        }
    }

    if (currentAction == Action::BUILD && ImGui::CollapsingHeader(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Buildings").c_str()))
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

    ImGui::End();

    mousePicker->RenderImGui();
}

//-------------------------------------------------
// Event handler
//-------------------------------------------------

void mdcii::world::GameWorld::OnLeftMouseButtonPressed()
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

    MDCII_ASSERT(terrain->tilesToAdd.tiles.empty(), "[GameWorld::OnLeftMouseButtonPressed()] Invalid number of tiles to add.")

    m_worldGui->selectedBuildingTile.Reset();
}

void mdcii::world::GameWorld::OnMouseMoved()
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

void mdcii::world::GameWorld::Init()
{
    Log::MDCII_LOG_DEBUG("[GameWorld::Init()] Start initializing the game world...");

    terrain = std::make_unique<Terrain>(context, this);
    tileAtlas = std::make_unique<TileAtlas>();
    terrainRenderer = std::make_unique<renderer::TerrainRenderer>(context, tileAtlas);

    nlohmann::json json;
    if (stateId == state::StateId::NEW_GAME)
    {
        if (file::MapFile mapFile{ m_filePath }; mapFile.LoadJsonFromFile())
        {
            json = mapFile.json;
        }
    }
    else
    {
        if (file::SavegameFile savegameFile{ m_filePath }; savegameFile.LoadJsonFromFile())
        {
            json = savegameFile.json;
        }
    }

    if (json.empty())
    {
        Log::MDCII_LOG_DEBUG("[GameWorld::Init()] Starts POP State {}.", magic_enum::enum_name(stateId));

        context->stateStack->PopState(stateId);
        context->stateStack->PushState(state::StateId::MAIN_MENU);

        return;
    }

    MDCII_ASSERT(!json.empty(), "[GameWorld::Init()] Invalid Json value.")

    // world
    width = json["world"].at("width").get<int32_t>();
    height = json["world"].at("height").get<int32_t>();

    // islands
    terrain->CreateIslandsFromJson(json["islands"]);

    worldLayer = std::make_unique<layer::WorldLayer>(context, this);
    worldLayer->PrepareCpuDataForRendering();
    worldLayer->PrepareGpuDataForRendering();

    worldGridLayer = std::make_unique<layer::WorldGridLayer>(context, this);
    worldGridLayer->PrepareCpuDataForRendering();
    worldGridLayer->PrepareGpuDataForRendering();

    gridRenderer = std::make_unique<renderer::GridRenderer>(context);
    m_worldGui = std::make_unique<WorldGui>(this);

    MDCII_ASSERT(!terrain->islands.empty(), "[GameWorld::Init()] No islands created.")

    Log::MDCII_LOG_DEBUG("[GameWorld::Init()] The world was successfully initialized.");
}

void mdcii::world::GameWorld::AddListeners()
{
    Log::MDCII_LOG_DEBUG("[GameWorld::AddListeners()] Add event listeners.");

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

void mdcii::world::GameWorld::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[GameWorld::CleanUp()] CleanUp game world.");

    event::EventManager::event_dispatcher.removeListener(event::MdciiEventType::MOUSE_BUTTON_PRESSED, m_mouseButtonPressed);
    event::EventManager::event_dispatcher.removeListener(event::MdciiEventType::MOUSE_MOVED, m_mouseMoved);
}

//-------------------------------------------------
// Json
//-------------------------------------------------

void mdcii::world::to_json(nlohmann::json& t_json, const GameWorld& t_gameWorld)
{
    // version
    t_json["version"] = Game::VERSION;

    // world
    t_json["world"] = { { "width", t_gameWorld.width }, { "height", t_gameWorld.height } };

    // islands
    auto islandsJson = nlohmann::json::array();
    for (const auto& island : t_gameWorld.terrain->islands)
    {
        // island
        auto islandJson = nlohmann::json::object();
        islandJson = *island;
        islandsJson.push_back(islandJson);
    }

    t_json["islands"] = islandsJson;
}
