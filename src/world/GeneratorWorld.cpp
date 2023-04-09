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
#include "GeneratorWorld.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "TileAtlas.h"
#include "WorldGui.h"
#include "MousePicker.h"
#include "renderer/TerrainRenderer.h"
#include "layer/WorldLayer.h"
#include "layer/WorldGridLayer.h"
#include "state/State.h"
#include "state/StateStack.h"
#include "data/Text.h"
#include "file/MdciiResourcesManager.h"
#include "file/IslandFile.h"
#include "vendor/eventpp/utilities/argumentadapter.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::GeneratorWorld::GeneratorWorld(std::shared_ptr<state::Context> t_context, state::StateId t_stateId, const int32_t t_width, const int32_t t_height)
    : World(std::move(t_context), t_stateId)
{
    Log::MDCII_LOG_DEBUG("[GeneratorWorld::GeneratorWorld()] Create GeneratorWorld.");

    width = t_width;
    height = t_height;

    terrain = std::make_unique<Terrain>(context, this);
    tileAtlas = std::make_unique<TileAtlas>();
    terrainRenderer = std::make_unique<renderer::TerrainRenderer>(context, tileAtlas);

    worldLayer = std::make_unique<layer::WorldLayer>(context, this);
    worldLayer->PrepareCpuDataForRendering();
    worldLayer->PrepareGpuDataForRendering();

    worldGridLayer = std::make_unique<layer::WorldGridLayer>(context, this);
    worldGridLayer->PrepareCpuDataForRendering();
    worldGridLayer->PrepareGpuDataForRendering();

    m_worldGui = std::make_unique<WorldGui>(this);

    AddListeners();
}

mdcii::world::GeneratorWorld::~GeneratorWorld() noexcept
{
    Log::MDCII_LOG_DEBUG("[GeneratorWorld::~GeneratorWorld()] Destruct GeneratorWorld.");

    CleanUp();
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::GeneratorWorld::Update()
{
    terrainRenderer->Update();
}

void mdcii::world::GeneratorWorld::Render()
{
    if (m_renderDeepWater)
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

    ogl::OpenGL::EnableAlphaBlending();

    if (m_renderIslands)
    {
        for (const auto& island : terrain->islands)
        {
            terrainRenderer->Render(*island->coastLayer, zoom, rotation);
            terrainRenderer->Render(*island->mixedLayer, zoom, rotation);
        }
    }

    ogl::OpenGL::DisableBlending();

    mousePicker->Render(*context->window, *context->camera);
}

void mdcii::world::GeneratorWorld::RenderImGui()
{
    begin_right("GeneratorWorld", 362.0f);

    ImGui::SetWindowSize({ 361.0f, 600.0f });

    ImGui::Separator();

    auto bt{ data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "BackTo") };
    if (ImGui::Button(bt.append(" ").append(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "MainMenu")).c_str()))
    {
        context->stateStack->PopState(stateId);
        context->stateStack->PushState(state::StateId::MAIN_MENU);
    }

    ImGui::Separator();

    ImGui::Text("World width %d", width);
    ImGui::Text("World height %d", height);

    ImGui::Separator();

    RenderIslandFileChooser(context->mdciiResourcesManager->islandFiles);

    if (ImGui::Button("Add island"))
    {
        m_addIsland = true;
    }

    if (m_addIsland && m_island_width == -1 && m_island_height == -1 && m_putX == -1 && m_putY == -1)
    {
        MDCII_ASSERT(m_json.empty(), "[GeneratorWorld::RenderImGui()] Invalid Json value.")

        if (file::IslandFile islandFile{ context->mdciiResourcesManager->islandFiles.at(m_island_file_index) }; islandFile.LoadJsonFromFile())
        {
            m_json = islandFile.json;

            MDCII_ASSERT(!m_json.empty(), "[GeneratorWorld::RenderImGui()] Invalid Json value.")

            m_island_width = m_json.at("width");
            m_island_height = m_json.at("height");

            MDCII_ASSERT(m_island_width > 0, "[GeneratorWorld::RenderImGui()] Invalid island width.")
            MDCII_ASSERT(m_island_height > 0, "[GeneratorWorld::RenderImGui()] Invalid island height.")

            Log::MDCII_LOG_DEBUG("[GeneratorWorld::RenderImGui()] Selected island size ({}, {}).", m_island_width, m_island_height);
        }
    }

    ImGui::Separator();

    ImGui::Checkbox("Render deep water", &m_renderDeepWater);
    ImGui::Checkbox("Render islands", &m_renderIslands);

    m_worldGui->RotateGui();
    m_worldGui->ZoomGui();

    ImGui::Separator();

    if (m_currentSelectedTile)
    {
        m_currentSelectedTile->RenderImGui();
    }

    ImGui::End();

    mousePicker->RenderImGui();
}

