// This file is part of the MDCII project.
//
// Copyright (c) 2024. stwe <https://github.com/stwe/MDCII>
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
#include "Island.h"
#include "MousePicker.h"
#include "DeepWater.h"
#include "MdciiAssert.h"
#include "Gui.h"
#include "state/State.h"
#include "resource/MdciiFile.h"
#include "resource/OriginalResourcesManager.h"
#include "resource/TileAtlas.h"
#include "resource/AnimalsTileAtlas.h"
#include "resource/AssetManager.h"
#include "renderer/Renderer.h"
#include "camera/Camera.h"
#include "world/layer/TerrainLayer.h"
#include "world/layer/FiguresLayer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::World::World(state::State* t_state, const int t_width, const int t_height)
    : state{ t_state }
    , worldWidth{ t_width }
    , worldHeight{ t_height }
{
    MDCII_LOG_DEBUG("[World::World()] Create World.");

    MDCII_ASSERT(state, "[World::World()] Null pointer.")

    MDCII_LOG_DEBUG("[World::World()] The size of the world is set to ({}, {}).", worldWidth, worldHeight);

    CreateCoreObjects();
}

mdcii::world::World::World(state::State* t_state, const nlohmann::json& t_json)
    : state{ t_state }
{
    MDCII_LOG_DEBUG("[World::World()] Create World.");

    MDCII_ASSERT(state, "[World::World()] Null pointer.")

    Init(t_json);
}

mdcii::world::World::~World() noexcept
{
    MDCII_LOG_DEBUG("[World::~World()] Destruct World.");
}

//-------------------------------------------------
// Create core objects
//-------------------------------------------------

