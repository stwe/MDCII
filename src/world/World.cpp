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
#include "Island.h"
#include "MousePicker.h"
#include "Layer.h"
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

    m_tileAtlas = std::make_unique<resource::TileAtlas>(this);
    m_animalsTileAtlas = std::make_unique<resource::AnimalsTileAtlas>(this);
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

    // render deep water && islands
    m_tileAtlas->CalcAnimationFrame(t_elapsedTime);
    m_tileAtlas->RenderDeepWater(m_renderDeepWaterGrid);
    m_tileAtlas->RenderIslands(m_renderIslandsGrid);

    resource::SpriteSheet spriteSheetN{ "BEEF_N", resource::Direction::NORTH, 0, 8 };
    resource::SpriteSheet spriteSheetNe{ "BEEF_NE", resource::Direction::NORTH_EAST, 8, 8 };
    resource::Animation animationN{ islands.at(0).get(), &spriteSheetN, 3, 5 };
    resource::Animation animationNe{ islands.at(0).get(), &spriteSheetNe, 2, 5 };

    m_animalsTileAtlas->CalcAnimationFrame(t_elapsedTime);
    m_animalsTileAtlas->Render(&animationN, olc::WHITE);
    m_animalsTileAtlas->Render(&animationNe, olc::WHITE);

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

                            if (island->IsWorldPositionOverIsland({ island->startX + posX, island->startY + posY }))
                            {
                                const auto& terrainTileToCheck{ island->GetLayer(LayerType::TERRAIN)->GetSortedTile(posX, posY, camera->rotation) };
                                const auto& buildingTileToCheck{ island->GetLayer(LayerType::BUILDINGS)->GetSortedTile(posX, posY, camera->rotation) };

                                if (terrainTileToCheck.HasBuildingAboveWaterAndCoast() && !buildingTileToCheck.HasBuilding())
                                {
                                    newTiles.emplace_back(
                                            Gui::select_building.building,
                                            magic_enum::enum_integer(Gui::select_building.rotation - camera->rotation),
                                            offset.x < 0 ? Gui::select_building.building->size.w - 1 - x : x,
                                            offset.y < 0 ? Gui::select_building.building->size.h - 1 - y : y,
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

                            island->GetLayer(LayerType::BUILDINGS)->AddBuilding(newTiles);
                            island->GetLayer(LayerType::MIXED)->AddBuilding(newTiles);

                            CheckLayer(island, LayerType::BUILDINGS);
                            CheckLayer(island, LayerType::MIXED);
                        }

                        // render the building (dark grey) to be added and a green grid
                        for (const auto& tile : newTiles)
                        {
                            m_tileAtlas->Render(island->startX, island->startY, &tile, olc::DARK_GREY);
                            renderer::Renderer::RenderAsset(resource::Asset::GREEN_ISO, island->startX, island->startY, this, &tile, true);
                        }

                        break; // break offsets loop
                    }
                    else
                    {
                        std::vector<Tile>().swap(newTiles);
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
    mousePicker->RenderImGui();

    static auto renderCoastLayer{ false };
    static auto renderTerrainLayer{ false };
    static auto renderBuildingsLayer{ false };
    static auto renderMixedLayer{ true };
    static auto renderDeepWaterLayer{ true };

    ImGui::Checkbox("Render islands terrain grid", &m_renderIslandsGrid);
    ImGui::Checkbox("Render deep water grid", &m_renderDeepWaterGrid);

    ImGui::Separator();

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
