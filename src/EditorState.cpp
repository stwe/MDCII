#include <imgui.h>
#include <magic_enum.hpp>
#include "EditorState.h"
#include "Log.h"
#include "data/HousesJsonFile.h"
#include "renderer/TileRenderer.h"
#include "renderer/TextRenderer.h"
#include "renderer/Utils.h"
#include "ogl/OpenGL.h"
#include "ogl/resource/ResourceManager.h"
#include "ogl/resource/ResourceUtil.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::EditorState::EditorState(const Id t_id, state::StateStack* t_stateStack, std::shared_ptr<Context> t_context)
    : State(t_id, t_stateStack, std::move(t_context))
    , m_rotation{ renderer::Rotation::DEG0 }
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

void mdcii::EditorState::PreRender()
{
}

void mdcii::EditorState::Render()
{
    RenderMap();
}

void mdcii::EditorState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    ImGui::Begin("Edit", nullptr, 0);

    ImGui::Text("Current rotation: %s", magic_enum::enum_name(m_rotation).data());
    if (ImGui::Button("Rotate"))
    {
        Rotate();
    }

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

    // create text renderer
    m_textRenderer = std::make_unique<renderer::TextRenderer>("E:/Dev/MDCII/resources/bitter/Bitter-Regular.otf");

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
    if (m_rotation == renderer::Rotation::DEG0)
    {
        for (int y = 0; y < 8; ++y)
        {
            for (int x = 0; x < 4; ++x)
            {
                RenderMapContent(x, y);
            }
        }
    }

    if (m_rotation == renderer::Rotation::DEG90)
    {
        for (int x = 0; x < 4; ++x)
        {
            for (int y = 7; y >= 0; --y)
            {
                RenderMapContent(x, y);
            }
        }
    }

    if (m_rotation == renderer::Rotation::DEG180)
    {
        for (int y = 7; y >= 0; --y)
        {
            for (int x = 3; x >= 0; --x)
            {
                RenderMapContent(x, y);
            }
        }
    }

    if (m_rotation == renderer::Rotation::DEG270)
    {
        for (int x = 3; x >= 0; --x)
        {
            for (int y = 0; y < 8; ++y)
            {
                RenderMapContent(x, y);
            }
        }
    }
}

void mdcii::EditorState::RenderMapContent(const int t_x, const int t_y)
{
    const auto id{ ogl::resource::ResourceManager::LoadTexture("resources/textures/red.png").id };
    auto pos{ renderer::Utils::MapToIso(t_x, t_y, m_rotation) };

    m_renderer->RenderTile(
        renderer::Utils::GetModelMatrix(
            pos,
            glm::vec2(64.0f, 32.0f)
        ),
        id,
        *context->window,
        *m_camera
    );

    m_textRenderer->RenderText(
        std::to_string(t_x).append(",  ").append(std::to_string(t_y)),
        pos.x + 16, pos.y + 8,
        0.25f,
        glm::vec3(0.0f, 1.0f, 0.0f),
        *context->window,
        *m_camera
    );

    const auto idx{ t_y * 4 + t_x };
    const auto gfx{ m_map.at(idx) };
    if (gfx > 0)
    {
        RenderBuilding(m_map.at(idx), t_x, t_y);
    }
}

void mdcii::EditorState::RenderBuilding(int t_id, const int t_mapX, const int t_mapY) const
{
    t_id += static_cast<int>(m_rotation);

    const auto w{ static_cast<float>(m_stdBshFile->bshTextures[t_id]->width) };
    const auto h{ static_cast<float>(m_stdBshFile->bshTextures[t_id]->height) };

    auto screenPosition{ renderer::Utils::MapToIso(t_mapX, t_mapY, m_rotation) };
    screenPosition.y -= (h - 32.0f);

    if (t_id == 4)
    {
        screenPosition.y -= 20.0f;
    }

    const auto openGLTextureId{ m_stdBshFile->bshTextures[t_id]->textureId };

    m_renderer->RenderTile(
        renderer::Utils::GetModelMatrix(screenPosition, glm::vec2(w, h)),
        openGLTextureId,
        *context->window,
        *m_camera
    );
}

void mdcii::EditorState::Rotate()
{
    if (m_rotation == renderer::Rotation::DEG0)
    {
        m_rotation = renderer::Rotation::DEG90;
    }
    else if (m_rotation == renderer::Rotation::DEG90)
    {
        m_rotation = renderer::Rotation::DEG180;
    } else if (m_rotation == renderer::Rotation::DEG180)
    {
        m_rotation = renderer::Rotation::DEG270;
    } else if (m_rotation == renderer::Rotation::DEG270)
    {
        m_rotation = renderer::Rotation::DEG0;
    }
}
