#include <imgui.h>
#include "EditorState.h"
#include "EditorGui.h"
#include "Game.h"
#include "Log.h"
#include "data/Text.h"
#include "event/EventManager.h"
#include "eventpp/utilities/argumentadapter.h"
#include "map/Map.h"
#include "map/MapContent.h"
#include "map/MousePicker.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::EditorState::EditorState(const Id t_id, state::StateStack* t_stateStack, std::shared_ptr<Context> t_context)
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
    m_map->Render(*context->window, *context->camera);
}

void mdcii::EditorState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    // todo
    //m_map->RenderImGui();

    // title
    ImGui::Begin(m_text->GetMenuText(m_lang, "Menu").c_str());

    // debug stuff
    m_editorGui->DebugGui();
    ImGui::Separator();

    // rotate map buttons
    m_editorGui->RotateMapGui();
    ImGui::Separator();

    // the selected workshop
    m_editorGui->WorkshopGui(m_map->selectedBauGfx);
    if (m_map->selectedBauGfx.HasBuilding())
    {
        ImGui::Separator();
    }

    // list all workshops
    m_editorGui->AllWorkshopsGui();

    ImGui::End();

    // MousePicker Gui
    m_map->mousePicker->RenderImGui();

    ogl::Window::ImGuiEnd();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::EditorState::Init()
{
    Log::MDCII_LOG_DEBUG("[EditorState::Init()] Initializing editor state.");

    // set lang
    m_lang = Game::INI.Get<std::string>("locale", "lang");

    Log::MDCII_LOG_INFO("[EditorState::Init()] Locale is set to: {}.", m_lang);

    // read menu texts
    m_text = std::make_shared<data::Text>();

    // create the Map object to edit
    m_map = std::make_shared<map::Map>("data/ExampleMap.json", context->buildings, *context->window, *context->camera);

    // create the menus
    m_editorGui = std::make_unique<EditorGui>(m_text, m_map, context->buildings);

    Log::MDCII_LOG_DEBUG("[EditorState::Init()] The editor state was successfully initialized.");
}

void mdcii::EditorState::AddListeners() const
{
    Log::MDCII_LOG_DEBUG("[EditorState::AddListeners()] Add listeners.");

    // a new bauGfx is selected from the menu
    event::EventManager::eventDispatcher.appendListener(
        event::MdciiEventType::BAUGFX_SELECTED,
        eventpp::argumentAdapter<void(const event::BauGfxSelectedEvent&)>(
            [&](const event::BauGfxSelectedEvent& t_event)
            {
                if (t_event.selectedBauGfx.HasBuilding())
                {
                    m_map->selectedBauGfx = t_event.selectedBauGfx;
                }
            }
        )
    );

    // right mouse button pressed
    event::EventManager::eventDispatcher.appendListener(
        event::MdciiEventType::MOUSE_BUTTON_PRESSED,
        eventpp::argumentAdapter<void(const event::MouseButtonPressedEvent&)>
        (
            [&](const event::MouseButtonPressedEvent& t_event)
            {
                if (t_event.button == 1)
                {
                    // clear the selection
                    m_map->selectedBauGfx = {};

                    // update buildings layer
                    const auto view{ Game::ecs.view<const ecs::BuildingUpdatedComponent, const ecs::BuildingsLayerTileComponent>() };
                    for (const auto entity : view)
                    {
                        const auto& [uc, bc] { view.get(entity) };
                        m_map->mapContent->GetLayer(map::LayerType::BUILDINGS).ReplaceTile(bc.mapTile);

                        Game::ecs.remove<ecs::BuildingUpdatedComponent>(entity);
                    }
                }
            }
        )
    );
}
