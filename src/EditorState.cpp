#include <imgui.h>
#include <magic_enum.hpp>
#include "EditorState.h"
#include "Log.h"
#include "data/HousesJsonFile.h"
#include "renderer/TileRenderer.h"
#include "renderer/Utils.h"
#include "ogl/OpenGL.h"
#include "ogl/resource/ResourceManager.h"
#include "ogl/resource/ResourceUtil.h"

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
    m_camera->Update();
}

void mdcii::EditorState::Render()
{
    RenderMap();
}

void mdcii::EditorState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    ImGui::Begin("Edit", nullptr, 0);

    ImGui::Text("Current Id: %d", m_currentId);

    TileMenuById();
    //TileMenuByGroup();

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

    // load Grafiken.txt
    m_graphicsFileContent = ogl::resource::ResourceUtil::ReadGraphicsFile("E:/Dev/MDCII/resources/data/Grafiken.txt");

    Log::MDCII_LOG_DEBUG("[EditorState::Init()] The editor state was successfully initialized.");
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::EditorState::TileMenuById()
{
    if (ImGui::TreeNode("Objects"))
    {
        for (const auto& [id, tileAssetProperties] : m_housesJsonFile->tileAssetPropertiesMap)
        {
            if (tileAssetProperties.id >= 0)
            {
                auto idStr{ std::to_string(tileAssetProperties.id) };

                std::string name{};
                if (m_graphicsFileContent.count(tileAssetProperties.id))
                {
                    name = m_graphicsFileContent.at(tileAssetProperties.id);
                }

                if (ImGui::TreeNode(idStr.append(" ").append(name).c_str()))
                {
                    m_currentId = id;
                    m_renderer->RenderTileGfxImGui(tileAssetProperties, *m_stdBshFile);
                    m_renderer->RenderTileBauGfxImGui(tileAssetProperties, *m_bauhausBshFile);

                    ImGui::TreePop();
                }
            }
        }

        ImGui::TreePop();
    }
}

void mdcii::EditorState::TileMenuByGroup()
{
    if (ImGui::TreeNode("Objects"))
    {
        // for each TileKind ...
        magic_enum::enum_for_each<data::TileKind>([&](const data::TileKind t_kind)
        {
            const auto ret{ m_housesJsonFile->tileAssetPropertiesMultimap.equal_range(t_kind) };
            const std::string kindStr{ magic_enum::enum_name(t_kind) };

            // create a tree node
            if (ImGui::TreeNode(kindStr.c_str()))
            {
                for (auto it = ret.first; it != ret.second; ++it)
                {

                    std::string name{};
                    if (m_graphicsFileContent.count(it->second.id))
                    {
                        name = m_graphicsFileContent.at(it->second.id);
                    }

                    // create a tree node for each Id of the TileKind
                    const auto idStr{ std::to_string(it->second.id).append(" ").append(name) };
                    if (ImGui::TreeNode(idStr.c_str()))
                    {
                        m_currentId = it->second.id;
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
}

void mdcii::EditorState::RenderMap()
{
    const auto id{ ogl::resource::ResourceManager::LoadTexture("resources/textures/red.png").id };

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 4; ++x)
        {
            glm::vec2 s;
            s = renderer::Utils::MapToIso(x, y);

            m_renderer->RenderTile(
                renderer::Utils::GetModelMatrix(s, glm::vec2(64.0f, 32.0f)),
                id,
                *context->window,
                *m_camera
            );

            auto idx{ y * 4 + x };
            auto gfx{ m_map.at(idx) };
            if (gfx > 0)
            {
                RenderBuilding(m_map.at(idx), x, y);
            }
        }
    }
}

void mdcii::EditorState::RenderBuilding(const int t_id, const int t_mapX, const int t_mapY) const
{
    const auto w{ static_cast<float>(m_stdBshFile->bshTextures[t_id]->width) };
    const auto h{ static_cast<float>(m_stdBshFile->bshTextures[t_id]->height) };

    auto screenPosition{ renderer::Utils::MapToIso(t_mapX, t_mapY) };
    screenPosition.y -= (h - 32.0f);

    if (t_id == 4)
    {
        screenPosition.y -= 20.0f;
    }

    m_renderer->RenderTile(
        renderer::Utils::GetModelMatrix(screenPosition, glm::vec2(w, h)),
        m_stdBshFile->bshTextures[t_id]->textureId,
        *context->window,
        *m_camera
    );
}
