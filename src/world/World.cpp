// This file is part of the MDCII project.
//
// Copyright (c) 2023. stwe <https://github.com/stwe/MDCII>
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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#include "World.h"
#include "Game.h"
#include "GameState.h"
#include "Island.h"
#include "MousePicker.h"
#include "Layer.h"
#include "DeepWater.h"
#include "MdciiAssert.h"
#include "Gui.h"
#include "resource/MdciiFile.h"
#include "resource/OriginalResourcesManager.h"
#include "resource/AssetManager.h"
#include "renderer/Renderer.h"
#include "camera/Camera.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::World::World(GameState* t_gameState, const std::string& t_fileName)
    : gameState{ t_gameState }
{
    MDCII_LOG_DEBUG("[World::World()] Create World.");

    MDCII_ASSERT(gameState, "[World::World()] Null pointer.")

    Init(t_fileName);
}

mdcii::world::World::~World() noexcept
{
    MDCII_LOG_DEBUG("[World::~World()] Destruct World.");
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::World::OnUserUpdate(const float t_elapsedTime)
{
    // first access
    if (m_flag)
    {
        MDCII_LOG_DEBUG("[World::OnUserUpdate()] First world update.");

        MDCII_ASSERT(deepWater->layer->currentTiles.empty(), "[World::OnUserUpdate()] Invalid number of tiles.")

        FindVisibleIslands();
        FindVisibleDeepWaterTiles();

        m_flag = false;
    }

    // on key pressed
    if (camera->OnUserUpdate(t_elapsedTime) && !m_flag)
    {
        FindVisibleIslands();
        FindVisibleDeepWaterTiles();
    }

    // render
    renderer::Renderer::CalcAnimationFrame(t_elapsedTime);
    renderer->RenderDeepWater();
    renderer->RenderIslands();

    // ---------------------------------
    // todo: Rendering depends on ImGui
    // ---------------------------------

    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    // render the selected building to add it
    if (Gui::select_building.building)
    {
        const std::array offsets{
            olc::vi2d(+1, +1), // + x, + y
            olc::vi2d(-1, -1), // - x, - y
            olc::vi2d(+1, -1), // + x, - y
            olc::vi2d(-1, +1), // - x, + y
        };

        for (const auto* island : currentIslands)
        {
            if (auto mouseOverIsland{ island->IsMouseOverIsland() }; mouseOverIsland.has_value())
            {
                const auto* terrainLayer{ island->GetLayer(LayerType::TERRAIN) };
                const auto* buildingsLayer{ island->GetLayer(LayerType::BUILDINGS) };

                if (terrainLayer->GetTile(mouseOverIsland.value().x, mouseOverIsland.value().y, camera->rotation).HasBuildingAboveWaterAndCoast())
                {
                    std::vector<Tile> newTiles;

                    for (const auto& offset : offsets)
                    {
                        for (auto y{ 0 }; y < Gui::select_building.building->size.h; ++y)
                        {
                            for (auto x{ 0 }; x < Gui::select_building.building->size.w; ++x)
                            {
                                const auto offsetX{ x * offset.x };
                                const auto offsetY{ y * offset.y };

                                const auto posX{ mouseOverIsland.value().x + offsetX };
                                const auto posY{ mouseOverIsland.value().y + offsetY };

                                const auto& terrainTileToCheck{ terrainLayer->GetTile(posX, posY, camera->rotation) };
                                const auto& buildingTileToCheck{ buildingsLayer->GetTile(posX, posY, camera->rotation) };

                                if (terrainTileToCheck.HasBuildingAboveWaterAndCoast() && !buildingTileToCheck.HasBuilding())
                                {
                                    newTiles.emplace_back(
                                        Gui::select_building.building,
                                        magic_enum::enum_integer(Gui::select_building.rotation),
                                        offset.x < 0 ? Gui::select_building.building->size.w - 1 - x : x,
                                        offset.y < 0 ? Gui::select_building.building->size.h - 1 - y : y,
                                        posX, posY
                                    );
                                }
                            }
                        }

                        if (newTiles.size() == Gui::select_building.building->size.h * Gui::select_building.building->size.w)
                        {
                            for (const auto& tile : newTiles)
                            {
                                renderer->RenderBuilding(island->startX, island->startY, &tile, olc::DARK_GREY);
                                //renderer->RenderAsset(resource::Asset::GREEN_ISO, island->startX, island->startY, &tile);
                            }

                            break;
                        }
                        else
                        {
                            std::vector<Tile>().swap(newTiles);
                        }
                    }
                }
            }
        }

        if (gameState->game->GetMouse(1).bHeld)
        {
            Gui::select_building.building = nullptr;
            Gui::select_building.rotation = Rotation::DEG0;
        }

        if (gameState->game->GetMouse(0).bPressed)
        {
            MDCII_LOG_DEBUG("[World::OnUserUpdate()] Add {}", Gui::select_building.building->id);

            // todo
        }
    }
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::world::World::RenderImGui()
{
    static auto renderCoastLayer{ false };
    static auto renderTerrainLayer{ false };
    static auto renderBuildingsLayer{ false };
    static auto renderMixedLayer{ true };
    static auto renderDeepWaterLayer{ true };

    if (ImGui::Checkbox("Render Coast Layer", &renderCoastLayer))
    {
        ToggleRenderLayer(RENDER_COAST_LAYER);
        DisableRenderLayer(RENDER_MIXED_LAYER, renderMixedLayer);

        FindVisibleIslands();
    }
    if (ImGui::Checkbox("Render Terrain Layer", &renderTerrainLayer))
    {
        ToggleRenderLayer(RENDER_TERRAIN_LAYER);
        DisableRenderLayer(RENDER_MIXED_LAYER, renderMixedLayer);

        FindVisibleIslands();
    }
    if (ImGui::Checkbox("Render Buildings Layer", &renderBuildingsLayer))
    {
        ToggleRenderLayer(RENDER_BUILDINGS_LAYER);
        DisableRenderLayer(RENDER_MIXED_LAYER, renderMixedLayer);

        FindVisibleIslands();
    }
    if (ImGui::Checkbox("Render Mixed Layer", &renderMixedLayer))
    {
        ToggleRenderLayer(RENDER_MIXED_LAYER);

        DisableRenderLayer(RENDER_COAST_LAYER, renderCoastLayer);
        DisableRenderLayer(RENDER_TERRAIN_LAYER, renderTerrainLayer);
        DisableRenderLayer(RENDER_BUILDINGS_LAYER, renderBuildingsLayer);

        FindVisibleIslands();
    }
    if (ImGui::Checkbox("Render Deep-Water Layer", &renderDeepWaterLayer))
    {
        ToggleRenderLayer(RENDER_DEEP_WATER_LAYER);
        FindVisibleDeepWaterTiles();
    }

    ImGui::Separator();

    Gui::RenderAddBuildingsGui(gameState->game);
}

//-------------------------------------------------
// World / screen positions
//-------------------------------------------------

olc::vi2d mdcii::world::World::ToScreen(const int t_x, const int t_y) const
{
    const auto position{ rotate_position(t_x, t_y, worldWidth, worldHeight, camera->rotation) };

    return olc::vi2d{
        camera->screenPosition.x + (position.x - position.y) * get_tile_width_half(camera->zoom),
        camera->screenPosition.y + (position.x + position.y) * get_tile_height_half(camera->zoom)
    };
}

bool mdcii::world::World::IsWorldPositionOnAnyIsland(const int t_x, const int t_y) const
{
    return std::ranges::any_of(islands, [t_x, t_y](auto const& t_island)
    {
        return t_island->IsWorldPositionOverIsland({ t_x, t_y });
    });
}

bool mdcii::world::World::IsWorldPositionOutsideScreen(const int t_x, const int t_y) const
{
    const auto screenPosition{ ToScreen(t_x, t_y) };
    return screenPosition.x > Game::INI.Get<int>("window", "width") + get_tile_width(camera->zoom) ||
           screenPosition.x < -get_tile_width_half(camera->zoom) ||
           screenPosition.y > Game::INI.Get<int>("window", "height") + get_tile_height(camera->zoom) ||
           screenPosition.y < -get_tile_height_half(camera->zoom);
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::World::Init(const std::string& t_fileName)
{
    MDCII_LOG_DEBUG("[World::Init()] Start initializing the world ...");

    if (resource::MdciiFile mdciiFile{ this, t_fileName }; mdciiFile.LoadJsonFromFile())
    {
        islands = mdciiFile.CreateWorldContent();
    }

    deepWater = std::make_unique<DeepWater>(this);
    renderer = std::make_unique<renderer::Renderer>(this);
    camera = std::make_unique<camera::Camera>(this);

    MDCII_LOG_DEBUG("[World::Init()] The world were initialized successfully.");
}

//-------------------------------------------------
// Render helper
//-------------------------------------------------

void mdcii::world::World::FindVisibleIslands()
{
    std::vector<Island*>().swap(currentIslands);

    for (const auto& island : islands)
    {
        if (HasRenderLayerOption(RenderLayer::RENDER_MIXED_LAYER))
        {
            CheckLayer(island.get(), LayerType::MIXED);

            if (!island->layers[magic_enum::enum_integer(LayerType::MIXED)]->currentTiles.empty())
            {
                currentIslands.push_back(island.get());
            }
        }
        else if (HasRenderLayerOption(RenderLayer::RENDER_COAST_LAYER) ||
                 HasRenderLayerOption(RenderLayer::RENDER_TERRAIN_LAYER) ||
                 HasRenderLayerOption(RenderLayer::RENDER_BUILDINGS_LAYER))
        {
            auto dirty{ false };
            if (HasRenderLayerOption(RenderLayer::RENDER_COAST_LAYER))
            {
                CheckLayer(island.get(), LayerType::COAST);
                if (!island->layers[magic_enum::enum_integer(LayerType::COAST)]->currentTiles.empty())
                {
                    dirty = true;
                }
            }
            if (HasRenderLayerOption(RenderLayer::RENDER_TERRAIN_LAYER))
            {
                CheckLayer(island.get(), LayerType::TERRAIN);
                if (!island->layers[magic_enum::enum_integer(LayerType::TERRAIN)]->currentTiles.empty())
                {
                    dirty = true;
                }
            }
            if (HasRenderLayerOption(RenderLayer::RENDER_BUILDINGS_LAYER))
            {
                CheckLayer(island.get(), LayerType::BUILDINGS);
                if (!island->layers[magic_enum::enum_integer(LayerType::BUILDINGS)]->currentTiles.empty())
                {
                    dirty = true;
                }
            }

            if (dirty)
            {
                currentIslands.push_back(island.get());
            }
        }
    }

    MDCII_LOG_DEBUG("[World::FindVisibleIslands()] Render {} islands.", currentIslands.size());
}

void mdcii::world::World::CheckLayer(const Island* t_island, const LayerType t_layerType) const
{
    const auto layerTypeInt{ magic_enum::enum_integer(t_layerType) };

    std::vector<Tile>().swap(t_island->layers[layerTypeInt]->currentTiles);
    t_island->layers[layerTypeInt]->currentTiles = t_island->layers[layerTypeInt]->sortedTiles.at(magic_enum::enum_integer(camera->rotation));

    auto [begin, end]{ std::ranges::remove_if(t_island->layers[layerTypeInt]->currentTiles, [&](const Tile& t_tile)
    {
        return IsWorldPositionOutsideScreen(t_tile.posX + t_island->startX, t_tile.posY + t_island->startY);
    }) };

    t_island->layers[layerTypeInt]->currentTiles.erase(begin, end);
}

void mdcii::world::World::FindVisibleDeepWaterTiles() const
{
    if (!HasRenderLayerOption(RenderLayer::RENDER_DEEP_WATER_LAYER))
    {
        std::vector<Tile>().swap(deepWater->layer->currentTiles);
        MDCII_LOG_DEBUG("[World::FindVisibleDeepWaterTiles()] Render {} deep water tiles.", deepWater->layer->currentTiles.size());

        return;
    }

    const auto shouldBeRemoved = [&](const Tile& t_tile)
    {
        return IsWorldPositionOutsideScreen(t_tile.posX, t_tile.posY);
    };

    std::vector<Tile>().swap(deepWater->layer->currentTiles);
    deepWater->layer->currentTiles = deepWater->layer->sortedTiles.at(magic_enum::enum_integer(camera->rotation));

    auto [begin, end]{ std::ranges::remove_if(deepWater->layer->currentTiles, shouldBeRemoved) };
    deepWater->layer->currentTiles.erase(begin, end);

    MDCII_LOG_DEBUG("[World::FindVisibleDeepWaterTiles()] Render {} deep water tiles.", deepWater->layer->currentTiles.size());
}

//-------------------------------------------------
//  Render layer bits
//-------------------------------------------------

void mdcii::world::World::ToggleRenderLayer(const RenderLayer t_renderLayer)
{
    renderLayer ^= t_renderLayer;
}

void mdcii::world::World::DisableRenderLayer(const RenderLayer t_renderLayer, bool& t_toFalse)
{
    renderLayer &= ~t_renderLayer;
    t_toFalse = false;
}
