#include <imgui.h>
#include <magic_enum.hpp>
#include <filesystem>
#include "EditorState.h"
#include "Game.h"
#include "Log.h"
#include "data/HousesJsonFile.h"
#include "data/GraphicsFile.h"
#include "map/Map.h"
#include "map/MousePicker.h"
#include "renderer/ImGuiTileRenderer.h"
#include "ogl/OpenGL.h"
#include "cod/CodParser.h"

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

    // ESC for quit
    if (context->window->IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        Log::MDCII_LOG_INFO("[EditorState::Input()] Starts POP EditorState.");
        RequestStackPop();
    }
}

void mdcii::EditorState::Update()
{
    /*
    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }
    */

    m_camera->Update();
}

void mdcii::EditorState::PreRender()
{
}

void mdcii::EditorState::Render()
{
    m_map->Render(*context->window, *m_camera);
    m_mousePicker->Render(*context->window, *m_camera);
}

void mdcii::EditorState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    m_map->RenderImGui();

    ImGui::Begin("EditorState", nullptr, 0);

    TileMenuById();
    //TileMenuByGroup();

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

    // create houses.json
    CreateHousesJsonFile();

    // load Grafiken.txt for ImGui menus
    m_graphicsFileContent = data::GraphicsFile::ReadGraphicsFile(Game::RESOURCES_PATH + "data/Grafiken.txt");

    // create camera
    m_camera = std::make_unique<camera::Camera>(context->window);

    // create a Map object to edit
    m_map = std::make_shared<map::Map>();

    // create the MousePicker to select tiles
    m_mousePicker = std::make_unique<map::MousePicker>(m_map);

    Log::MDCII_LOG_DEBUG("[EditorState::Init()] The editor state was successfully initialized.");
}

void mdcii::EditorState::CreateHousesJsonFile() const
{
    if (std::filesystem::exists(Game::RESOURCES_PATH + "data/houses.json"))
    {
        return;
    }

    Log::MDCII_LOG_INFO("[EditorState::CreateHousesJsonFile()] Start building the houses.json...");
    auto parser{ cod::CodParser("haeuser.cod", true) };
    Log::MDCII_LOG_INFO("[EditorState::CreateHousesJsonFile()] The houses.json was created successfully.");
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::EditorState::TileMenuById() const
{
    if (ImGui::TreeNode("Objects"))
    {
        for (const auto& [id, tileAssetProperties] : m_map->housesJsonFile->tileAssets)
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
                    m_map->selectedId = id;
                    renderer::ImGuiTileRenderer::RenderTileGfxImGui(tileAssetProperties, *m_map->stdBshFile);
                    renderer::ImGuiTileRenderer::RenderTileBauGfxImGui(tileAssetProperties, *m_map->bauhausBshFile);

                    ImGui::TreePop();
                }
            }
        }

        ImGui::TreePop();
    }
}

void mdcii::EditorState::TileMenuByGroup() const
{
    if (ImGui::TreeNode("Objects"))
    {
        // for each TileKind ...
        magic_enum::enum_for_each<data::TileKind>([&](const data::TileKind t_kind)
        {
            const auto ret{ m_map->housesJsonFile->tileAssetsByKind.equal_range(t_kind) };
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
                        m_map->selectedId = it->second.id;
                        renderer::ImGuiTileRenderer::RenderTileGfxImGui(it->second, *m_map->stdBshFile);
                        renderer::ImGuiTileRenderer::RenderTileBauGfxImGui(it->second, *m_map->bauhausBshFile);

                        ImGui::TreePop();
                    }
                }

                ImGui::TreePop();
            }
        });

        ImGui::TreePop();
    }
}
