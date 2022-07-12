#include <imgui.h>
#include <magic_enum.hpp>
#include "Map.h"
#include "Game.h"
#include "MapContent.h"
#include "Log.h"
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
    RenderEntities(t_window, t_camera);
}

void mdcii::map::Map::RenderImGui()
{
    ImGui::Begin("Map");

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

    t_screenPosition.y -= h - MapTile::TILE_HEIGHT;
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

void mdcii::map::Map::Rotate(const ChangeRotation t_changeRotation) const
{
    switch (t_changeRotation)
    {
    case ChangeRotation::LEFT:
        --mapContent->rotation;
        mapContent->SortEntitiesOfAllLayers();
        break;
    case ChangeRotation::RIGHT:
        ++mapContent->rotation;
        mapContent->SortEntitiesOfAllLayers();
        break;
    }
}

const char* mdcii::map::Map::ShowCurrentRotation() const
{
    return magic_enum::enum_name(mapContent->rotation).data();
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::map::Map::Init(const std::string& t_filePath)
{
    Log::MDCII_LOG_DEBUG("[Map::Init()] Initializing map.");

    // load map content
    mapContent = std::make_unique<MapContent>(t_filePath, m_buildings);

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
// Render Entities
//-------------------------------------------------

void mdcii::map::Map::RenderGridEntities(const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    if (!renderGrid && !renderText)
    {
        return;
    }

    const auto view{ Game::ecs.view<const ecs::GridComponent>() };
    for (const auto entity : view)
    {
        const auto& gc{ view.get<const ecs::GridComponent>(entity) };
        const auto screenPosition{ gc.mapTile.screenPositions[magic_enum::enum_integer(mapContent->rotation)] };

        if (renderGrid)
        {
            renderer->RenderTile(
                renderer::RenderUtils::GetModelMatrix(
                    screenPosition,
                    glm::vec2(MapTile::TILE_WIDTH, MapTile::TILE_HEIGHT)
                ),
                gc.textureId,
                t_window, t_camera
            );
        }

        if (renderText)
        {
            textRenderer->RenderText(
                std::to_string(gc.mapTile.mapX).append(", ").append(std::to_string(gc.mapTile.mapY)),
                screenPosition.x + static_cast<float>(MapTile::TILE_WIDTH) / 4.0f,
                screenPosition.y + static_cast<float>(MapTile::TILE_HEIGHT) / 4.0f,
                0.25f,
                glm::vec3(1.0f, 0.0f, 0.0f),
                t_window, t_camera
            );
        }
    }
}

void mdcii::map::Map::RenderEntities(const ogl::Window& t_window, const camera::Camera& t_camera) const
{
    if (renderTerrainLayer)
    {
        const auto view{ Game::ecs.view<const ecs::TileComponent, const ecs::TerrainLayerComponent>() };
        for (const auto entity : view.use<const ecs::TileComponent>())
        {
            const auto& [tileComponent, tc] { view.get(entity) };
            RenderEntity(t_window, t_camera, tileComponent, false);
        }
    }

    if (renderBuildingsLayer)
    {
        const auto view{ Game::ecs.view<const ecs::TileComponent, const ecs::BuildingsLayerComponent>() };
        for (const auto entity : view.use<const ecs::TileComponent>())
        {
            const auto& [tileComponent, bc] { view.get(entity) };
            RenderEntity(t_window, t_camera, tileComponent, false);
        }
    }
}

void mdcii::map::Map::RenderEntity(
    const ogl::Window& t_window,
    const camera::Camera& t_camera,
    const ecs::TileComponent& t_tileComponent,
    const bool t_selected
) const
{
    // get gfx in the right direction for the current map rotation
    const auto rot{ magic_enum::enum_integer(mapContent->rotation) };
    auto orientation{ t_tileComponent.mapTile.orientation };
    if (t_tileComponent.building.rotate > 0)
    {
        orientation = (orientation + rot) % 4;
    }
    auto gfx{ t_tileComponent.mapTile.gfxs[orientation] };

    if (t_tileComponent.building.size.w > 1)
    {
        // y * width + x
        const auto offset{ t_tileComponent.mapTile.y * t_tileComponent.building.size.w + t_tileComponent.mapTile.x };
        gfx += offset;
    }

    RenderBuilding(
        t_window,
        t_camera,
        gfx,
        t_tileComponent.mapTile.screenPositions[rot],
        static_cast<float>(t_tileComponent.building.posoffs),
        t_selected
    );
}
