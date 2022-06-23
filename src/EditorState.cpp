#include <imgui.h>
#include "EditorState.h"
#include "Game.h"
#include "Log.h"
#include "file/BshFile.h"
#include "data/Text.h"
#include "map/Map.h"
#include "map/MousePicker.h"
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

void mdcii::EditorState::EditMenu()
{
    if (m_currentId > -1)
    {
        ImGui::Text(m_buildingName.c_str());

        ImGui::Separator();

        const auto& building{ context->buildings->buildingsMap.at(m_currentId) };

        const auto textureWidth{ m_map->bauhausBshFile->bshTextures.at(building.baugfx)->width };
        const auto textureHeight{ m_map->bauhausBshFile->bshTextures.at(building.baugfx)->height };
        const auto textureId{ reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(m_map->bauhausBshFile->bshTextures.at(building.baugfx + m_orientation)->textureId)) };

        if (ImGui::Button(m_text->GetMenuText(m_lang, "RotateBuildingRight").c_str()))
        {
            ++m_orientation;
            if (m_orientation > 3)
            {
                m_orientation = 0;
            }
        }

        ImGui::SameLine();

        ImGui::Image(
            textureId,
            ImVec2(static_cast<float>(textureWidth), static_cast<float>(textureHeight)),
            ImVec2(0.0f, 0.0f),
            ImVec2(1.0f, 1.0f),
            ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
            ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
        );

        ImGui::SameLine();

        if (ImGui::Button(m_text->GetMenuText(m_lang, "RotateBuildingLeft").c_str()))
        {
            --m_orientation;
            if (m_orientation < 0)
            {
                m_orientation = 3;
            }
        }
    }

    ImGui::Separator();

    if (ImGui::TreeNode(m_text->GetMenuText(m_lang, "Workshops").c_str()))
    {
        const auto& shops{ m_text->GetBuildingsTexts(data::Text::Section::WORKSHOPS, m_lang) };

        for (const auto& [k, v] : shops)
        {
            if (ImGui::TreeNode(v.c_str()))
            {
                const auto& building{ context->buildings->buildingsMap.at(std::stoi(k)) };

                const auto textureWidth{ m_map->bauhausBshFile->bshTextures.at(building.baugfx)->width };
                const auto textureHeight{ m_map->bauhausBshFile->bshTextures.at(building.baugfx)->height };
                const auto textureId{ reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(m_map->bauhausBshFile->bshTextures.at(building.baugfx)->textureId)) };

                if (ImGui::ImageButton(
                    textureId,
                    ImVec2(static_cast<float>(textureWidth), static_cast<float>(textureHeight)),
                    ImVec2(0.0f, 0.0f),
                    ImVec2(1.0f, 1.0f),
                    -1,
                    ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
                ))
                {
                    m_currentId = std::stoi(k);
                    m_orientation = 0;
                    m_buildingName = v;
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}