void mdcii::world::World::CreateCoreObjects()
{
    MDCII_LOG_DEBUG("[World::CreateCoreObjects()] Create core objects.");

    if (worldWidth <= 0 || worldHeight <= 0)
    {
        throw MDCII_EXCEPTION("[World::CreateCoreObjects()] Invalid world size.");
    }

    deepWater = std::make_unique<DeepWater>(this);
    camera = std::make_unique<camera::Camera>(this);
    mousePicker = std::make_unique<MousePicker>(this, true);

    tileAtlas = std::make_unique<resource::TileAtlas>(this);
    animalsTileAtlas = std::make_unique<resource::AnimalsTileAtlas>(this);
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

    // render world
    HasRenderLayerOption(RenderLayer::RENDER_ALL) ?
        renderer::Renderer::RenderWorld(this, t_elapsedTime) :     // should be used in the release
        renderer::Renderer::RenderWorldParts(this, t_elapsedTime); // ability to switch layers on and off

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

        for (auto* island : currentIslands)
        {
            if (auto mouseOverIsland{ island->IsMouseOverIsland() }; mouseOverIsland.has_value())
            {
                std::vector<tile::TerrainTile> newTiles;

                for (const auto& offset : offsets)
                {
                    for (auto y{ 0 }; y < Gui::select_building.building->size.h; ++y)
                    {
                        for (auto x{ 0 }; x < Gui::select_building.building->size.w; ++x)
                        {
                            olc::vi2d rp{ x, y };
                            auto ro = offset;

                            if (Gui::select_building.building->size.w != Gui::select_building.building->size.h)
                            {
                                if (camera->rotation == Rotation::DEG0 || camera->rotation == Rotation::DEG180)
                                {
                                    if (Gui::select_building.rotation == world::Rotation::DEG90 || Gui::select_building.rotation == world::Rotation::DEG270)
                                    {
                                        rp = world::rotate_position(x, y,
                                                                    Gui::select_building.building->size.h,
                                                                    Gui::select_building.building->size.w,
                                                                    Rotation::DEG90);
                                        ro = world::rotate_position(offset.x, offset.y,
                                                                    Gui::select_building.building->size.h,
                                                                    Gui::select_building.building->size.w,
                                                                    Rotation::DEG90);
                                    }
                                }

                                if (camera->rotation == Rotation::DEG90 || camera->rotation == Rotation::DEG270)
                                {
                                    if (Gui::select_building.rotation == world::Rotation::DEG0 || Gui::select_building.rotation == world::Rotation::DEG180)
                                    {
                                        rp = world::rotate_position(x, y,
                                                                    Gui::select_building.building->size.h,
                                                                    Gui::select_building.building->size.w,
                                                                    Rotation::DEG90);
                                        ro = world::rotate_position(offset.x, offset.y,
                                                                    Gui::select_building.building->size.h,
                                                                    Gui::select_building.building->size.w,
                                                                    Rotation::DEG90);
                                    }
                                }
                            }

                            const auto offsetX{ rp.x * ro.x };
                            const auto offsetY{ rp.y * ro.y };

                            const auto posX{ mouseOverIsland.value().x + offsetX };
                            const auto posY{ mouseOverIsland.value().y + offsetY };

                            if (island->IsWorldPositionOverIsland({ island->startX + posX, island->startY + posY }))
                            {
                                const auto& terrainTileToCheck{ island->GetTerrainLayer(layer::LayerType::TERRAIN)->GetSortedTile(posX, posY, camera->rotation) };
                                const auto& buildingTileToCheck{ island->GetTerrainLayer(layer::LayerType::BUILDINGS)->GetSortedTile(posX, posY, camera->rotation) };

                                if (terrainTileToCheck.HasBuildingAboveWaterAndCoast() && !buildingTileToCheck.HasBuilding())
                                {
                                    newTiles.emplace_back(
                                        Gui::select_building.building,
                                        magic_enum::enum_integer(Gui::select_building.rotation - camera->rotation),
                                        ro.x < 0 ? Gui::select_building.building->size.h - 1 - rp.x : rp.x,
                                        ro.y < 0 ? Gui::select_building.building->size.w - 1 - rp.y : rp.y,
                                        posX, posY
                                    );
                                }
                            }
                        }
                    }

                    if (newTiles.size() == Gui::select_building.building->size.h * static_cast<std::size_t>(Gui::select_building.building->size.w))
                    {
                        // add on left mouse button click
                        if (state->game->GetMouse(0).bPressed)
                        {
                            MDCII_LOG_DEBUG("[World::OnUserUpdate()] Add {}", Gui::select_building.building->id);

                            island->GetTerrainLayer(layer::LayerType::BUILDINGS)->AddBuilding(newTiles);
                            island->GetTerrainLayer(layer::LayerType::MIXED)->AddBuilding(newTiles);

                            CheckLayer(island, layer::LayerType::BUILDINGS);
                            CheckLayer(island, layer::LayerType::MIXED);
                        }

                        // render the building (dark grey) to be added and a green grid
                        for (const auto& tile : newTiles)
                        {
                            tileAtlas->RenderTile(island->startX, island->startY, &tile, olc::DARK_GREY);
                            renderer::Renderer::RenderAsset(resource::Asset::GREEN_ISO, island->startX, island->startY, this, &tile, true);
                        }

                        break; // break offsets loop
                    }
                    else
                    {
                        std::vector<tile::TerrainTile>().swap(newTiles);
                    }
                }
            }
        }

        if (state->game->GetMouse(1).bHeld)
        {
            Gui::select_building.building = nullptr;
            Gui::select_building.rotation = Rotation::DEG0;
        }
    }

    mousePicker->OnUserUpdate();
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::world::World::RenderImGui()
{
    Gui::SaveGameGui(this);

    ImGui::Separator();

    ImGui::Text("Camera rotation %s", magic_enum::enum_name(camera->rotation).data());

    mousePicker->RenderImGui();

    static auto renderCoastLayer{ false };
    static auto renderTerrainLayer{ false };
    static auto renderBuildingsLayer{ false };
    static auto renderMixedLayer{ false };
    static auto renderDeepWaterLayer{ false };
    static auto renderFiguresLayer{ false };

    static auto renderAll{ true };

    ImGui::Checkbox("Render islands terrain grid", &renderIslandsGrid);
    ImGui::Checkbox("Render deep water grid", &renderDeepWaterGrid);

    ImGui::Separator();

    if (ImGui::Checkbox("Render Coast Layer", &renderCoastLayer))
    {
        ToggleRenderLayer(RENDER_COAST_LAYER);

        DisableRenderLayer(RENDER_MIXED_LAYER, renderMixedLayer);
        DisableRenderLayer(RENDER_ALL, renderAll);

        FindVisibleIslands();
    }
    if (ImGui::Checkbox("Render Terrain Layer", &renderTerrainLayer))
    {
        ToggleRenderLayer(RENDER_TERRAIN_LAYER);

        DisableRenderLayer(RENDER_MIXED_LAYER, renderMixedLayer);
        DisableRenderLayer(RENDER_ALL, renderAll);

        FindVisibleIslands();
    }
    if (ImGui::Checkbox("Render Buildings Layer", &renderBuildingsLayer))
    {
        ToggleRenderLayer(RENDER_BUILDINGS_LAYER);

        DisableRenderLayer(RENDER_MIXED_LAYER, renderMixedLayer);
        DisableRenderLayer(RENDER_ALL, renderAll);

        FindVisibleIslands();
    }
    if (ImGui::Checkbox("Render Figures Layer", &renderFiguresLayer))
    {
        ToggleRenderLayer(RENDER_FIGURES_LAYER);

        DisableRenderLayer(RENDER_ALL, renderAll);

        FindVisibleIslands();
    }
    if (ImGui::Checkbox("Render Mixed Layer", &renderMixedLayer))
    {
        ToggleRenderLayer(RENDER_MIXED_LAYER);

        DisableRenderLayer(RENDER_COAST_LAYER, renderCoastLayer);
        DisableRenderLayer(RENDER_TERRAIN_LAYER, renderTerrainLayer);
        DisableRenderLayer(RENDER_BUILDINGS_LAYER, renderBuildingsLayer);
        DisableRenderLayer(RENDER_ALL, renderAll);

        FindVisibleIslands();
    }
    if (ImGui::Checkbox("Render Deep-Water Layer", &renderDeepWaterLayer))
    {
        ToggleRenderLayer(RENDER_DEEP_WATER_LAYER);

        DisableRenderLayer(RENDER_ALL, renderAll);

        FindVisibleDeepWaterTiles();
    }
    if (ImGui::Checkbox("Render All", &renderAll))
    {
        ToggleRenderLayer(RENDER_ALL);

        DisableRenderLayer(RENDER_COAST_LAYER, renderCoastLayer);
        DisableRenderLayer(RENDER_TERRAIN_LAYER, renderTerrainLayer);
        DisableRenderLayer(RENDER_BUILDINGS_LAYER, renderBuildingsLayer);
        DisableRenderLayer(RENDER_FIGURES_LAYER, renderFiguresLayer);
        DisableRenderLayer(RENDER_MIXED_LAYER, renderMixedLayer);

        FindVisibleIslands();
        FindVisibleDeepWaterTiles();
    }

    ImGui::Separator();

    Gui::RenderAddBuildingsGui(state->game);
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

void mdcii::world::World::Init(const nlohmann::json& t_json)
{
    MDCII_LOG_DEBUG("[World::Init()] Start initializing the world ...");

    worldWidth = t_json.at("world").at("width").get<int>();
    worldHeight = t_json.at("world").at("height").get<int>();

    if (worldWidth <= 0 || worldHeight <= 0)
    {
        throw MDCII_EXCEPTION("[World::Init()] Invalid world size.");
    }

    MDCII_LOG_DEBUG("[World::Init()] The size of the world is set to ({}, {}).", worldWidth, worldHeight);

    std::vector<std::unique_ptr<Island>> newIslands;
    for (const auto& [islandKeys, islandVars] : t_json["islands"].items())
    {
        newIslands.emplace_back(std::make_unique<Island>(this, islandVars));
    }

    islands = std::move(newIslands);

    CreateCoreObjects();

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
        if (HasRenderLayerOption(RenderLayer::RENDER_FIGURES_LAYER))
        {
            CheckLayer(island.get(), layer::LayerType::FIGURES);
            if (!island->GetFiguresLayer()->currentTiles.empty())
            {
                currentIslands.push_back(island.get());
            }
        }

        if (HasRenderLayerOption(RenderLayer::RENDER_MIXED_LAYER))
        {
            CheckLayer(island.get(), layer::LayerType::MIXED);
            if (!island->GetTerrainLayer(layer::LayerType::MIXED)->currentTiles.empty())
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
                CheckLayer(island.get(), layer::LayerType::COAST);
                if (!island->GetTerrainLayer(layer::LayerType::COAST)->currentTiles.empty())
                {
                    dirty = true;
                }
            }
            if (HasRenderLayerOption(RenderLayer::RENDER_TERRAIN_LAYER))
            {
                CheckLayer(island.get(), layer::LayerType::TERRAIN);
                if (!island->GetTerrainLayer(layer::LayerType::TERRAIN)->currentTiles.empty())
                {
                    dirty = true;
                }
            }
            if (HasRenderLayerOption(RenderLayer::RENDER_BUILDINGS_LAYER))
            {
                CheckLayer(island.get(), layer::LayerType::BUILDINGS);
                if (!island->GetTerrainLayer(layer::LayerType::BUILDINGS)->currentTiles.empty())
                {
                    dirty = true;
                }
            }

            if (dirty)
            {
                currentIslands.push_back(island.get());
            }
        }

        if (HasRenderLayerOption(RenderLayer::RENDER_ALL))
        {
            CheckLayer(island.get(), layer::LayerType::FIGURES);
            CheckLayer(island.get(), layer::LayerType::MIXED);
            if (!island->GetTerrainLayer(layer::LayerType::MIXED)->currentTiles.empty())
            {
                currentIslands.push_back(island.get());
            }
        }
    }

    MDCII_LOG_DEBUG("[World::FindVisibleIslands()] Render {} islands.", currentIslands.size());
}

