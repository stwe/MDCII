#include <imgui.h>
#include <magic_enum.hpp>
#include "EditorState.h"
#include "Log.h"
#include "data/HousesJsonFile.h"
#include "renderer/TileRenderer.h"
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
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }
    */

    // ESC for quit
    if (context->window->IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        Log::MDCII_LOG_INFO("[EditorState::Input()] Starts POP EditorState.");
        RequestStackPop();
    }
}

void mdcii::EditorState::Update()
{
}

void mdcii::EditorState::Render()
{
}

void mdcii::EditorState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    ImGui::Begin("Edit", nullptr, 0);

    if (ImGui::TreeNode("Objects"))
    {
        // for each TileKind ...
        magic_enum::enum_for_each<data::TileKind>([&](const data::TileKind t_kind)
        {
            const auto ret{ m_housesJsonFile->tileAssetPropertiesMultimap.equal_range(t_kind) };
            const std::string str{ magic_enum::enum_name(t_kind) };

            // create a tree node
            if (ImGui::TreeNode(str.c_str()))
            {
                for (auto it = ret.first; it != ret.second; ++it)
                {
                    // create a tree node for each Id of the TileKind
                    if (ImGui::TreeNode(std::to_string(it->second.id).c_str()))
                    {
                        m_renderer->RenderTileGfxImGui(it->second, *m_stdBshFile);
                        m_renderer->RenderTileBauGfxImGui(it->second, *m_bauhausBshFile);

                        ImGui::TreePop();
                    }
                }

                ImGui::TreePop();
            }
        });

        ImGui::TreePop();
    }

    ImGui::End();

    ogl::Window::ImGuiEnd();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::EditorState::Init()
{
    Log::MDCII_LOG_DEBUG("[EditorState::Init()] Initializing editor state.");

    // load palette
    m_paletteFile = std::make_unique<file::PaletteFile>("STADTFLD.COL");
    m_paletteFile->ReadDataFromChunks();

    // load stadtfld textures
    m_stdBshFile = std::make_unique<file::BshFile>("STADTFLD.BSH", m_paletteFile->palette);
    m_stdBshFile->ReadDataFromChunks();

    // load texture previews
    m_bauhausBshFile = std::make_unique<file::BshFile>("BAUHAUS.BSH", m_paletteFile->palette);
    m_bauhausBshFile->ReadDataFromChunks();

    // create a camera
    m_camera = std::make_unique<camera::Camera>(context->window, glm::vec2(0.0f, 0.0f));

    // create a renderer
    m_renderer = std::make_unique<renderer::TileRenderer>();

    // load the properties of the assets
    m_housesJsonFile = std::make_unique<data::HousesJsonFile>();

    Log::MDCII_LOG_DEBUG("[EditorState::Init()] The editor state was successfully initialized.");
}
