#include <imgui.h>
#include "EditorState.h"
#include "Game.h"
#include "Log.h"
#include "data/Text.h"
#include "map/Map.h"
#include "map/MousePicker.h"
#include "renderer/ImGuiTileRenderer.h"
#include "ogl/OpenGL.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::EditorState::EditorState(const Id t_id, state::StateStack* t_stateStack, std::shared_ptr<Context> t_context)
    : State(t_id, t_stateStack, std::move(t_context))
{
    Log::MDCII_LOG_DEBUG("[EditorState::EditorState()] Create EditorState.");

    Init();
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
    if (m_mousePicker->selected.lastChanged)
    {
        m_map->SelectTile(m_mousePicker->selected.lastPosition);
    }
}

void mdcii::EditorState::PreRender()
{
}

void mdcii::EditorState::Render()
{
    m_map->Render(*context->window, *context->camera);
    m_mousePicker->Render(*context->window, *context->camera);
}

void mdcii::EditorState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    m_map->RenderImGui();

    ImGui::Begin("EditorState");
    EditMenu();
    ImGui::End();

    m_mousePicker->RenderImGui();

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

    // read menu texts
    m_text = std::make_unique<data::Text>();

    // create the Map object to edit
    m_map = std::make_shared<map::Map>(
        map::ExampleMap::EXAMPLE_ISLAND,
        map::ExampleMap::EXAMPLE_WIDTH, map::ExampleMap::EXAMPLE_HEIGHT,
        context->buildings
    );

    // create the MousePicker to select tiles
    m_mousePicker = std::make_unique<map::MousePicker>(m_map);

    Log::MDCII_LOG_DEBUG("[EditorState::Init()] The editor state was successfully initialized.");
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::EditorState::EditMenu() const
{
    if (ImGui::TreeNode(m_text->GetMenuText(m_lang, "Workshops").c_str()))
    {
        const auto& shops{ m_text->GetBuildingsTexts(data::Text::Section::WORKSHOPS, m_lang) };

        for (const auto& [k, v] : shops)
        {
            if (ImGui::TreeNode(v.c_str()))
            {
                const auto& building{ context->buildings->buildingsMap.at(std::stoi(k)) };
                renderer::ImGuiTileRenderer::RenderTileBauGfxImGui(building, *m_map->bauhausBshFile);

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    // todo:
    // todo: bei select: Gebäude oben anzeigen und bei weiteren Klicks drehen
    // todo:
}