//-------------------------------------------------
// Event handler
//-------------------------------------------------

void mdcii::world::GeneratorWorld::OnLeftMouseButtonPressed()
{
    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    if (m_addIsland)
    {
        // todo: If there are multiple islands, the Aabbs must be observed.
        // todo: Change the color of the selection in the shader (e.g. pass 2 instead of 1). If overlapping, maybe show more red.

        Log::MDCII_LOG_DEBUG("[GeneratorWorld::OnLeftMouseButtonPressed()] Write island at ({}, {}).", m_putX, m_putY);
        terrain->AddIslandFromJson(m_json, m_putX, m_putY);

        m_addIsland = false;
        m_island_width = -1;
        m_island_height = -1;
        m_putX = -1;
        m_putY = -1;
        m_json.clear();
    }
}

void mdcii::world::GeneratorWorld::OnMouseMoved()
{
    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    // todo: Selection currently only works in DEG0

    if (m_addIsland &&
        IsPositionInWorld(mousePicker->currentPosition) &&
        IsPositionInWorld(mousePicker->currentPosition.x + m_island_width - 1, mousePicker->currentPosition.y + m_island_height - 1))
    {
        // delete selection only if the mouse position has changed
        if (m_putX != -1 && m_putY != -1 && glm::ivec2(m_putX, m_putY) != mousePicker->currentPosition)
        {
            for (auto y{ 0 }; y < m_island_height; ++y)
            {
                for (auto x{ 0 }; x < m_island_width; ++x)
                {
                    Log::MDCII_LOG_DEBUG("[GeneratorWorld::OnMouseMoved()] Delete rendered selection at ({}, {}).", m_putX, m_putY);
                    renderer::TerrainRenderer::UnselectPosition({ m_putX + x, m_putY + y }, *worldLayer);
                }
            }

            m_putX = -1;
            m_putY = -1;
        }

        // render selection
        if (m_putX == -1 && m_putY == -1)
        {
            m_putX = mousePicker->currentPosition.x;
            m_putY = mousePicker->currentPosition.y;

            for (auto y{ 0 }; y < m_island_height; ++y)
            {
                for (auto x{ 0 }; x < m_island_width; ++x)
                {
                    Log::MDCII_LOG_DEBUG("[GeneratorWorld::OnMouseMoved()] Render new selection at ({}, {}).", m_putX, m_putY);
                    renderer::TerrainRenderer::SelectPosition({ m_putX + x, m_putY + y }, *worldLayer);
                }
            }
        }
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::GeneratorWorld::AddListeners()
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
// ImGui
//-------------------------------------------------

void mdcii::world::GeneratorWorld::RenderIslandFileChooser(std::vector<std::string>& t_files) const
{
    if (t_files.empty())
    {
        ImGui::TextUnformatted(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "MissingFiles").c_str());
    }
    else
    {
        file_chooser(t_files, &m_island_file_index);
    }
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::world::GeneratorWorld::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[GeneratorWorld::CleanUp()] CleanUp generator world.");

    event::EventManager::event_dispatcher.removeListener(event::MdciiEventType::MOUSE_BUTTON_PRESSED, m_mouseButtonPressed);
    event::EventManager::event_dispatcher.removeListener(event::MdciiEventType::MOUSE_MOVED, m_mouseMoved);
}

//-------------------------------------------------
// Json
//-------------------------------------------------

void mdcii::world::to_json(nlohmann::json& t_json, const GeneratorWorld& t_generatorWorld)
{
    // version
    t_json["version"] = Game::VERSION;

    // world
    t_json["world"] = { { "width", t_generatorWorld.width }, { "height", t_generatorWorld.height } };

    // islands
    auto islandsJson = nlohmann::json::array();

    // todo: islands Json
    /*
    for (const auto& island : t_generatorWorld.terrain->islands)
    {
        // island
        auto islandJson = nlohmann::json::object();
        islandJson = *island;
        islandsJson.push_back(islandJson);
    }
    */

    t_json["islands"] = islandsJson;
}
