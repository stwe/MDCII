#include <imgui.h>
#include <magic_enum.hpp>
#include "Map.h"
#include "Game.h"
#include "MapContent.h"
#include "Log.h"
#include "ogl/resource/ResourceManager.h"
#include "renderer/ImGuiTileRenderer.h"
#include "renderer/RenderUtils.h"
#include "renderer/TileRenderer.h"
#include "renderer/TextRenderer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::map::Map::Map(const std::string& t_filePath, std::shared_ptr<data::Buildings> t_buildings)
    : m_buildings{ std::move(t_buildings) }
{
    Log::MDCII_LOG_DEBUG("[Map::Map()] Create Map.");

    Init(t_filePath);

    CreateGridEntities();
    CreateBuildingEntities();
}

mdcii::map::Map::~Map() noexcept
{
    Log::MDCII_LOG_DEBUG("[Map::~Map()] Destruct Map.");
}

//-------------------------------------------------
// Render
//-------------------------------------------------

void mdcii::map::Map::Render(const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    RenderGridEntities(t_window, t_camera);
    RenderBuildingEntities(t_window, t_camera);
}

void mdcii::map::Map::RenderImGui()
{
    ImGui::Begin("Map");

    // selected

    if (selectedIndex > INVALID)
    {
        renderer::ImGuiTileRenderer::RenderTileBauGfxImGui(
            m_buildings->buildingsMap.at(mapContent->mapTiles.at(selectedIndex).buildingId),
            *bauhausBshFile
        );
    }

    ImGui::End();
}

void mdcii::map::Map::RenderBuilding(
    const ogl::Window& t_window,
    const camera::Camera& t_camera,
    const int t_gfx,
    glm::vec2 t_screenPosition,
    const float t_elevation,
    const bool t_selected
) const
{
    const auto w{ static_cast<float>(stdBshFile->bshTextures[t_gfx]->width) };
    const auto h{ static_cast<float>(stdBshFile->bshTextures[t_gfx]->height) };
    const auto textureId{ stdBshFile->bshTextures[t_gfx]->textureId };

    t_screenPosition.y -= h - TILE_HEIGHT;
    t_screenPosition.y -= t_elevation;

    renderer->RenderTile(
        renderer::RenderUtils::GetModelMatrix(t_screenPosition, glm::vec2(w, h)),
        textureId,
        t_window, t_camera,
        t_selected
    );
}

//-------------------------------------------------
// Rotate
//-------------------------------------------------

void mdcii::map::Map::Rotate(const ChangeRotation t_changeRotation)
{
    switch (t_changeRotation)
    {
    case ChangeRotation::LEFT:
        --rotation;
        SortEntities();
        break;
    case ChangeRotation::RIGHT:
        ++rotation;
        SortEntities();
        break;
    }
}

const char* mdcii::map::Map::ShowCurrentRotation() const
{
    return magic_enum::enum_name(rotation).data();
}

//-------------------------------------------------
// Select
//-------------------------------------------------

void mdcii::map::Map::SelectTile(const glm::ivec2& t_position)
{
    if (IsPositionInMap(t_position))
    {
        selectedIndex = GetMapIndex(t_position.x, t_position.y);
        const auto& mapTile{ mapContent->mapTiles.at(selectedIndex) };
        if (!registry.all_of<ecs::SelectedComponent>(mapTile.entity))
        {
            registry.emplace<ecs::SelectedComponent>(mapTile.entity, selectedIndex);
        }
    }
    else
    {
        selectedIndex = INVALID;
    }
}

//-------------------------------------------------
// Utils
//-------------------------------------------------

glm::vec2 mdcii::map::Map::MapToIso(const int t_mapX, const int t_mapY, const Rotation t_rotation) const
{
    const auto position{ RotateMapPosition(t_mapX, t_mapY, t_rotation) };

    return {
        (position.x - position.y) * TILE_WIDTH_HALF,
        (position.x + position.y) * TILE_HEIGHT_HALF
    };
}

