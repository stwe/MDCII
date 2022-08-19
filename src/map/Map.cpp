// This file is part of the MDCII project.
//
// Copyright (c) 2022. stwe <https://github.com/stwe/MDCII>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <imgui.h>
#include "Map.h"
#include "Game.h"
#include "MousePicker.h"
#include "MapContent.h"
#include "MdciiAssert.h"
#include "ecs/Components.h"
#include "file/OriginalResourcesManager.h"
#include "state/State.h"
#include "renderer/TileRenderer.h"
#include "renderer/TextRenderer.h"
#include "renderer/RenderUtils.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::map::Map::Map(const std::string& t_mapFilePath, std::shared_ptr<state::Context> t_context)
    : context{ std::move(t_context) }
{
    Log::MDCII_LOG_DEBUG("[Map::Map()] Create Map.");

    Init(t_mapFilePath);
}

mdcii::map::Map::~Map() noexcept
{
    Log::MDCII_LOG_DEBUG("[Map::~Map()] Destruct Map.");
}

//-------------------------------------------------
// Render
//-------------------------------------------------

void mdcii::map::Map::Render() const
{
    RenderGridEntities();
    RenderEntities();

    mousePicker->Render(*context->window, *context->camera);
}

void mdcii::map::Map::RenderImGui()
{
    // Map stuff
    ImGui::Begin("Map");

    ImGui::Checkbox("Render grid", &renderGrid);
    ImGui::Checkbox("Render grid coords", &renderText);
    ImGui::Separator();
    ImGui::Checkbox("Render terrain layer", &renderTerrainLayer);
    ImGui::SameLine();
    ImGui::Checkbox("Skip buildings layer tiles", &skipBuildingsLayerTiles);
    ImGui::Separator();
    ImGui::Checkbox("Render buildings layer", &renderBuildingsLayer);

    ImGui::End();

    // MapContent entities
    mapContent->RenderImGui();

    // MousePicker Gui
    mousePicker->RenderImGui();
}

void mdcii::map::Map::RenderBuilding(
    const int t_gfx,
    glm::vec2 t_screenPosition,
    const float t_elevation,
    const bool t_selected
) const
{
    const auto& stadtfldBshTextures{ context->originalResourcesManager->GetStadtfldBshByZoom(mapContent->zoom) };

    const auto w{ static_cast<float>(stadtfldBshTextures[t_gfx]->width) };
    const auto h{ static_cast<float>(stadtfldBshTextures[t_gfx]->height) };
    const auto textureId{ stadtfldBshTextures[t_gfx]->textureId };

    t_screenPosition.y -= h - static_cast<float>(get_tile_height(mapContent->zoom));
    t_screenPosition.y -= t_elevation;

    renderer->RenderTile(
        renderer::RenderUtils::GetModelMatrix(t_screenPosition, glm::vec2(w, h)),
        textureId,
        *context->window, *context->camera,
        t_selected
    );
}

//-------------------------------------------------
// Rotate && Zoom
//-------------------------------------------------

void mdcii::map::Map::Rotate(const ChangeRotation t_changeRotation) const
{
    switch (t_changeRotation)
    {
    case ChangeRotation::LEFT:
        mapContent->RotateLeft();
        break;
    case ChangeRotation::RIGHT:
        mapContent->RotateRight();
        break;
    }
}

