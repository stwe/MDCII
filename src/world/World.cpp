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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#include <imgui.h>
#include "World.h"
#include "Game.h"
#include "MdciiAssert.h"
#include "MousePicker.h"
#include "event/EventManager.h"
#include "eventpp/utilities/argumentadapter.h"
#include "state/State.h"
#include "map/TileAtlas.h"
#include "renderer/WorldRenderer.h"
#include "file/OriginalResourcesManager.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::World::World(std::string t_mapFilePath, std::shared_ptr<state::Context> t_context)
    : context{ std::move(t_context) }
    , m_mapFilePath{ std::move(t_mapFilePath) }
{
    Log::MDCII_LOG_DEBUG("[World::World()] Create World.");

    Init();
    AddListeners();
}

mdcii::world::World::~World() noexcept
{
    Log::MDCII_LOG_DEBUG("[World::~World()] Destruct World.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const mdcii::world::WorldLayer& mdcii::world::World::GetLayer(const WorldLayerType t_layerType) const
{
    const auto& layer{ *layers.at(magic_enum::enum_integer(t_layerType)) };
    MDCII_ASSERT(layer.layerType == t_layerType, "[World::GetLayer()] Invalid layer type.")

    return layer;
}

mdcii::world::WorldLayer& mdcii::world::World::GetLayer(const WorldLayerType t_layerType)
{
    auto& layer{ *layers.at(magic_enum::enum_integer(t_layerType)) };
    MDCII_ASSERT(layer.layerType == t_layerType, "[World::GetLayer()] Invalid layer type.")

    return layer;
}

//-------------------------------------------------
// Render
//-------------------------------------------------

void mdcii::world::World::Render() const
{
    // terrain, buildings
    worldRenderer->Render(m_renderLayerType, zoom, rotation, *context->window, *context->camera);

    // grid
    if (m_renderGridLayer)
    {
        worldRenderer->Render(zoom, rotation, *context->window, *context->camera);
    }

    // mouse
    mousePicker->Render(*context->window, *context->camera);
}

void mdcii::world::World::RenderImGui()
{
    ImGui::Begin("World");

    // select layer to render
    static int e{ 0 };
    ImGui::RadioButton("Terrain", &e, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Buildings", &e, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Terrain && Buildings", &e, 2);

    auto layer{ magic_enum::enum_cast<WorldLayerType>(e) };
    if (layer.has_value())
    {
        const auto l{ layer.value() };
        m_renderLayerType = l;
    }

    // toggle grid
    ImGui::Checkbox("Grid", &m_renderGridLayer);

    // rotate world
    if (ImGui::Button("Rotate right"))
    {
        Rotate(map::ChangeRotation::RIGHT);
    }

    ImGui::SameLine();

    if (ImGui::Button("Rotate left"))
    {
        Rotate(map::ChangeRotation::LEFT);
    }

    // zoom in/out world
    if (ImGui::Button("+"))
    {
        Zoom(map::ChangeZoom::ZOOM_IN);
    }

    ImGui::SameLine();

    if (ImGui::Button("-"))
    {
        Zoom(map::ChangeZoom::ZOOM_OUT);
    }

    // actions
    ShowActionButtons();

    // selected tile
    if (m_currentTileIndex >= 0)
    {
        const auto& both{ GetLayer(WorldLayerType::TERRAIN_AND_BUILDINGS).tiles.at(m_currentTileIndex) };
        if (both.HasBuilding())
        {
            // todo: Im Mixed Layer sind die Cpu Daten noch unverändert.

            ImGui::Separator();

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(255, 0, 0)));
            ImGui::Text("Terrain & Buildings Layer");
            ImGui::PopStyleColor();

            both.RenderImGui();
        }
    }

    ImGui::End();

    // MousePicker Gui
    mousePicker->RenderImGui();
}

//-------------------------------------------------
// Rotate && Zoom
//-------------------------------------------------

void mdcii::world::World::Rotate(const map::ChangeRotation t_changeRotation)
{
    switch (t_changeRotation)
    {
    case map::ChangeRotation::LEFT:
        --rotation;
        break;
    case map::ChangeRotation::RIGHT:
        ++rotation;
        break;
    }
}

