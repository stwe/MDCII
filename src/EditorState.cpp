#include <imgui.h>
#include "EditorState.h"
#include "Log.h"
#include "MdciiAssert.h"
#include "data/HousesJsonFile.h"
#include "renderer/MeshRenderer.h"
#include "renderer/Utils.h"
#include "file/BshFile.h"
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
        for (const auto& [id, tileAssetProperties] : m_housesJsonFile->tileAssetPropertiesMap)
        {
            const auto gfx{ tileAssetProperties.gfx + 1 };

            const auto stadtfldTextureId{ reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(m_stdBshFile->bshTextures.at(gfx)->textureId)) };

            const auto assetId{ std::to_string(tileAssetProperties.id) };

            if (ImGui::TreeNode(assetId.c_str()))
            {
                // stadtfld.bsh texture

                MDCII_ASSERT(m_stdBshFile->bshTextures.at(gfx)->width, "Invalid width")
                MDCII_ASSERT(m_stdBshFile->bshTextures.at(gfx)->height, "Invalid height")

                ImGui::Text("Gfx: %d", gfx);


                ImGui::Image(
                    stadtfldTextureId,
                    ImVec2(
                        static_cast<float>(m_stdBshFile->bshTextures.at(gfx)->width),
                        static_cast<float>(m_stdBshFile->bshTextures.at(gfx)->height)
                    ),
                    ImVec2(0.0f, 0.0f),
                    ImVec2(1.0f, 1.0f),
                    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                    ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
                );


                // bauhaus.bsh preview texture

                const auto baugfx{ tileAssetProperties.baugfx + 1 };

                MDCII_ASSERT(m_bauhausBshFile->bshTextures.at(baugfx)->width, "Invalid width")
                MDCII_ASSERT(m_bauhausBshFile->bshTextures.at(baugfx)->height, "Invalid height")

                ImGui::Text("BauGfx: %d", baugfx);

                const auto bauhausTextureId{ reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(m_bauhausBshFile->bshTextures.at(baugfx)->textureId)) };
                ImGui::Image(
                    bauhausTextureId,
                    ImVec2(
                        static_cast<float>(m_bauhausBshFile->bshTextures.at(baugfx)->width),
                        static_cast<float>(m_bauhausBshFile->bshTextures.at(baugfx)->height)
                    ),
                    ImVec2(0.0f, 0.0f),
                    ImVec2(1.0f, 1.0f),
                    ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                    ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
                );

                ImGui::TreePop();
            }
        }

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
    m_renderer = std::make_unique<renderer::MeshRenderer>();

    // load the properties of the assets
    m_housesJsonFile = std::make_unique<data::HousesJsonFile>();

    Log::MDCII_LOG_DEBUG("[EditorState::Init()] The editor state was successfully initialized.");

}
