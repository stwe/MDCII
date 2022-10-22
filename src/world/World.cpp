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

#include <glm/gtx/hash.hpp>
#include <imgui.h>
#include "World.h"
#include "Game.h"
#include "TileAtlas.h"
#include "MdciiAssert.h"
#include "MousePicker.h"
#include "WorldGui.h"
#include "event/EventManager.h"
#include "eventpp/utilities/argumentadapter.h"
#include "state/State.h"
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

const mdcii::world::WorldLayer& mdcii::world::World::GetLayer(WorldLayerType t_layerType) const
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

    ImGui::Separator();

    // select layer to render
    static int e{ magic_enum::enum_integer(m_renderLayerType) };
    ImGui::RadioButton("Terrain", &e, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Buildings", &e, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Terrain && Buildings", &e, 2);

    if (auto layer{ magic_enum::enum_cast<WorldLayerType>(e) }; layer.has_value())
    {
        const auto l{ layer.value() };
        m_renderLayerType = l;
    }

    // toggle grid
    ImGui::Checkbox("Grid", &m_renderGridLayer);

    ImGui::Separator();

    // world gui
    m_worldGui->RotateGui();
    ImGui::Separator();

    m_worldGui->ZoomGui();
    ImGui::Separator();

    m_worldGui->ShowActionsGui();
    ImGui::Separator();

    if (currentAction == Action::BUILD)
    {
        m_currentTileIndex = -1;
        m_worldGui->AllWorkshopsGui();
    }

    if (currentAction == Action::STATUS)
    {
        if (m_worldGui->selectedWorkshop.HasBuilding())
        {
            m_worldGui->selectedWorkshop.Reset();
        }

        if (m_currentTileIndex >= 0)
        {
            const auto& terrainLayerTile{ *GetLayer(WorldLayerType::TERRAIN).tiles.at(m_currentTileIndex) };
            const auto& buildingsLayerTile{ *GetLayer(WorldLayerType::BUILDINGS).tiles.at(m_currentTileIndex) };

            if (buildingsLayerTile.HasBuilding())
            {
                buildingsLayerTile.RenderImGui();
            }
            else
            {
                terrainLayerTile.RenderImGui();
            }
        }
    }

    if (currentAction == Action::OPTIONS)
    {
        m_currentTileIndex = -1;
        if (m_worldGui->selectedWorkshop.HasBuilding())
        {
            m_worldGui->selectedWorkshop.Reset();
        }

        m_worldGui->SaveGameGui();
    }

    ImGui::End();

    // MousePicker Gui
    mousePicker->RenderImGui();
}

//-------------------------------------------------
// Rotate && Zoom
//-------------------------------------------------

void mdcii::world::World::RotateWorld(const ChangeRotation t_changeRotation)
{
    switch (t_changeRotation)
    {
    case ChangeRotation::LEFT:
        --rotation;
        break;
    case ChangeRotation::RIGHT:
        ++rotation;
        break;
    }
}