void mdcii::world::World::Zoom(const map::ChangeZoom t_changeZoom)
{
    switch (t_changeZoom)
    {
    case map::ChangeZoom::ZOOM_IN:
        ++zoom;
        context->camera->zoom = zoom;
        break;
    case map::ChangeZoom::ZOOM_OUT:
        --zoom;
        context->camera->zoom = zoom;
        break;
    }
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

bool mdcii::world::World::IsPositionInWorld(const int t_x, const int t_y) const
{
    if (t_x >= 0 && t_x < width &&
        t_y >= 0 && t_y < height)
    {
        return true;
    }

    return false;
}

int mdcii::world::World::GetMapIndex(const int t_x, const int t_y) const
{
    MDCII_ASSERT(IsPositionInWorld(t_x, t_y), "[World::GetMapIndex()] Invalid world position given.")

    return t_y * width + t_x;
}

int mdcii::world::World::GetMapIndex(const int t_x, const int t_y, const map::Rotation t_rotation) const
{
    MDCII_ASSERT(IsPositionInWorld(t_x, t_y), "[World::GetMapIndex()] Invalid world position given.")

    const auto position{ RotatePosition(t_x, t_y, t_rotation) };

    return position.y * width + position.x;
}

glm::vec2 mdcii::world::World::WorldToScreen(const int t_x, const int t_y, const map::Zoom t_zoom, const map::Rotation t_rotation) const
{
    MDCII_ASSERT(IsPositionInWorld(t_x, t_y), "[World::WorldToScreen()] Invalid world position given.")

    const auto position{ RotatePosition(t_x, t_y, t_rotation) };

    return {
        (position.x - position.y) * get_tile_width_half(t_zoom),
        (position.x + position.y) * get_tile_height_half(t_zoom)
    };
}

glm::ivec2 mdcii::world::World::RotatePosition(const int t_x, const int t_y, const map::Rotation t_rotation) const
{
    return rotate_position(t_x, t_y, width, height, t_rotation);
}

//-------------------------------------------------
// Event handler
//-------------------------------------------------

void mdcii::world::World::OnLeftMouseButtonPressed()
{
    const auto& mousePosition{ mousePicker->currentPosition };
    if (IsPositionInWorld(mousePosition.x, mousePosition.y) && m_currentAction == Action::STATUS)
    {
        m_currentTileIndex = GetMapIndex(mousePosition.x, mousePosition.y);
    }
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::World::Init()
{
    Log::MDCII_LOG_DEBUG("[World::Init()] Start initializing the world...");

    CreateTerrainAndBuildingsLayers(); // create Terrain && Buildings Layer
    PrepareRendering();                // add some pre-calculations to the tiles
    MergeTerrainAndBuildingsLayers();  // merge Terrain && Buildings Layer into a new Layer
    CreateGridLayer();                 // create Grid Layer

    tileAtlas = std::make_unique<map::TileAtlas>();
    worldRenderer = std::make_unique<renderer::WorldRenderer>(this);
    mousePicker = std::make_unique<MousePicker>(this, *context->window, *context->camera);

    Log::MDCII_LOG_DEBUG("[World::Init()] The world was successfully initialized.");
}

void mdcii::world::World::AddListeners()
{
    Log::MDCII_LOG_DEBUG("[World::AddListeners()] Add listeners.");

    // OnLeftMouseButtonPressed
    event::EventManager::event_dispatcher.appendListener(
        event::MdciiEventType::MOUSE_BUTTON_PRESSED,
        eventpp::argumentAdapter<void(const event::MouseButtonPressedEvent&)>(
            [&](const event::MouseButtonPressedEvent& t_event) {
                OnLeftMouseButtonPressed();
            }
        )
    );
}

void mdcii::world::World::CreateTerrainAndBuildingsLayers()
{
    MDCII_ASSERT(layers.empty(), "[World::CreateTerrainAndBuildingsLayers()] Invalid number of Layers.")

    // read file
    Log::MDCII_LOG_DEBUG("[World::CreateTerrainAndBuildingsLayers()] Starts creating Json value from file {}...", m_mapFilePath);
    nlohmann::json j = read_json_from_file(Game::RESOURCES_REL_PATH + m_mapFilePath);
    Log::MDCII_LOG_DEBUG("[World::CreateTerrainAndBuildingsLayers()] The Json value was created successfully.");

    // set width && height of the world
    for (const auto& [k, v] : j.items())
    {
        if (k == "width")
        {
            width = v.get<int>();
        }
        else if (k == "height")
        {
            height = v.get<int>();
        }
    }

    if (width < 0 || height < 0)
    {
        throw MDCII_EXCEPTION("[World::CreateTerrainAndBuildingsLayers()] Invalid width or height given.");
    }

    Log::MDCII_LOG_DEBUG("[World::CreateTerrainAndBuildingsLayers()] World width: {}", width);
    Log::MDCII_LOG_DEBUG("[World::CreateTerrainAndBuildingsLayers()] World height: {}", height);

    // create terrain && buildings layer
    Log::MDCII_LOG_DEBUG("[World::CreateTerrainAndBuildingsLayers()] Starts creating Terrain and Buildings Layer...");
    for (const auto& [k, v] : j.items())
    {
        if (k == "layers")
        {
            for (const auto& [a, o] : v.items())
            {
                auto layer{ std::make_unique<WorldLayer>(this) };

                for (const auto& [layerName, layerTiles] : o.items())
                {
                    layer->SetLayerTypeByString(layerName);
                    MDCII_ASSERT(layer->layerType != WorldLayerType::NONE, "[World::CreateTerrainAndBuildingsLayers()] Invalid Layer type.")

                    for (const auto& [i, tile] : layerTiles.items())
                    {
                        layer->AddTileFromJson(tile);
                    }
                }

                layers.emplace_back(std::move(layer));
            }
        }
    }

    MDCII_ASSERT(layers.size() == 2, "[World::CreateTerrainAndBuildingsLayers()] Invalid number of Layers.")
    MDCII_ASSERT((static_cast<size_t>(width) * static_cast<size_t>(height)) == layers.at(0)->tiles.size(), "[World::CreateTerrainAndBuildingsLayers()] Invalid number of tiles.")
    MDCII_ASSERT((static_cast<size_t>(width) * static_cast<size_t>(height)) == layers.at(1)->tiles.size(), "[World::CreateTerrainAndBuildingsLayers()] Invalid number of tiles.")

    Log::MDCII_LOG_DEBUG("[World::CreateTerrainAndBuildingsLayers()] The Terrain and Buildings Layer has been created successfully.");
}

void mdcii::world::World::PrepareRendering()
{
    MDCII_ASSERT(layers.size() == 2, "[World::PrepareRendering()] Invalid number of layers.")

    Log::MDCII_LOG_DEBUG("[World::PrepareRendering()] Prepares the Terrain and Buildings Layer for rendering.");

    auto& terrainLayer{ GetLayer(WorldLayerType::TERRAIN) };
    auto& buildingsLayer{ GetLayer(WorldLayerType::BUILDINGS) };

    // pre-calc tiles
    for (auto y{ 0 }; y < height; ++y)
    {
        for (auto x{ 0 }; x < width; ++x)
        {
            PreCalcTile(terrainLayer.tiles.at(GetMapIndex(x, y)), x, y);
            PreCalcTile(buildingsLayer.tiles.at(GetMapIndex(x, y)), x, y);
        }
    }

    terrainLayer.PrepareRendering();
    buildingsLayer.PrepareRendering();
}

void mdcii::world::World::MergeTerrainAndBuildingsLayers()
{
    Log::MDCII_LOG_DEBUG("[World::MergeTerrainAndBuildingsLayers()] Merge Terrain Layer with Buildings Layer into a new Layer.");

    MDCII_ASSERT(layers.size() == 2, "[World::MergeLayer()] Invalid number of Layers.")

    // get the existing layers
    const auto& terrainLayer{ GetLayer(WorldLayerType::TERRAIN) };
    const auto& buildingsLayer{ GetLayer(WorldLayerType::BUILDINGS) };

    // create a new layer
    auto layer{ std::make_unique<WorldLayer>(this) };
    layer->layerType = WorldLayerType::TERRAIN_AND_BUILDINGS;
    layer->tiles = terrainLayer.tiles;
    layer->sortedTiles = terrainLayer.sortedTiles;
    layer->modelMatrices = terrainLayer.modelMatrices;
    layer->textureAtlasIndices = terrainLayer.textureAtlasIndices;
    layer->offsets = terrainLayer.offsets;
    layer->heights = terrainLayer.heights;

    // merge

    // for each zoom
    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        // for each rotation
        magic_enum::enum_for_each<map::Rotation>([&](const map::Rotation t_rotation) {
            const auto z{ magic_enum::enum_integer(t_zoom) };
            const auto r{ magic_enum::enum_integer(t_rotation) };

            auto& mt{ layer->modelMatrices.at(z).at(r) };
            const auto& mb{ buildingsLayer.modelMatrices.at(z).at(r) };

            auto& it{ layer->textureAtlasIndices.at(z) };
            const auto& ib{ buildingsLayer.textureAtlasIndices.at(z) };

            auto& ot{ layer->offsets.at(z).at(r) };
            const auto& ob{ buildingsLayer.offsets.at(z).at(r) };

            auto& ht{ layer->heights.at(z) };
            const auto& hb{ buildingsLayer.heights.at(z) };

            // for each tile
            auto i{ 0 };
            for (const auto& mapTile : buildingsLayer.sortedTiles.at(r))
            {
                if (mapTile.HasBuilding())
                {
                    mt.at(i) = mb.at(i);
                    it.at(i)[r] = ib.at(i)[r];
                    ot.at(i) = ob.at(i);
                    ht.at(i)[r] = hb.at(i)[r];
                }

                i++;
            }
        });
    });

    // store new layer
    layers.emplace_back(std::move(layer));

    MDCII_ASSERT(layers.size() == 3, "[World::MergeLayer()] Invalid number of Layers.")
}