void mdcii::world::World::CheckLayer(Island* t_island, const layer::LayerType t_layerType) const
{
    if (t_layerType == layer::LayerType::FIGURES)
    {
        std::vector<tile::FigureTile>().swap(t_island->GetFiguresLayer()->currentTiles);
        t_island->GetFiguresLayer()->currentTiles = t_island->GetFiguresLayer()->sortedTiles.at(magic_enum::enum_integer(camera->rotation));

        auto [begin, end]{ std::ranges::remove_if(t_island->GetFiguresLayer()->currentTiles,
                                                  [&](const tile::FigureTile& t_tile) {
                                                      return IsWorldPositionOutsideScreen(t_tile.posX + t_island->startX, t_tile.posY + t_island->startY);
                                                  } )};

        t_island->GetFiguresLayer()->currentTiles.erase(begin, end);
    }
    else
    {
        std::vector<tile::TerrainTile>().swap(t_island->GetTerrainLayer(t_layerType)->currentTiles);
        t_island->GetTerrainLayer(t_layerType)->currentTiles = t_island->GetTerrainLayer(t_layerType)->sortedTiles.at(magic_enum::enum_integer(camera->rotation));

        auto [begin, end]{ std::ranges::remove_if(t_island->GetTerrainLayer(t_layerType)->currentTiles,
                                                 [&](const tile::TerrainTile& t_tile) {
                                                     return IsWorldPositionOutsideScreen(t_tile.posX + t_island->startX, t_tile.posY + t_island->startY);
                                                 } )};

        t_island->GetTerrainLayer(t_layerType)->currentTiles.erase(begin, end);
    }
}

