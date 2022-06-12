#include <imgui.h>
#include <magic_enum.hpp>
#include "SandboxState.h"
#include "Game.h"
#include "Log.h"
#include "ogl/OpenGL.h"
#include "file/BshFile.h"
#include "renderer/RenderUtils.h"
#include "renderer/TileRenderer.h"
#include "renderer/TextRenderer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::SandboxState::SandboxState(const Id t_id, state::StateStack* t_stateStack, std::shared_ptr<Context> t_context)
    : State(t_id, t_stateStack, std::move(t_context))
{
    Log::MDCII_LOG_DEBUG("[SandboxState::SandboxState()] Create SandboxState.");

    Init();
}

mdcii::SandboxState::~SandboxState() noexcept
{
    Log::MDCII_LOG_DEBUG("[SandboxState::~SandboxState()] Destruct SandboxState.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::SandboxState::Input()
{
    if (context->window->IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        Log::MDCII_LOG_INFO("[SandboxState::Input()] Starts POP SandboxState.");
        RequestStackPop();
    }
}

void mdcii::SandboxState::Update()
{
}

void mdcii::SandboxState::Render()
{
    const auto view{ m_registry.view<PositionComponent, GfxComponent>() };

    for (auto [entity, pc, gc] : view.each())
    {
        auto w{ 0.0f };
        auto h{ 0.0f };
        auto screenPosition{ glm::vec2() };
        uint32_t textureId{ 0 };

        switch (m_rotation)
        {
        case map::Rotation::DEG0:
            w = static_cast<float>(m_stdBshFile->bshTextures[gc.gfx0]->width);
            h = static_cast<float>(m_stdBshFile->bshTextures[gc.gfx0]->height);
            screenPosition = pc.s0;
            textureId = m_stdBshFile->bshTextures[gc.gfx0]->textureId;
            break;
        case map::Rotation::DEG90:
            w = static_cast<float>(m_stdBshFile->bshTextures[gc.gfx90]->width);
            h = static_cast<float>(m_stdBshFile->bshTextures[gc.gfx90]->height);
            screenPosition = pc.s90;
            textureId = m_stdBshFile->bshTextures[gc.gfx90]->textureId;
            break;
        case map::Rotation::DEG180:
            w = static_cast<float>(m_stdBshFile->bshTextures[gc.gfx180]->width);
            h = static_cast<float>(m_stdBshFile->bshTextures[gc.gfx180]->height);
            screenPosition = pc.s180;
            textureId = m_stdBshFile->bshTextures[gc.gfx180]->textureId;
            break;
        case map::Rotation::DEG270:
            w = static_cast<float>(m_stdBshFile->bshTextures[gc.gfx270]->width);
            h = static_cast<float>(m_stdBshFile->bshTextures[gc.gfx270]->height);
            screenPosition = pc.s270;
            textureId = m_stdBshFile->bshTextures[gc.gfx270]->textureId;
            break;
        }

        screenPosition.y -= h - TILE_HEIGHT;
        if (pc.higher)
        {
            screenPosition.y -= 20.0f;
        }

        m_renderer->RenderTile(
            renderer::RenderUtils::GetModelMatrix(screenPosition, glm::vec2(w, h)),
            textureId,
            *context->window, *context->camera
        );
    }
}

void mdcii::SandboxState::RenderImGui()
{
    ogl::Window::ImGuiBegin();

    ImGui::Begin("SandboxState");

    ImGui::Text("Current rotation: %s", magic_enum::enum_name(m_rotation).data());
    if (ImGui::Button("Rotate right"))
    {
        ++m_rotation;
        SortEntities();
    }
    if (ImGui::Button("Rotate left"))
    {
        --m_rotation;
        SortEntities();
    }

    ImGui::End();

    ogl::Window::ImGuiEnd();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::SandboxState::Init()
{
    Log::MDCII_LOG_DEBUG("[SandboxState::Init()] Initializing sandbox state.");

    m_paletteFile = std::make_unique<file::PaletteFile>(Game::RESOURCES_PATH + "STADTFLD.COL");
    m_paletteFile->ReadDataFromChunks();

    m_stdBshFile = std::make_unique<file::BshFile>(Game::RESOURCES_PATH + "STADTFLD.BSH", m_paletteFile->palette);
    m_stdBshFile->ReadDataFromChunks();

    m_renderer = std::make_unique<renderer::TileRenderer>();
    m_textRenderer = std::make_unique<renderer::TextRenderer>(Game::RESOURCES_PATH + "bitter/Bitter-Regular.otf");

    CreateMapEntities();

    Log::MDCII_LOG_DEBUG("[SandboxState::Init()] The sandbox state was successfully initialized.");
}

void mdcii::SandboxState::CreateMapEntities()
{
    auto getMapIndex = [](const int t_x, const int t_y)
    {
        return t_y * WIDTH + t_x;
    };

    auto mapToIso = [](const int t_x, const int t_y)
    {
        return glm::vec2(
            (t_x - t_y) * TILE_WIDTH_HALF,
            (t_x + t_y) * TILE_HEIGHT_HALF
        );
    };

    for (auto y{ 0 }; y < HEIGHT; ++y)
    {
        for (auto x{ 0 }; x < WIDTH; ++x)
        {
            const auto entity{ m_registry.create() };

            auto idx0{ getMapIndex(x, y) };
            auto idx90{ getMapIndex(WIDTH - y - 1, x) };
            auto idx180{ getMapIndex(WIDTH - x - 1, HEIGHT - y - 1) };
            auto idx270{ getMapIndex(y, HEIGHT - x - 1) };

            auto s0{ mapToIso(x, y) };
            auto s90{ mapToIso(WIDTH - y - 1, x) };
            auto s180{ mapToIso(WIDTH - x - 1, HEIGHT - y - 1) };
            auto s270{ mapToIso(y, HEIGHT - x - 1) };

            auto gfx{ m_map.at(idx0) };
            auto higher{ false };

            // todo: hard coded
            if (gfx == 1320 || gfx == 4)
            {
                higher = true;
            }

            m_registry.emplace<PositionComponent>(
                entity,
                x, y,
                idx0, idx90, idx180, idx270,
                s0, s90, s180, s270,
                higher
            );

            m_registry.emplace<GfxComponent>(entity, gfx, gfx + 1, gfx + 2, gfx + 3);
        }
    }

    SortEntities();
}

void mdcii::SandboxState::SortEntities()
{
    switch (m_rotation)
    {
    case map::Rotation::DEG0:
        m_registry.sort<PositionComponent>([](const auto& t_lhs, const auto& t_rhs)
        {
            return t_lhs.idx0 < t_rhs.idx0;
        });
        break;
    case map::Rotation::DEG90:
        m_registry.sort<PositionComponent>([](const auto& t_lhs, const auto& t_rhs)
        {
            return t_lhs.idx90 < t_rhs.idx90;
        });
        break;
    case map::Rotation::DEG180:
        m_registry.sort<PositionComponent>([](const auto& t_lhs, const auto& t_rhs)
        {
            return t_lhs.idx180 < t_rhs.idx180;
        });
        break;
    case map::Rotation::DEG270:
        m_registry.sort<PositionComponent>([](const auto& t_lhs, const auto& t_rhs)
        {
            return t_lhs.idx270 < t_rhs.idx270;
        });
        break;
    }
}
