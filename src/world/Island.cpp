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

#include "Island.h"
#include "state/State.h"
#include "MdciiAssert.h"
#include "Terrain.h"
#include "layer/TerrainLayer.h"
#include "layer/GridLayer.h"
#include "physics/Aabb.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::Island::Island(std::shared_ptr<state::Context> t_context, Terrain* t_terrain)
    : m_context{ std::move(t_context) }
    , m_terrain{ t_terrain }
{
    Log::MDCII_LOG_DEBUG("[Island::Island()] Create Island.");

    MDCII_ASSERT(m_context, "[Island::Island()] Null pointer.")
    MDCII_ASSERT(m_terrain, "[Island::Island()] Null pointer.")
}

mdcii::world::Island::~Island() noexcept
{
    Log::MDCII_LOG_DEBUG("[Island::~Island()] Destruct Island.");
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::world::Island::InitValuesFromJson(const nlohmann::json& t_json)
{
    Log::MDCII_LOG_DEBUG("[Island::InitValuesFromJson()] Start initialize an island...");

    for (const auto& [k, v] : t_json.items())
    {
        if (k == "width")
        {
            width = v.get<int32_t>();
        }
        if (k == "height")
        {
            height = v.get<int32_t>();
        }
        if (k == "x")
        {
            worldX = v.get<int32_t>();
        }
        if (k == "y")
        {
            worldY = v.get<int32_t>();
        }
    }

    MDCII_ASSERT(width > 0, "[Island::InitValuesFromJson()] Invalid width.")
    MDCII_ASSERT(height > 0, "[Island::InitValuesFromJson()] Invalid height.")
    MDCII_ASSERT(worldX >= 0, "[Island::InitValuesFromJson()] Invalid world x.")
    MDCII_ASSERT(worldY >= 0, "[Island::InitValuesFromJson()] Invalid world y.")

    aabb = std::make_unique<physics::Aabb>(glm::ivec2(worldX, worldY), glm::ivec2(width, height));

    for (const auto& [k, v] : t_json.items())
    {
        if (k == "layers")
        {
            CreateLayersFromJson(v);
        }
    }

    Log::MDCII_LOG_DEBUG("[Island::InitValuesFromJson()] The island have been initialized successfully.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

bool mdcii::world::Island::IsWorldPositionInAabb(const glm::ivec2& t_position) const
{
    MDCII_ASSERT(aabb, "[Island::IsPositionOnIsland()] Null pointer.")

    return physics::Aabb::PointVsAabb(t_position, *aabb);
}

glm::ivec2 mdcii::world::Island::GetIslandPositionFromWorldPosition(const glm::ivec2& t_position) const
{
    MDCII_ASSERT(IsWorldPositionInAabb(t_position), "[Island::GetIslandPositionFromWorldPosition()] Invalid world position.")

    return { t_position.x - worldX, t_position.y - worldY };
}

const mdcii::layer::Tile& mdcii::world::Island::GetCoastTileFromCurrentPosition() const
{
    // todo: check is in Island
    return coastLayer->GetTile(currentPosition);
}

const mdcii::layer::Tile& mdcii::world::Island::GetTerrainTileFromCurrentPosition() const
{
    return terrainLayer->GetTile(currentPosition);
}

const mdcii::layer::Tile& mdcii::world::Island::GetBuildingTileFromCurrentPosition() const
{
    return buildingsLayer->GetTile(currentPosition);
}

//-------------------------------------------------
// Render
//-------------------------------------------------

void mdcii::world::Island::RenderImGui() const
{
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0.7f, 0.8f, 0.8f)));
    ImGui::Text("Island");
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Text("Start World x: %d", worldX);
    ImGui::Text("Start World y: %d", worldY);
    ImGui::Text("Width: %d", width);
    ImGui::Text("Height: %d", height);
    ImGui::Text("Current selected x: %d", currentPosition.x);
    ImGui::Text("Current selected y: %d", currentPosition.y);
}

//-------------------------------------------------
// Layer
//-------------------------------------------------

void mdcii::world::Island::CreateLayersFromJson(const nlohmann::json& t_json)
{
    Log::MDCII_LOG_DEBUG("[Island::CreateLayersFromJson()] Create the Layer objects.");

    for (const auto& [k, v] : t_json.items())
    {
        for (const auto& [layerNameJson, layerTilesJson] : v.items())
        {
            if (layerNameJson == "coast")
            {
                coastLayer = std::make_unique<layer::TerrainLayer>(m_context, m_terrain->world, this);
                coastLayer->CreateTilesFromJson(layerTilesJson);
                coastLayer->PrepareCpuDataForRendering();
                coastLayer->PrepareGpuDataForRendering();
            }

            if (layerNameJson == "terrain")
            {
                terrainLayer = std::make_unique<layer::TerrainLayer>(m_context, m_terrain->world, this);
                terrainLayer->CreateTilesFromJson(layerTilesJson);
                terrainLayer->PrepareCpuDataForRendering();
                terrainLayer->PrepareGpuDataForRendering();
            }

            if (layerNameJson == "buildings")
            {
                buildingsLayer = std::make_unique<layer::TerrainLayer>(m_context, m_terrain->world, this);
                buildingsLayer->CreateTilesFromJson(layerTilesJson);
                buildingsLayer->PrepareCpuDataForRendering();
                buildingsLayer->PrepareGpuDataForRendering();
            }
        }
    }

    MDCII_ASSERT(terrainLayer, "[Island::CreateLayersFromJson()] Null pointer.")
    MDCII_ASSERT(buildingsLayer, "[Island::CreateLayersFromJson()] Null pointer.")

    mixedLayer = std::make_unique<layer::TerrainLayer>(m_context, m_terrain->world, this);
    mixedLayer->instancesToRender = terrainLayer->instancesToRender;
    mixedLayer->modelMatrices = terrainLayer->modelMatrices;
    mixedLayer->gfxNumbers = terrainLayer->gfxNumbers;
    mixedLayer->buildingIds = terrainLayer->buildingIds;

    magic_enum::enum_for_each<Zoom>([this](const Zoom t_zoom) {
        magic_enum::enum_for_each<Rotation>([this, &t_zoom](const Rotation t_rotation) {
            const auto z{ magic_enum::enum_integer(t_zoom) };
            const auto r{ magic_enum::enum_integer(t_rotation) };

            auto& mt{ mixedLayer->modelMatrices.at(z).at(r) };
            const auto& mb{ buildingsLayer->modelMatrices.at(z).at(r) };

            auto& gt{ mixedLayer->gfxNumbers };
            const auto& gb{ buildingsLayer->gfxNumbers };

            auto& bt{ mixedLayer->buildingIds };
            const auto& bb{ buildingsLayer->buildingIds };

            auto instance{ 0 };
            for (const auto& mapTile : buildingsLayer->sortedTiles.at(r))
            {
                if (mapTile->HasBuilding())
                {
                    mt.at(instance) = mb.at(instance);
                    gt.at(instance)[r] = gb.at(instance)[r];
                    bt.at(instance)[r] = bb.at(instance)[r];
                }

                instance++;
            }
        });
    });

    mixedLayer->PrepareGpuDataForRendering();

    gridLayer = std::make_unique<layer::GridLayer>(m_context, m_terrain->world);
    gridLayer->sortedTiles = terrainLayer->sortedTiles;
    gridLayer->PrepareCpuDataForRendering();
    gridLayer->PrepareGpuDataForRendering();
}