void mdcii::world::World::FindVisibleDeepWaterTiles() const
{
    if (!HasRenderLayerOption(RenderLayer::RENDER_DEEP_WATER_LAYER) && !HasRenderLayerOption(RenderLayer::RENDER_ALL))
    {
        std::vector<tile::TerrainTile>().swap(deepWater->layer->currentTiles);
        MDCII_LOG_DEBUG("[World::FindVisibleDeepWaterTiles()] Render {} deep water tiles.", deepWater->layer->currentTiles.size());

        return;
    }

    const auto shouldBeRemoved = [&](const tile::TerrainTile& t_tile)
    {
        return IsWorldPositionOutsideScreen(t_tile.posX, t_tile.posY);
    };

    std::vector<tile::TerrainTile>().swap(deepWater->layer->currentTiles);
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

//-------------------------------------------------
// Serializing World into Json
//-------------------------------------------------

void mdcii::world::to_json(nlohmann::json& t_json, const World& t_world)
{
    // version
    t_json["version"] = Game::VERSION;

    // world size
    t_json["world"] = { { "width", t_world.worldWidth }, { "height", t_world.worldHeight } };

    // islands
    auto islandsJson = nlohmann::json::array();
    for (const auto& island : t_world.islands)
    {
        auto islandJson = nlohmann::json::object();
        islandJson = *island;
        islandsJson.push_back(islandJson);
    }

    t_json["islands"] = islandsJson;
}
