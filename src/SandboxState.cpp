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
    const auto view{ m_registry.view<BuildingComponent, PositionComponent>() };

    for (const entt::entity entity: view.use<PositionComponent>())
    {
        auto [bc, pc] = view.get(entity);

        // get gfx in the right direction for the current map rotation
        const auto rotation{ magic_enum::enum_integer(m_rotation) };
        auto direction{ bc.tileAsset.direction };
        if (bc.building.rotate > 0)
        {
            direction = (direction + rotation) % 4;
        }
        const auto gfx{ pc.gfx[direction] };

        // get width, height and texture of the gfx
        const auto w{ static_cast<float>(m_stdBshFile->bshTextures[gfx]->width) };
        const auto h{ static_cast<float>(m_stdBshFile->bshTextures[gfx]->height) };
        const auto textureId{ m_stdBshFile->bshTextures[gfx]->textureId };

        // get the screen position of the gfx, which is different for each map rotation
        auto screenPosition{ pc.screenPositions[magic_enum::enum_integer(m_rotation)] };
        screenPosition.y -= h - TILE_HEIGHT;
        if (bc.building.posoffs == 20)
        {
            screenPosition.y -= 20.0f;
        }

        // render tile
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
            // create entity
            const auto entity{ m_registry.create() };

            // an index for each rotation for sorting
            const auto idx0{ getMapIndex(x, y) };
            const auto idx90{ getMapIndex(WIDTH - y - 1, x) };
            const auto idx180{ getMapIndex(WIDTH - x - 1, HEIGHT - y - 1) };
            const auto idx270{ getMapIndex(y, HEIGHT - x - 1) };

            // a screen position for each rotation
            const auto s0{ mapToIso(x, y) };
            const auto s90{ mapToIso(WIDTH - y - 1, x) };
            const auto s180{ mapToIso(WIDTH - x - 1, HEIGHT - y - 1) };
            const auto s270{ mapToIso(y, HEIGHT - x - 1) };

            // get tile asset from map
            auto tileAsset{ m_map.at(idx0) };

            // ---------------- building component

            if (tileAsset.buildingId == -1)
            {
                continue;
            }

            m_registry.emplace<BuildingComponent>(
                entity,
                tileAsset,
                context->buildings->buildingsMap.at(tileAsset.buildingId)
            );

            // ---------------- position component

            const auto building{ context->buildings->buildingsMap.at(tileAsset.buildingId) };
            const auto gfx0{ building.gfx };

            std::vector i{ idx0, idx90, idx180, idx270 };
            std::vector s{ s0, s90, s180, s270 };
            std::vector<int> g;
            g.push_back(gfx0);
            if (building.rotate > 0)
            {
                g.push_back(gfx0 + 1);
                g.push_back(gfx0 + 2);
                g.push_back(gfx0 + 3);
            }

            m_registry.emplace<PositionComponent>(
                entity,
                x, y,
                i, s, g
            );
        }
    }

    SortEntities();
}

void mdcii::SandboxState::SortEntities()
{
    auto i{ magic_enum::enum_integer(m_rotation) };

    m_registry.sort<PositionComponent>([i](const auto& t_lhs, const auto& t_rhs)
    {
        return t_lhs.indices[i] < t_rhs.indices[i];
    });
}