void mdcii::world::World::CreateGridLayer()
{
    Log::MDCII_LOG_DEBUG("[World::CreateGridLayer()] Starts creating Grid Layer...");

    MDCII_ASSERT(layers.size() == 3, "[World::CreateGridLayer()] Invalid number of Layers.")

    // get the Terrain Layer
    const auto& terrainLayer{ GetLayer(WorldLayerType::TERRAIN) };

    // create a new Layer
    auto layer{ std::make_unique<WorldLayer>(this) };
    layer->layerType = WorldLayerType::GRID;
    layer->tiles = terrainLayer.tiles; // to get the nr. of instances
    layer->modelMatrices = terrainLayer.modelMatrices;
    layers.emplace_back(std::move(layer));

    MDCII_ASSERT(layers.size() == 4, "[World::CreateGridLayer()] Invalid number of Layers.")

    Log::MDCII_LOG_DEBUG("[World::CreateGridLayer()] The Grid Layer has been created successfully.");
}

void mdcii::world::World::PreCalcTile(Tile& t_tile, const int t_x, const int t_y) const
{
    // set world position
    t_tile.worldX = t_x;
    t_tile.worldY = t_y;

    // pre-calculate the position on the screen for each zoom and each rotation
    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        std::array<glm::vec2, map::NR_OF_ROTATIONS> positions{};

        positions[0] = WorldToScreen(t_x, t_y, t_zoom, map::Rotation::DEG0);
        positions[1] = WorldToScreen(t_x, t_y, t_zoom, map::Rotation::DEG90);
        positions[2] = WorldToScreen(t_x, t_y, t_zoom, map::Rotation::DEG180);
        positions[3] = WorldToScreen(t_x, t_y, t_zoom, map::Rotation::DEG270);

        t_tile.screenPositions.at(magic_enum::enum_integer(t_zoom)) = positions;
    });

    // pre-calculate the index for each rotation for sorting
    t_tile.indices[0] = GetMapIndex(t_x, t_y, map::Rotation::DEG0);
    t_tile.indices[1] = GetMapIndex(t_x, t_y, map::Rotation::DEG90);
    t_tile.indices[2] = GetMapIndex(t_x, t_y, map::Rotation::DEG180);
    t_tile.indices[3] = GetMapIndex(t_x, t_y, map::Rotation::DEG270);

    // pre-calculate a gfx for each rotation
    if (t_tile.HasBuilding())
    {
        const auto building{ context->originalResourcesManager->GetBuildingById(t_tile.buildingId) };
        const auto gfx0{ building.gfx };

        t_tile.gfxs.push_back(gfx0);
        if (building.rotate > 0)
        {
            t_tile.gfxs.push_back(gfx0 + (1 * building.rotate));
            t_tile.gfxs.push_back(gfx0 + (2 * building.rotate));
            t_tile.gfxs.push_back(gfx0 + (3 * building.rotate));
        }
    }
}

