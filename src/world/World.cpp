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
#include "Terrain.h"
#include "TileAtlas.h"
#include "WorldGui.h"
#include "state/State.h"
#include "renderer/TerrainRenderer.h"
#include "renderer/GridRenderer.h"
#include "world/Island.h"
#include "camera/Camera.h"
#include "ogl/Window.h"
#include "layer/GridLayer.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::World::World(std::string t_mapFilePath, std::shared_ptr<state::Context> t_context, const state::StateId t_stateId)
    : context{ std::move(t_context) }
    , m_stateId{ t_stateId }
    , m_mapFilePath{ std::move(t_mapFilePath) }
{
    Log::MDCII_LOG_DEBUG("[World::World()] Create World from {}.", m_mapFilePath);

    MDCII_ASSERT(!m_mapFilePath.empty(), "[World::World()] Invalid path given.")
    MDCII_ASSERT(context, "[World::World()] Null pointer.")

    rotation = Rotation::DEG0;
    zoom = Zoom::GFX;
    m_layerType = layer::LayerType::ALL;

    Init();
}

mdcii::world::World::~World() noexcept
{
    Log::MDCII_LOG_DEBUG("[World::~World()] Destruct World.");

    CleanUp();
}

//-------------------------------------------------
// Logic
//-------------------------------------------------

void mdcii::world::World::Update() const
{
    if (m_runAnimations)
    {
        terrainRenderer->Update();
    }
}

void mdcii::world::World::Render() const
{
    for(const auto& island : terrain->islands)
    {
        if (m_layerType == layer::LayerType::COAST)
        {
            terrainRenderer->Render(*island->coastLayer, zoom, rotation);
        }

        if (m_layerType == layer::LayerType::TERRAIN)
        {
            terrainRenderer->Render(*island->terrainLayer, zoom, rotation);
        }

        if (m_layerType == layer::LayerType::BUILDINGS)
        {
            terrainRenderer->Render(*island->buildingsLayer, zoom, rotation);
        }

        if (m_layerType == layer::LayerType::MIXED)
        {
            terrainRenderer->Render(*island->mixedLayer, zoom, rotation);
        }

        if (m_layerType == layer::LayerType::ALL)
        {
            terrainRenderer->Render(*island->coastLayer, zoom, rotation);
            terrainRenderer->Render(*island->mixedLayer, zoom, rotation);
        }
    }

    if (m_renderGridLayer)
    {
        worldGridRenderer->Render(*worldGridLayer, zoom, rotation);
    }
}

void mdcii::world::World::RenderImGui()
{
    auto winW{ static_cast<float>(context->window->GetWidth()) };

    ImGui::SetNextWindowSize(ImVec2(290.0f, 675.0f), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Pos.x + (winW / 1.4f), 4.0f + ImGui::GetMainViewport()->Pos.y), ImGuiCond_Once);

    int windowFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;
    //ImGuiWindowFlags_NoBackground;

    ImGui::SetNextWindowBgAlpha(0.8f);

    ImGui::Begin("World", nullptr, windowFlags);

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 230, 230, 255));

    ImGui::Separator();

    static int e{ magic_enum::enum_integer(m_layerType) };
    ImGui::RadioButton("Coast", &e, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Terrain", &e, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Buildings", &e, 2);
    ImGui::RadioButton("Mixed", &e, 3);
    ImGui::SameLine();
    ImGui::RadioButton("All", &e, 4);

    if (auto layer{ magic_enum::enum_cast<layer::LayerType>(e) }; layer.has_value())
    {
        const auto l{ layer.value() };
        m_layerType = l;
    }

    ImGui::Checkbox("World-Grid", &m_renderGridLayer);
    ImGui::Checkbox("Animations", &m_runAnimations);

    ImGui::Separator();

    m_worldGui->RotateGui();
    ImGui::Separator();

    m_worldGui->ZoomGui();
    ImGui::Separator();

    ImGui::PopStyleColor();

    ImGui::End();
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
// Init
//-------------------------------------------------

void mdcii::world::World::Init()
{
    Log::MDCII_LOG_DEBUG("[World::Init()] Start initializing the world...");

    terrain = std::make_unique<Terrain>(context);
    tileAtlas = std::make_unique<TileAtlas>();
    terrainRenderer = std::make_unique<renderer::TerrainRenderer>(context, tileAtlas);

    nlohmann::json j = read_json_from_file(Game::RESOURCES_REL_PATH + m_mapFilePath);
    for (const auto& [k, v] : j.items())
    {
        if (k == "islands")
        {
            terrain->CreateIslandsFromJson(v);
        }
    }

    worldGridLayer = std::make_unique<layer::GridLayer>(context);
    worldGridLayer->PrepareCpuDataForRendering();
    worldGridLayer->PrepareGpuDataForRendering();

    worldGridRenderer = std::make_unique<renderer::GridRenderer>(context);

    m_worldGui = std::make_unique<WorldGui>(this);

    MDCII_ASSERT(!terrain->islands.empty(), "[World::Init()] No islands created.")

    Log::MDCII_LOG_DEBUG("[World::Init()] The world was successfully initialized.");
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::world::World::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[World::CleanUp()] CleanUp World.");
}
