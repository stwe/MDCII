#include <imgui.h>
#include <magic_enum.hpp>
#include "Map.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "ogl/resource/ResourceManager.h"
#include "renderer/ImGuiTileRenderer.h"
#include "renderer/RenderUtils.h"
#include "renderer/TileRenderer.h"
#include "renderer/TextRenderer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::map::Map::Map(
    std::vector<MapTile> t_mapTiles,
    const int t_width, const int t_height,
    std::shared_ptr<data::Buildings> t_buildings
)
    : mapTiles{ std::move(t_mapTiles) }
    , width{ t_width }
    , height{ t_height }
    , m_buildings{ std::move(t_buildings) }
{
    Log::MDCII_LOG_DEBUG("[Map::Map()] Create Map.");

    MDCII_ASSERT(!mapTiles.empty(), "[Map::Map()] Empty map given.")
    MDCII_ASSERT(width, "[Map::Map()] Invalid width.")
    MDCII_ASSERT(height, "[Map::Map()] Invalid height.")
    MDCII_ASSERT((static_cast<size_t>(width) * static_cast<size_t>(height)) == mapTiles.size(), "[Map::Map()] Invalid size.")

    Init();

    CreateGridEntities();
    CreateBuildingEntities();
}

mdcii::map::Map::~Map() noexcept
{
    Log::MDCII_LOG_DEBUG("[Map::~Map()] Destruct Map.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::map::Map::Render(const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    RenderGridEntities(t_window, t_camera);
    RenderBuildingEntities(t_window, t_camera);
}

void mdcii::map::Map::RenderImGui()
{
    ImGui::Begin("Map");

    // render options

    ImGui::Checkbox("Render grid", &renderGrid);
    ImGui::Checkbox("Render buildings", &renderBuildings);
    ImGui::Checkbox("Render text", &renderText);

    ImGui::Separator();

    // rotation

    ImGui::Text("Current rotation: %s", magic_enum::enum_name(rotation).data());
    if (ImGui::Button("Rotate right"))
    {
        ++rotation;
        SortEntities();
    }
    if (ImGui::Button("Rotate left"))
    {
        --rotation;
        SortEntities();
    }

    ImGui::Separator();

    // selected

    if (selectedIndex > INVALID)
    {
        renderer::ImGuiTileRenderer::RenderTileBauGfxImGui(
            m_buildings->buildingsMap.at(mapTiles.at(selectedIndex).buildingId),
            *bauhausBshFile
        );
    }

    ImGui::End();
}

//-------------------------------------------------
// Utils
//-------------------------------------------------

glm::ivec2 mdcii::map::Map::RotateMapPosition(const int t_mapX, const int t_mapY, const Rotation t_rotation) const
{
    int x{ t_mapX };
    int y{ t_mapY };

    switch (t_rotation)
    {
    case Rotation::DEG0:
        x = t_mapX;
        y = t_mapY;
        break;
    case Rotation::DEG90:
        x = width - t_mapY - 1;
        y = t_mapX;
        break;
    case Rotation::DEG180:
        x = width - t_mapX - 1;
        y = height - t_mapY - 1;
        break;
    case Rotation::DEG270:
        x = t_mapY;
        y = height - t_mapX - 1;
        break;
    }

    return { x, y };
}

glm::vec2 mdcii::map::Map::MapToIso(const int t_mapX, const int t_mapY, const Rotation t_rotation) const
{
    const auto position{ RotateMapPosition(t_mapX, t_mapY, t_rotation) };

    return {
        (position.x - position.y) * TILE_WIDTH_HALF,
        (position.x + position.y) * TILE_HEIGHT_HALF
    };
}

int mdcii::map::Map::GetMapIndex(const int t_mapX, const int t_mapY, const Rotation t_rotation) const
{
    const auto position{ RotateMapPosition(t_mapX, t_mapY, t_rotation) };

    return position.y * width + position.x;
}

void mdcii::map::Map::SelectTile(const glm::ivec2& t_position, const int t_index)
{
    if (t_position.x >= 0 && t_position.x < width &&
        t_position.y >= 0 && t_position.y < height &&
        t_index > INVALID)
    {
        selectedIndex = t_index;
        const auto& mapTile{ mapTiles.at(selectedIndex) };
        if (!m_registry.all_of<ecs::SelectedComponent>(mapTile.entity))
        {
            m_registry.emplace<ecs::SelectedComponent>(mapTile.entity, t_index);
        }
    }
    else
    {
        selectedIndex = INVALID;
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::map::Map::Init()
{
    Log::MDCII_LOG_DEBUG("[Map::Init()] Initializing map.");

    // load palette from stadtfld.col
    m_paletteFile = std::make_unique<file::PaletteFile>(Game::RESOURCES_PATH + "STADTFLD.COL");
    m_paletteFile->ReadDataFromChunks();

    // load bsh graphics from stadtfld.bsh
    stdBshFile = std::make_unique<file::BshFile>(Game::RESOURCES_PATH + "STADTFLD.BSH", m_paletteFile->palette);
    stdBshFile->ReadDataFromChunks();

    // load bsh graphics from bauhaus.bsh
    bauhausBshFile = std::make_unique<file::BshFile>(Game::RESOURCES_PATH + "BAUHAUS.BSH", m_paletteFile->palette);
    bauhausBshFile->ReadDataFromChunks();

    // create tile renderer
    renderer = std::make_unique<renderer::TileRenderer>();

    // create text renderer
    textRenderer = std::make_unique<renderer::TextRenderer>(Game::RESOURCES_PATH + "bitter/Bitter-Regular.otf");

    Log::MDCII_LOG_DEBUG("[Map::Init()] The map was successfully initialized.");
}

//-------------------------------------------------
// Entities
//-------------------------------------------------

void mdcii::map::Map::CreateGridEntities()
{
    for (auto y{ 0 }; y < height; ++y)
    {
        for (auto x{ 0 }; x < width; ++x)
        {
            // create entity
            const entt::entity entity{ m_registry.create() };

            // get map tile
            auto& mapTile{ mapTiles.at(GetMapIndex(x, y, Rotation::DEG0)) };

            // store entity handle also in the MapTile
            mapTile.gridEntity = entity;

            // a screen position for each rotation
            const auto s0{ MapToIso(x, y, Rotation::DEG0) };
            const auto s90{ MapToIso(x, y, Rotation::DEG90) };
            const auto s180{ MapToIso(x, y, Rotation::DEG180) };
            const auto s270{ MapToIso(x, y, Rotation::DEG270) };
            std::vector screenPositions{ s0, s90, s180, s270 };

            // add GridComponent
            m_registry.emplace<ecs::GridComponent>(
                entity,
                x, y,
                TILE_WIDTH, TILE_HEIGHT,
                ogl::resource::ResourceManager::LoadTexture("textures/red.png").id,
                screenPositions
            );
        }
    }
}

void mdcii::map::Map::CreateBuildingEntities()
{
    for (auto y{ 0 }; y < height; ++y)
    {
        for (auto x{ 0 }; x < width; ++x)
        {
            // create an entity
            const auto entity{ m_registry.create() };

            // an index for each rotation needed for sorting
            const auto idx0{ GetMapIndex(x, y, Rotation::DEG0) };
            const auto idx90{ GetMapIndex(x, y, Rotation::DEG90) };
            const auto idx180{ GetMapIndex(x, y, Rotation::DEG180) };
            const auto idx270{ GetMapIndex(x, y, Rotation::DEG270) };
            std::vector indices{ idx0, idx90, idx180, idx270 };

            // a screen position for each rotation
            const auto s0{ MapToIso(x, y, Rotation::DEG0) };
            const auto s90{ MapToIso(x, y, Rotation::DEG90) };
            const auto s180{ MapToIso(x, y, Rotation::DEG180) };
            const auto s270{ MapToIso(x, y, Rotation::DEG270) };
            std::vector screenPositions{ s0, s90, s180, s270 };

            // get map tile
            auto& mapTile{ mapTiles.at(idx0) };

            // store entity handle also in the MapTile
            mapTile.entity = entity;

            // get building
            const auto building{ m_buildings->buildingsMap.at(mapTile.buildingId) };

            // ---------------- building component

            if (mapTile.buildingId == INVALID)
            {
                continue;
            }

            m_registry.emplace<ecs::BuildingComponent>(
                entity,
                mapTile,
                building
            );

            // ---------------- position component

            const auto gfx0{ building.gfx };

            std::vector<int> gfxs;

            /**
             * gfx0 = 1370 / building.rotation = 4
             *
             * orientation0 = 1370
             * orientation1 = 1370 + (1 * 4)
             * orientation2 = 1370 + (2 * 4)
             * orientation3 = 1370 + (3 * 4)
             */

            gfxs.push_back(gfx0);
            if (building.rotate > 0)
            {
                gfxs.push_back(gfx0 + (1 * building.rotate));
                gfxs.push_back(gfx0 + (2 * building.rotate));
                gfxs.push_back(gfx0 + (3 * building.rotate));
            }

            m_registry.emplace<ecs::PositionComponent>(
                entity,
                x, y,
                indices, screenPositions, gfxs
            );
        }
    }

    SortEntities();
}

void mdcii::map::Map::RenderGridEntities(const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    const auto view{ m_registry.view<const ecs::GridComponent>() };
    for (const auto entity : view)
    {
        const auto& gc{ view.get<const ecs::GridComponent>(entity) };
        const auto screenPosition{ gc.screenPositions[magic_enum::enum_integer(rotation)] };

        if (renderGrid)
        {
            renderer->RenderTile(
                renderer::RenderUtils::GetModelMatrix(
                    screenPosition,
                    glm::vec2(gc.tileWidth, gc.tileHeight)
                ),
                gc.textureId,
                t_window, t_camera
            );
        }

        if (renderText)
        {
            textRenderer->RenderText(
                std::to_string(gc.mapX).append(", ").append(std::to_string(gc.mapY)),
                screenPosition.x + static_cast<float>(gc.tileWidth) / 4.0f,
                screenPosition.y + static_cast<float>(gc.tileHeight) / 4.0f,
                0.25f,
                glm::vec3(1.0f, 0.0f, 0.0f),
                t_window, t_camera
            );
        }
    }
}

void mdcii::map::Map::RenderBuildingEntities(const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    if (renderBuildings)
    {
        // render tile
        const auto view{ m_registry.view<
            const ecs::PositionComponent,
            const ecs::BuildingComponent>()
        };
        for (const auto entity : view.use<const ecs::PositionComponent>())
        {
            const auto& [positionComponent, buildingComponent] { view.get(entity) };

            RenderBuildingEntity(
                t_window, t_camera,
                positionComponent, buildingComponent,
                false
            );
        }

        // render tile as selected
        const auto viewSelected{ m_registry.view<
            const ecs::PositionComponent,
            const ecs::BuildingComponent,
            const ecs::SelectedComponent>()
        };
        for (const auto entity : viewSelected.use<const ecs::PositionComponent>())
        {
            const auto& [positionComponent, buildingComponent, selectedComponent] { viewSelected.get(entity) };

            RenderBuildingEntity(
                t_window, t_camera,
                positionComponent, buildingComponent,
                true
            );
        }
    }
}

void mdcii::map::Map::RenderBuildingEntity(
    const ogl::Window& t_window,
    const camera::Camera& t_camera,
    const ecs::PositionComponent& t_positionComponent,
    const ecs::BuildingComponent& t_buildingComponent,
    const bool t_selected
) const
{
    // get gfx in the right direction for the current map rotation
    const auto rot{ magic_enum::enum_integer(rotation) };
    auto orientation{ t_buildingComponent.mapTile.orientation };
    if (t_buildingComponent.building.rotate > 0)
    {
        orientation = (orientation + rot) % 4;
    }
    auto gfx{ t_positionComponent.gfx[orientation] };

    if (t_buildingComponent.mapTile.width > 1)
    {
        // y * width + x
        const auto offset{ t_buildingComponent.mapTile.y * t_buildingComponent.mapTile.width + t_buildingComponent.mapTile.x };
        gfx += offset;
    }

    // get width, height and texture of the gfx
    const auto w{ static_cast<float>(stdBshFile->bshTextures[gfx]->width) };
    const auto h{ static_cast<float>(stdBshFile->bshTextures[gfx]->height) };
    const auto textureId{ stdBshFile->bshTextures[gfx]->textureId };

    // get the screen position of the gfx, which is different for each map rotation
    auto screenPosition{ t_positionComponent.screenPositions[rot] };
    screenPosition.y -= h - TILE_HEIGHT;
    if (t_buildingComponent.building.posoffs == ELEVATION)
    {
        screenPosition.y -= ELEVATION;
    }

    // render tile
    renderer->RenderTile(
        renderer::RenderUtils::GetModelMatrix(screenPosition, glm::vec2(w, h)),
        textureId,
        t_window, t_camera,
        t_selected
    );
}

void mdcii::map::Map::SortEntities()
{
    auto i{ magic_enum::enum_integer(rotation) };

    m_registry.sort<ecs::PositionComponent>([i](const auto& t_lhs, const auto& t_rhs)
    {
        return t_lhs.indices[i] < t_rhs.indices[i];
    });
}