//-------------------------------------------------
// ImGui
//-------------------------------------------------

void mdcii::world::World::ShowActionButtons()
{
    magic_enum::enum_for_each<Action>([&](auto t_val) {
        constexpr Action action{ t_val };
        constexpr int i{ magic_enum::enum_integer(action) };

        if (m_actionButtons[i])
        {
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(7.0f, 0.6f, 0.6f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(7.0f, 0.7f, 0.7f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(7.0f, 0.8f, 0.8f)));

            ImGui::Button(ACTION_NAMES[i].data());
            if (ImGui::IsItemClicked(0))
            {
                if (action != m_currentAction)
                {
                    m_actionButtons[i] = !m_actionButtons[i];
                }
            }

            ImGui::PopStyleColor(3);
            ImGui::PopID();

            ImGui::SameLine();
        }
        else
        {
            if (ImGui::Button(ACTION_NAMES[i].data()))
            {
                std::fill(m_actionButtons.begin(), m_actionButtons.end(), false);
                m_actionButtons[i] = true;
                m_currentAction = action;

                Log::MDCII_LOG_DEBUG("[World::ShowActionButtons()] Change to action: {}", magic_enum::enum_name(m_currentAction));
            }

            ImGui::SameLine();
        }
    });

    ImGui::NewLine();
}