void mdcii::map::Map::Zoom(const ChangeZoom t_changeZoom) const
{
    switch (t_changeZoom)
    {
    case ChangeZoom::ZOOM_IN:
        mapContent->ZoomIn();
        break;
    case ChangeZoom::ZOOM_OUT:
        mapContent->ZoomOut();
        break;
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::map::Map::Init(const std::string& t_filePath)
{
    Log::MDCII_LOG_DEBUG("[Map::Init()] Initializing map.");

    // create the MousePicker to select tiles
    mousePicker = std::make_unique<MousePicker>(this, *context->window, *context->camera);

    // load map content
    mapContent = std::make_unique<MapContent>(t_filePath, context);
    mapContent->zoom = Zoom::GFX;

    // todo: the camera needs to know the current zoom
    context->camera->zoom = Zoom::GFX; // default

    // create tile renderer
    renderer = std::make_unique<renderer::TileRenderer>();

    // create text renderer
    textRenderer = std::make_unique<renderer::TextRenderer>(Game::RESOURCES_REL_PATH + "bitter/Bitter-Regular.otf");

    Log::MDCII_LOG_DEBUG("[Map::Init()] The map was successfully initialized.");
}

//-------------------------------------------------
// Render Entities
//-------------------------------------------------

void mdcii::map::Map::RenderGridEntities() const
{
    if (!renderGrid && !renderText)
    {
        return;
    }

    const auto view{ Game::ecs.view<const ecs::GridComponent>() };
    for (const auto entity : view)
    {
        const auto& gc{ view.get<const ecs::GridComponent>(entity) };
        const auto& screenPosition{ gc.mapTile.screenPositions.at(magic_enum::enum_integer(mapContent->zoom)).at(rotation_to_int(mapContent->rotation)) };

        if (renderGrid)
        {
            renderer->RenderTile(
                renderer::RenderUtils::GetModelMatrix(
                    screenPosition,
                    glm::vec2(get_tile_width(mapContent->zoom), get_tile_height(mapContent->zoom))
                ),
                gc.textureId,
                *context->window, *context->camera
            );
        }

        if (renderText && mapContent->zoom != Zoom::SGFX)
        {
            textRenderer->RenderText(
                std::to_string(gc.mapTile.mapX).append(", ").append(std::to_string(gc.mapTile.mapY)),
                screenPosition.x + static_cast<float>(get_tile_width(mapContent->zoom)) / 4.0f,
                screenPosition.y + static_cast<float>(get_tile_height(mapContent->zoom)) / 4.0f,
                0.25f,
                glm::vec3(1.0f, 0.0f, 0.0f),
                *context->window, *context->camera
            );
        }
    }
}

void mdcii::map::Map::RenderEntities() const
{
    // render nothing
    if (!renderTerrainLayer && !renderBuildingsLayer)
    {
        return;
    }

    // render terrain only
    if (renderTerrainLayer && !renderBuildingsLayer)
    {
        RenderTerrainLayerEntities();
        return;
    }

    // render buildings only
    if (!renderTerrainLayer && renderBuildingsLayer)
    {
        RenderBuildingsLayerEntities();
        return;
    }

    // render terrain or buildings
    RenderTerrainOrBuildingsEntities();
}

void mdcii::map::Map::RenderEntity(
    const MapTile& t_mapTile,
    const data::Building& t_building,
    const bool t_selected
) const
{
    auto buildingRotation{ t_mapTile.rotation };
    if (t_building.rotate > 0)
    {
        buildingRotation = buildingRotation + mapContent->rotation;
    }

    auto gfx{ t_mapTile.gfxs[rotation_to_int(buildingRotation)] };

    if (t_building.size.w > 1)
    {
        // default: orientation 0
        auto rp{ glm::ivec2(t_mapTile.x, t_mapTile.y) };

        if (t_mapTile.rotation == Rotation::DEG270)
        {
            rp = rotate_position(
                t_mapTile.x, t_mapTile.y,
                t_building.size.w, t_building.size.h,
                Rotation::DEG90
            );
        }

        if (t_mapTile.rotation == Rotation::DEG180)
        {
            rp = rotate_position(
                t_mapTile.x, t_mapTile.y,
                t_building.size.w, t_building.size.h,
                Rotation::DEG180
            );
        }

        if (t_mapTile.rotation == Rotation::DEG90)
        {
            rp = rotate_position(
                t_mapTile.x, t_mapTile.y,
                t_building.size.w, t_building.size.h,
                Rotation::DEG270
            );
        }

        const auto offset{ rp.y * t_building.size.w + rp.x };
        gfx += offset;
    }

    auto elevation{ 0.0f };
    if (t_building.posoffs > 0)
    {
        elevation = static_cast<float>(get_elevation(mapContent->zoom));
    }

    RenderBuilding(
        gfx,
        t_mapTile.screenPositions.at(magic_enum::enum_integer(mapContent->zoom)).at(rotation_to_int(mapContent->rotation)),
        elevation,
        t_selected
    );
}

void mdcii::map::Map::RenderTerrainLayerEntities() const
{
#ifdef MDCII_DEBUG_BUILD
    auto t{ 0 }; // terrain tiles expected result = width * height
#endif

    const auto view{ Game::ecs.view<const ecs::TerrainLayerTileComponent>() };
    for (const auto entity : view)
    {
        if (skipBuildingsLayerTiles)
        {
            if (Game::ecs.all_of<const ecs::BuildingsLayerTileComponent>(entity))
            {
#ifdef MDCII_DEBUG_BUILD
                t++;
#endif
                continue;
            }
        }

        const auto& [terrainLayerTileComponent]{ view.get(entity) };
        RenderEntity(terrainLayerTileComponent.mapTile, terrainLayerTileComponent.building, false);
#ifdef MDCII_DEBUG_BUILD
        t++;
#endif
    }

    MDCII_ASSERT(t == mapContent->width * mapContent->height, "[Map::RenderTerrainLayerEntities()] Invalid number of entities.")
}

void mdcii::map::Map::RenderBuildingsLayerEntities() const
{
    const auto view{ Game::ecs.view<ecs::BuildingsLayerTileComponent>(entt::exclude<ecs::BuildingUpdatedComponent>) };
    for (const auto entity : view)
    {
        const auto& [buildingsComponent]{ view.get(entity) };
        RenderEntity(buildingsComponent.mapTile, buildingsComponent.building, false);
    }
}

void mdcii::map::Map::RenderTerrainOrBuildingsEntities() const
{
    const auto view{ Game::ecs.view<ecs::TerrainLayerTileComponent>(/*entt::exclude<ecs::BuildingUpdatedComponent>*/) };
    for (const auto entity : view)
    {
        const auto& terrainLayerTileComponent{ view.get<ecs::TerrainLayerTileComponent>(entity) };

        if (Game::ecs.all_of<const ecs::BuildingsLayerTileComponent>(entity))
        {
            const auto& buildingsLayerTileComponent{ Game::ecs.get<const ecs::BuildingsLayerTileComponent>(entity) };

            RenderEntity(buildingsLayerTileComponent.mapTile, buildingsLayerTileComponent.building, false);
        }
        else
        {
            RenderEntity(terrainLayerTileComponent.mapTile, terrainLayerTileComponent.building, false);
        }
    }
}