bool mdcii::map::Map::IsPositionInMap(const glm::ivec2& t_position) const
{
    if (t_position.x >= 0 && t_position.x < mapContent->width &&
        t_position.y >= 0 && t_position.y < mapContent->height)
    {
        return true;
    }

    return false;
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::map::Map::Init(const std::string& t_filePath)
{
    Log::MDCII_LOG_DEBUG("[Map::Init()] Initializing map.");

    // load map content
    mapContent = std::make_unique<MapContent>(t_filePath);

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
// Map helper
//-------------------------------------------------

glm::ivec2 mdcii::map::Map::RotateMapPosition(const int t_mapX, const int t_mapY, const Rotation t_rotation) const
{
    auto x{ t_mapX };
    auto y{ t_mapY };

    switch (t_rotation)
    {
    case Rotation::DEG0:
        break;
    case Rotation::DEG90:
        x = mapContent->width - t_mapY - 1;
        y = t_mapX;
        break;
    case Rotation::DEG180:
        x = mapContent->width - t_mapX - 1;
        y = mapContent->height - t_mapY - 1;
        break;
    case Rotation::DEG270:
        x = t_mapY;
        y = mapContent->height - t_mapX - 1;
        break;
    }

    return { x, y };
}

int mdcii::map::Map::GetMapIndex(const int t_mapX, const int t_mapY, const Rotation t_rotation) const
{
    const auto position{ RotateMapPosition(t_mapX, t_mapY, t_rotation) };

    return position.y * mapContent->width + position.x;
}

//-------------------------------------------------
// Entities
//-------------------------------------------------

void mdcii::map::Map::CreateGridEntities()
{
    for (auto y{ 0 }; y < mapContent->height; ++y)
    {
        for (auto x{ 0 }; x < mapContent->width; ++x)
        {
            CreateGridEntity(x, y);
        }
    }
}

void mdcii::map::Map::CreateBuildingEntities()
{
    for (auto y{ 0 }; y < mapContent->height; ++y)
    {
        for (auto x{ 0 }; x < mapContent->width; ++x)
        {
            CreateBuildingEntity(x, y);
        }
    }

    SortEntities();
}

//-------------------------------------------------
// Create Entity
//-------------------------------------------------

void mdcii::map::Map::CreateGridEntity(const int t_mapX, const int t_mapY)
{
    // create entity
    const entt::entity entity{ registry.create() };

    // get map tile
    auto& mapTile{ mapContent->mapTiles.at(GetMapIndex(t_mapX, t_mapY)) };

    // store entity handle also in the MapTile
    mapTile.gridEntity = entity;

    // a screen position for each rotation
    const auto s0{ MapToIso(t_mapX, t_mapY, Rotation::DEG0) };
    const auto s90{ MapToIso(t_mapX, t_mapY, Rotation::DEG90) };
    const auto s180{ MapToIso(t_mapX, t_mapY, Rotation::DEG180) };
    const auto s270{ MapToIso(t_mapX, t_mapY, Rotation::DEG270) };
    std::vector screenPositions{ s0, s90, s180, s270 };

    // add GridComponent
    registry.emplace<ecs::GridComponent>(
        entity,
        t_mapX, t_mapY,
        TILE_WIDTH, TILE_HEIGHT,
        ogl::resource::ResourceManager::LoadTexture("textures/red.png").id,
        screenPositions
    );
}

void mdcii::map::Map::CreateBuildingEntity(const int t_mapX, const int t_mapY)
{
    // an index for each rotation needed for sorting
    const auto idx0{ GetMapIndex(t_mapX, t_mapY, Rotation::DEG0) };
    const auto idx90{ GetMapIndex(t_mapX, t_mapY, Rotation::DEG90) };
    const auto idx180{ GetMapIndex(t_mapX, t_mapY, Rotation::DEG180) };
    const auto idx270{ GetMapIndex(t_mapX, t_mapY, Rotation::DEG270) };
    std::vector indices{ idx0, idx90, idx180, idx270 };

    // a screen position for each rotation
    const auto s0{ MapToIso(t_mapX, t_mapY, Rotation::DEG0) };
    const auto s90{ MapToIso(t_mapX, t_mapY, Rotation::DEG90) };
    const auto s180{ MapToIso(t_mapX, t_mapY, Rotation::DEG180) };
    const auto s270{ MapToIso(t_mapX, t_mapY, Rotation::DEG270) };
    std::vector screenPositions{ s0, s90, s180, s270 };

    // get map tile
    auto& mapTile{ mapContent->mapTiles.at(idx0) };
    if (mapTile.buildingId == INVALID)
    {
        return;
    }

    // create an entity
    const auto entity{ registry.create() };

    // store entity handle
    mapTile.entity = entity;

    // get building
    const auto building{ m_buildings->buildingsMap.at(mapTile.buildingId) };

    // ---------------- building component

    registry.emplace<ecs::BuildingComponent>(
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

    registry.emplace<ecs::PositionComponent>(
        entity,
        t_mapX, t_mapY,
        indices, screenPositions, gfxs
    );
}

//-------------------------------------------------
// Render Entities
//-------------------------------------------------

void mdcii::map::Map::RenderGridEntities(const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    if (!renderGrid && !renderText)
    {
        return;
    }

    const auto view{ registry.view<const ecs::GridComponent>() };
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
        const auto view{ registry.view<
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
        const auto viewSelected{ registry.view<
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

    RenderBuilding(
        t_window,
        t_camera,
        gfx,
        t_positionComponent.screenPositions[rot],
        static_cast<float>(t_buildingComponent.building.posoffs),
        t_selected
    );
}

//-------------------------------------------------
// Sort Entities
//-------------------------------------------------

void mdcii::map::Map::SortEntities()
{
    auto i{ magic_enum::enum_integer(rotation) };

    registry.sort<ecs::PositionComponent>([i](const auto& t_lhs, const auto& t_rhs)
    {
        return t_lhs.indices[i] < t_rhs.indices[i];
    });
}
