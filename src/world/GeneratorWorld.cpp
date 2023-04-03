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
#include "Log.h"
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
#include "eventpp/utilities/argumentadapter.h"

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

    ImGui::Separator();

    ImGui::Checkbox("Render deep water", &m_renderDeepWater);
    ImGui::Checkbox("Render islands", &m_renderIslands);

    m_worldGui->RotateGui();
    m_worldGui->ZoomGui();

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
}

void mdcii::world::GeneratorWorld::OnMouseMoved()
{
    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
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

void mdcii::world::GeneratorWorld::RenderIslandFileChooser(std::vector<std::string>& t_files)
{
    static int32_t fileIndex{ 0 };
    static int32_t x{ 0 };
    static int32_t y{ 0 };

    if (t_files.empty())
    {
        ImGui::TextUnformatted(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "MissingFiles").c_str());
    }
    else
    {
        file_chooser(t_files, &fileIndex);

        ImGui::SliderInt("Island x", &x, 0, World::WORLD_MAX_WIDTH - 1); // - island width / height
        ImGui::SliderInt("Island y", &y, 0, World::WORLD_MAX_HEIGHT - 1);

        if (ImGui::Button("Add island"))
        {
            Log::MDCII_LOG_DEBUG("[GeneratorWorld::RenderIslandFileChooser()] Add island at ({}, {})", x, y);

            file::IslandFile islandFile{ context->mdciiResourcesManager->islandFiles.at(fileIndex) };
            if (islandFile.LoadJsonFromFile())
            {
                terrain->AddIslandFromJson(islandFile.json, x, y);
            }
        }
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