void mdcii::world::World::ZoomWorld(const ChangeZoom t_changeZoom)
{
    switch (t_changeZoom)
    {
    case ChangeZoom::ZOOM_IN:
        ++zoom;
        context->camera->zoom = zoom;
        break;
    case ChangeZoom::ZOOM_OUT:
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

int mdcii::world::World::GetMapIndex(const int t_x, const int t_y, const Rotation t_rotation) const
{
    MDCII_ASSERT(IsPositionInWorld(t_x, t_y), "[World::GetMapIndex()] Invalid world position given.")

    const auto position{ RotatePosition(t_x, t_y, t_rotation) };

    if (t_rotation == Rotation::DEG0 || t_rotation == Rotation::DEG180)
    {
        return position.y * width + position.x;
    }

    return position.y * height + position.x;
}

glm::vec2 mdcii::world::World::WorldToScreen(const int t_x, const int t_y, const Zoom t_zoom, const Rotation t_rotation) const
{
    MDCII_ASSERT(IsPositionInWorld(t_x, t_y), "[World::WorldToScreen()] Invalid world position given.")

    const auto position{ RotatePosition(t_x, t_y, t_rotation) };

    return {
        (position.x - position.y) * get_tile_width_half(t_zoom),
        (position.x + position.y) * get_tile_height_half(t_zoom)
    };
}

glm::ivec2 mdcii::world::World::RotatePosition(const int t_x, const int t_y, const Rotation t_rotation) const
{
    return rotate_position(t_x, t_y, width, height, t_rotation);
}

//-------------------------------------------------
// Event handler
//-------------------------------------------------

void mdcii::world::World::OnLeftMouseButtonPressed()
{
    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    const auto& mousePosition{ mousePicker->currentPosition };

    if (IsPositionInWorld(mousePosition.x, mousePosition.y) && currentAction == Action::STATUS)
    {
        m_currentTileIndex = GetMapIndex(mousePosition.x, mousePosition.y);
    }

    if (IsPositionInWorld(mousePosition.x, mousePosition.y) && currentAction == Action::BUILD && !m_tilesToAdd.empty())
    {
        auto& buildingsLayer{ GetLayer(WorldLayerType::BUILDINGS) };

        // reset Tile pointers and replace with new Tile
        for (auto& tile : m_tilesToAdd)
        {
            buildingsLayer.ResetTilePointersAt(tile->instanceIds);
            buildingsLayer.StoreTile(std::move(tile));
        }

        // clear m_tilesToAdd vector
        std::vector<std::unique_ptr<Tile>>().swap(m_tilesToAdd);
    }

    MDCII_ASSERT(m_tilesToAdd.empty(), "[World::OnLeftMouseButtonPressed()] Invalid number of tiles to add.")

    // reset selected workshop on left mouse button clicked
    m_worldGui->selectedWorkshop.Reset();
}

void mdcii::world::World::OnMouseMoved()
{
    // do nothing (return) when the mouse is over the ImGui window
    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }

    // get current mouse position
    const auto& currentMousePosition{ mousePicker->currentPosition };

    // condition to build: BUILD action + workshop selected + current mouse in the world?
    if (currentAction == Action::BUILD && m_worldGui->selectedWorkshop.HasBuilding() && IsPositionInWorld(currentMousePosition.x, currentMousePosition.y))
    {
        // get layers
        auto& terrainLayer{ GetLayer(WorldLayerType::TERRAIN) };
        const auto& buildingsLayer{ GetLayer(WorldLayerType::BUILDINGS) };
        const auto& mixedLayer{ GetLayer(WorldLayerType::TERRAIN_AND_BUILDINGS) };

        // get building
        const auto& building{ context->originalResourcesManager->GetBuildingById(m_worldGui->selectedWorkshop.buildingId) };

        // only add if the entire building fits on the world
        if (IsBuildingOutsideTheWorld(currentMousePosition.x, currentMousePosition.y, building))
        {
            Log::MDCII_LOG_DEBUG("[World::OnMouseMoved()] The building is outside the world on x: {}, y: {}.", currentMousePosition.x, currentMousePosition.y);
            return;
        }

        // only add it if there is no other building on the position
        if (buildingsLayer.IsAlreadyBuildingOnPosition(currentMousePosition.x, currentMousePosition.y, building))
        {
            Log::MDCII_LOG_DEBUG("[World::OnMouseMoved()] There is an other building on the position x: {}, y: {}.", currentMousePosition.x, currentMousePosition.y);
            return;
        }

        // don't build on the coast
        if (IsBuildingOnWaterOrCoast(currentMousePosition.x, currentMousePosition.y, building))
        {
            Log::MDCII_LOG_DEBUG("[World::OnMouseMoved()] It cannot be built on the coast on position x: {}, y: {}.", currentMousePosition.x, currentMousePosition.y);
            return;
        }

        ///////////////
        // Delete
        ///////////////

        if (!m_tilesToAdd.empty())
        {
            Log::MDCII_LOG_DEBUG("[World::OnMouseMoved()] Delete building with Id {} from Gpu.", m_worldGui->selectedWorkshop.buildingId);

            for (const auto& tile : m_tilesToAdd)
            {
                magic_enum::enum_for_each<Zoom>([this, &terrainLayer, &tile](const Zoom t_zoom) {
                    const auto zoomInt{ magic_enum::enum_integer(t_zoom) };
                    magic_enum::enum_for_each<Rotation>([this, &terrainLayer, &zoomInt, &tile, &t_zoom](const Rotation t_rotation) {
                        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

                        const auto& tm{ terrainLayer.modelMatrices.at(zoomInt).at(rotationInt) };
                        const auto& ti{ terrainLayer.textureAtlasNumbers.at(zoomInt) };
                        const auto& to{ terrainLayer.offsets.at(zoomInt).at(rotationInt) };
                        const auto& th{ terrainLayer.heights.at(zoomInt) };

                        // delete: update Gpu data BUILDINGS
                        worldRenderer->UpdateGpuData(
                            tile->instanceIds.at(rotationInt),
                            WorldLayerType::BUILDINGS,
                            t_zoom, t_rotation,
                            glm::mat4(),
                            -1,
                            glm::vec2(-1.0f),
                            -1.0f
                        );

                        // delete: update Gpu data TERRAIN_AND_BUILDINGS
                        worldRenderer->UpdateGpuData(
                            tile->instanceIds.at(rotationInt),
                            WorldLayerType::TERRAIN_AND_BUILDINGS,
                            t_zoom, t_rotation,
                            tm.at(tile->instanceIds.at(rotationInt)),
                            ti.at(tile->instanceIds.at(rotationInt))[rotationInt],
                            to.at(tile->instanceIds.at(rotationInt)),
                            th.at(tile->instanceIds.at(rotationInt))[rotationInt]
                        );
                    });
                });
            }

            // clear vector
            std::vector<std::unique_ptr<Tile>>().swap(m_tilesToAdd);
            MDCII_ASSERT(m_tilesToAdd.empty(), "[World::OnMouseMoved()] Invalid number of tiles to add.")
        }

        ///////////////
        // Create
        ///////////////

        if (m_tilesToAdd.empty())
        {
            Log::MDCII_LOG_DEBUG("[World::OnMouseMoved()] Add building with Id {} to Gpu.", m_worldGui->selectedWorkshop.buildingId);

            for (auto y{ 0 }; y < building.size.h; ++y)
            {
                for (auto x{ 0 }; x < building.size.w; ++x)
                {
                    // create a Tile pointer for each part of the building
                    auto tile{ std::make_unique<Tile>() };
                    tile->buildingId = m_worldGui->selectedWorkshop.buildingId;
                    tile->rotation = m_worldGui->selectedWorkshop.rotation;
                    tile->x = x;
                    tile->y = y;

                    // pre-calc and set screen positions / indices / gfx
                    PreCalcTile(*tile, currentMousePosition.x + x, currentMousePosition.y + y);

                    // copy the instances Ids from Terrain Layer Tile at the same position
                    magic_enum::enum_for_each<Rotation>([&tile, &terrainLayer, &currentMousePosition, &x, &y](const Rotation t_rotation) {
                        const auto r{ magic_enum::enum_integer(t_rotation) };
                        tile->instanceIds.at(r) = terrainLayer.instanceIds.at(glm::ivec3(currentMousePosition.x + x, currentMousePosition.y + y, r)) ;
                    });

                    // create Gpu data for each zoom and each rotation
                    magic_enum::enum_for_each<Zoom>([this, &mixedLayer, &tile](const Zoom t_zoom) {
                        magic_enum::enum_for_each<Rotation>([this, &mixedLayer, &tile, &t_zoom](const Rotation t_rotation) {
                            const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

                            // create new Gpu data
                            const auto modelMatrix{ mixedLayer.GetModelMatrix(*tile, t_zoom, t_rotation) };
                            const auto atlasNr{ mixedLayer.GetTextureAtlasNr(*tile, t_zoom, t_rotation) };
                            const auto texOffset{ mixedLayer.GetTextureOffset(*tile, t_zoom, t_rotation) };
                            const auto texHeight{ mixedLayer.GetTextureHeight(*tile, t_zoom, t_rotation) };

                            // add: update Gpu data BUILDINGS
                            worldRenderer->UpdateGpuData(
                                tile->instanceIds.at(rotationInt),
                                WorldLayerType::BUILDINGS,
                                t_zoom, t_rotation,
                                modelMatrix,
                                atlasNr,
                                texOffset,
                                texHeight
                            );

                            // add: update Gpu data TERRAIN_AND_BUILDINGS
                            worldRenderer->UpdateGpuData(
                                tile->instanceIds.at(rotationInt),
                                WorldLayerType::TERRAIN_AND_BUILDINGS,
                                t_zoom, t_rotation,
                                modelMatrix,
                                atlasNr,
                                texOffset,
                                texHeight
                            );
                        });
                    });

                    m_tilesToAdd.emplace_back(std::move(tile));
                }
            }

            MDCII_ASSERT(m_tilesToAdd.size() == building.size.w * building.size.h, "[World::OnMouseMoved()] Invalid number of created tiles.")
        }
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

    tileAtlas = std::make_unique<TileAtlas>();
    worldRenderer = std::make_unique<renderer::WorldRenderer>(this);
    mousePicker = std::make_unique<MousePicker>(this, *context->window, *context->camera);
    m_worldGui = std::make_unique<WorldGui>(this);

    Log::MDCII_LOG_DEBUG("[World::Init()] The world was successfully initialized.");
}

void mdcii::world::World::AddListeners()
{
    Log::MDCII_LOG_DEBUG("[World::AddListeners()] Add listeners.");

    // OnLeftMouseButtonPressed
    event::EventManager::event_dispatcher.appendListener(
        event::MdciiEventType::MOUSE_BUTTON_PRESSED,
        eventpp::argumentAdapter<void(const event::MouseButtonPressedEvent&)>(
            [this](const event::MouseButtonPressedEvent& t_event) {
                if (t_event.button == 0)
                {
                    OnLeftMouseButtonPressed();
                }
            }
        )
    );

    // OnMouseMoved
    event::EventManager::event_dispatcher.appendListener(
        event::MdciiEventType::MOUSE_MOVED,
        eventpp::argumentAdapter<void(const event::MouseMovedEvent&)>(
            [this]([[maybe_unused]] const event::MouseMovedEvent& t_event) {
                OnMouseMoved();
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

                    for (const auto& [i, tileJson] : layerTiles.items())
                    {
                        layer->AddTileFromJson(tileJson);
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
            PreCalcTile(*terrainLayer.tiles.at(GetMapIndex(x, y)), x, y);
            PreCalcTile(*buildingsLayer.tiles.at(GetMapIndex(x, y)), x, y);
        }
    }

    terrainLayer.instancesToRender = static_cast<int32_t>(terrainLayer.tiles.size());
    buildingsLayer.instancesToRender = static_cast<int32_t>(buildingsLayer.tiles.size());

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

    // set type and number of instances
    layer->layerType = WorldLayerType::TERRAIN_AND_BUILDINGS;
    layer->instancesToRender = terrainLayer.instancesToRender;

    // set Gpu data only
    layer->modelMatrices = terrainLayer.modelMatrices;
    layer->textureAtlasNumbers = terrainLayer.textureAtlasNumbers;
    layer->offsets = terrainLayer.offsets;
    layer->heights = terrainLayer.heights;

    // merge Gpu data
    magic_enum::enum_for_each<Zoom>([&layer, &buildingsLayer](const Zoom t_zoom) {
        magic_enum::enum_for_each<Rotation>([&t_zoom, &layer, &buildingsLayer](const Rotation t_rotation) {
            const auto z{ magic_enum::enum_integer(t_zoom) };
            const auto r{ magic_enum::enum_integer(t_rotation) };

            auto& mt{ layer->modelMatrices.at(z).at(r) };
            const auto& mb{ buildingsLayer.modelMatrices.at(z).at(r) };

            auto& it{ layer->textureAtlasNumbers.at(z) };
            const auto& ib{ buildingsLayer.textureAtlasNumbers.at(z) };

            auto& ot{ layer->offsets.at(z).at(r) };
            const auto& ob{ buildingsLayer.offsets.at(z).at(r) };

            auto& ht{ layer->heights.at(z) };
            const auto& hb{ buildingsLayer.heights.at(z) };

            // for each tile
            auto i{ 0 };
            for (const auto& mapTile : buildingsLayer.sortedTiles.at(r))
            {
                if (mapTile->HasBuilding())
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
    layer->instancesToRender = terrainLayer.instancesToRender;
    layer->modelMatrices = terrainLayer.modelMatrices;
    layers.emplace_back(std::move(layer));

    MDCII_ASSERT(layers.size() == 4, "[World::CreateGridLayer()] Invalid number of Layers.")

    Log::MDCII_LOG_DEBUG("[World::CreateGridLayer()] The Grid Layer has been created successfully.");
}

void mdcii::world::World::PreCalcTile(Tile& t_tile, const int t_x, const int t_y) const
{
    // set world position for Deg0
    t_tile.worldXDeg0 = t_x;
    t_tile.worldYDeg0 = t_y;

    // pre-calculate the position on the screen for each zoom and each rotation
    magic_enum::enum_for_each<Zoom>([this, &t_x, &t_y, &t_tile](const Zoom t_zoom) {
        std::array<glm::vec2, NR_OF_ROTATIONS> positions{};

        positions[0] = WorldToScreen(t_x, t_y, t_zoom, Rotation::DEG0);
        positions[1] = WorldToScreen(t_x, t_y, t_zoom, Rotation::DEG90);
        positions[2] = WorldToScreen(t_x, t_y, t_zoom, Rotation::DEG180);
        positions[3] = WorldToScreen(t_x, t_y, t_zoom, Rotation::DEG270);

        t_tile.screenPositions.at(magic_enum::enum_integer(t_zoom)) = positions;
    });

    // pre-calculate the index for each rotation for sorting
    t_tile.indices[0] = GetMapIndex(t_x, t_y, Rotation::DEG0);
    t_tile.indices[1] = GetMapIndex(t_x, t_y, Rotation::DEG90);
    t_tile.indices[2] = GetMapIndex(t_x, t_y, Rotation::DEG180);
    t_tile.indices[3] = GetMapIndex(t_x, t_y, Rotation::DEG270);

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
// Add building
//-------------------------------------------------

bool mdcii::world::World::IsBuildingOutsideTheWorld(const int t_x, const int t_y, const data::Building& t_building) const
{
    for (auto y{ 0 }; y < t_building.size.h; ++y)
    {
        for (auto x{ 0 }; x < t_building.size.w; ++x)
        {
            if (!IsPositionInWorld(t_x + x, t_y + y))
            {
                return true;
            }
        }
    }

    return false;
}

bool mdcii::world::World::IsBuildingOnWaterOrCoast(const int t_x, const int t_y, const data::Building& t_building) const
{
    for (auto y{ 0 }; y < t_building.size.h; ++y)
    {
        for (auto x{ 0 }; x < t_building.size.w; ++x)
        {
            const auto& terrainTile{ GetLayer(WorldLayerType::TERRAIN).GetTile(t_x + x, t_y + y) };
            if (context->originalResourcesManager->GetBuildingById(terrainTile.buildingId).posoffs == 0)
            {
                return true;
            }
        }
    }

    return false;
}
