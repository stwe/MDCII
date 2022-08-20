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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <imgui.h>
#include "EditorState.h"
#include "EditorGui.h"
#include "Game.h"
#include "Log.h"
#include "data/Text.h"
#include "ecs/Components.h"
#include "event/EventManager.h"
#include "eventpp/utilities/argumentadapter.h"
#include "map/Map.h"
#include "map/MapContent.h"
#include "map/MousePicker.h"
#include "map/MapGenerator.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::EditorState::EditorState(const Id t_id, state::StateStack* t_stateStack, std::shared_ptr<state::Context> t_context)
    : State(t_id, t_stateStack, std::move(t_context))
{
    Log::MDCII_LOG_DEBUG("[EditorState::EditorState()] Create EditorState.");

    Init();
    AddListeners();
}

mdcii::EditorState::~EditorState() noexcept
{
    Log::MDCII_LOG_DEBUG("[EditorState::~EditorState()] Destruct EditorState.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::EditorState::Input()
{
    /*
    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }
    */

    if (context->window->IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        Log::MDCII_LOG_INFO("[EditorState::Input()] Starts POP EditorState.");
        RequestStackPop();
    }
}

void mdcii::EditorState::Update()
{
}

void mdcii::EditorState::PreRender()
{
}

void mdcii::EditorState::Render()
{
    m_map->Render();
}

void mdcii::EditorState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    // ---------------- Game menu ----------------

    // title
    ImGui::Begin(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Menu").c_str());

    // show action buttons
    m_editorGui->ShowActionsGui();
    ImGui::Separator();

    // rotate map buttons
    m_editorGui->RotateMapGui();
    ImGui::Separator();

    // zom in and out buttons
    m_editorGui->ZoomMapGui();
    ImGui::Separator();

    // todo: Wechsel mgl. obwohl Build zB noch nicht abgeschlossen ist

    // action: build
    if (m_map->currentAction == map::Map::Action::BUILD)
    {
        // possible selected workshop
        m_editorGui->WorkshopGui(m_map->currentSelectedBauGfx);
        if (m_map->currentSelectedBauGfx.HasBuilding())
        {
            ImGui::Separator();
        }

        // list all workshops
        if (!m_map->demolitionMode)
        {
            m_editorGui->AllWorkshopsGui();
        }

        // toggle button to enable/disable demolition mode
        ImGui::TextUnformatted(data::Text::GetMenuText(Game::INI.Get<std::string>("locale", "lang"), "Demolish").c_str());
        EditorGui::ToggleButton("_demolish", &m_map->demolitionMode);
    }

    // action: status
    if (m_map->currentAction == map::Map::Action::STATUS)
    {
        m_editorGui->CurrentSelectedMapTileGui(m_map->currentSelectedMapTile);

        // todo: general info about the map
    }

    // action: options
    if (m_map->currentAction == map::Map::Action::OPTIONS)
    {
        m_editorGui->SaveGameGui();

        // todo: load game, quit game; change config
    }

    ImGui::End();

    // -------------------------------------------


    // ---------------- Debug menus --------------
#ifdef MDCII_DEBUG_BUILD
    m_map->RenderImGui();
#endif
    // -------------------------------------------

    ogl::Window::ImGuiEnd();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::EditorState::Init()
{
    Log::MDCII_LOG_DEBUG("[EditorState::Init()] Initializing editor state.");

    Log::MDCII_LOG_INFO("[EditorState::Init()] Locale is set to: {}", Game::INI.Get<std::string>("locale", "lang"));

    // init menu texts
    data::Text::Init();

    // todo: tmp code
    const auto& mapName{ Game::INI.Get<std::string>("content", "start_map") };
    if (mapName != "data/ExampleMap.json")
    {
        // creates a 32x32 map if it doesn't already exist
        map::MapGenerator mg{ 32, 32, mapName };
    }

    // create the Map object to edit
    m_map = std::make_shared<map::Map>(mapName, context);

    // create the menus
    m_editorGui = std::make_unique<EditorGui>(m_map);

    Log::MDCII_LOG_DEBUG("[EditorState::Init()] The editor state was successfully initialized.");
}

void mdcii::EditorState::AddListeners() const
{
    Log::MDCII_LOG_DEBUG("[EditorState::AddListeners()] Add listeners.");

    // a new bauGfx is selected from the menu
    event::EventManager::eventDispatcher.appendListener(
        event::MdciiEventType::BAUGFX_SELECTED,
        eventpp::argumentAdapter<void(const event::BauGfxSelectedEvent&)>(
            [&](const event::BauGfxSelectedEvent& t_event) {
                if (t_event.selectedBauGfx.HasBuilding())
                {
                    m_map->currentSelectedBauGfx = t_event.selectedBauGfx;
                }
            }
        )
    );

    // mouse button pressed
    event::EventManager::eventDispatcher.appendListener(
        event::MdciiEventType::MOUSE_BUTTON_PRESSED,
        eventpp::argumentAdapter<void(const event::MouseButtonPressedEvent&)>(
            [&](const event::MouseButtonPressedEvent& t_event) {
                // right mouse button && build action
                if (t_event.button == map::MousePicker::RIGHT_MOUSE_BUTTON && m_map->currentAction == map::Map::Action::BUILD)
                {
                    // clear the selection
                    m_map->currentSelectedBauGfx = {};

                    // update buildings layer
                    const auto view{ Game::ecs.view<const ecs::BuildingUpdatedComponent, const ecs::BuildingsLayerTileComponent>() };
                    for (const auto entity : view)
                    {
                        const auto& buildingsLayerTileComponent{ view.get<const ecs::BuildingsLayerTileComponent>(entity) };
                        m_map->mapContent->GetLayer(map::LayerType::BUILDINGS).ReplaceTile(buildingsLayerTileComponent.mapTile);

                        Game::ecs.remove<ecs::BuildingUpdatedComponent>(entity);
                    }
                }
            }
        )
    );
}
