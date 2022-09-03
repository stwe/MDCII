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

#include "MapLayer.h"
#include "MapContent.h"
#include "TileAtlas.h"
#include "ecs/Components.h"
#include "ecs/EcsUtils.h"
#include "ogl/resource/ResourceManager.h"
#include "file/OriginalResourcesManager.h"
#include "renderer/RenderUtils.h"
#include "state/State.h"

//-------------------------------------------------
// Json
//-------------------------------------------------

void mdcii::map::to_json(nlohmann::json& t_json, const MapTile& t_mapTile)
{
    t_json = nlohmann::json{
        { "id", t_mapTile.buildingId },
        { "rotation", rotation_to_int(t_mapTile.rotation) },
        { "x", t_mapTile.x },
        { "y", t_mapTile.y },
        { "connected", t_mapTile.connectedMapTiles }
    };
}

void mdcii::map::from_json(const nlohmann::json& t_json, MapTile& t_mapTile)
{
    if (t_json.count("id"))
    {
        t_json.at("id").get_to(t_mapTile.buildingId);
    }

    if (t_json.count("rotation"))
    {
        auto r{ 0 };
        t_json.at("rotation").get_to(r);
        t_mapTile.rotation = int_to_rotation(r);
    }

    if (t_json.count("x"))
    {
        t_json.at("x").get_to(t_mapTile.x);
    }

    if (t_json.count("y"))
    {
        t_json.at("y").get_to(t_mapTile.y);
    }

    if (t_json.count("connected"))
    {
        t_json.at("connected").get_to(t_mapTile.connectedMapTiles);
    }
}

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::map::MapLayer::MapLayer(MapContent* t_mapContent)
    : m_mapContent{ t_mapContent }
{
    Log::MDCII_LOG_DEBUG("[MapLayer::MapLayer()] Create MapLayer.");

    MDCII_ASSERT(m_mapContent, "[MapLayer::MapLayer()] Null pointer.")

    Init();
}

mdcii::map::MapLayer::~MapLayer() noexcept
{
    Log::MDCII_LOG_DEBUG("[MapLayer::~MapLayer()] Destruct MapLayer.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const mdcii::map::MapLayer::Model_Matrices_For_Each_Rotation& mdcii::map::MapLayer::GetModelMatrices(const Zoom t_zoom) const
{
    return modelMatrices.at(magic_enum::enum_integer(t_zoom));
}

const mdcii::map::MapTile& mdcii::map::MapLayer::GetTile(const int t_mapX, const int t_mapY) const
{
    return mapTiles.at(m_mapContent->GetMapIndex(t_mapX, t_mapY));
}

mdcii::map::MapTile& mdcii::map::MapLayer::GetTile(const int t_mapX, const int t_mapY)
{
    return mapTiles.at(m_mapContent->GetMapIndex(t_mapX, t_mapY));
}

//-------------------------------------------------
// Setter
//-------------------------------------------------

void mdcii::map::MapLayer::SetLayerTypeByString(const std::string& t_layerType)
{
    const auto newType{ magic_enum::enum_cast<LayerType>(to_upper_case(t_layerType)) };
    if (newType.has_value())
    {
        layerType = newType.value();
    }
    else
    {
        throw MDCII_EXCEPTION("[MapLayer::SetLayerTypeByString()] Invalid layer type " + t_layerType + " given.");
    }
}

//-------------------------------------------------
// Instanced rendering
//-------------------------------------------------

void mdcii::map::MapLayer::SortMapTiles()
{
    // for each rotation
    magic_enum::enum_for_each<Rotation>([&](const Rotation t_rotation) {
        // sort tiles by index
        std::sort(mapTiles.begin(), mapTiles.end(), [&](const MapTile& t_a, const MapTile& t_b) {
            return t_a.indices[magic_enum::enum_integer(t_rotation)] < t_b.indices[magic_enum::enum_integer(t_rotation)];
        });

        // copy sorted tiles
        sortedMapTiles.at(magic_enum::enum_integer(t_rotation)) = mapTiles;
    });
}

void mdcii::map::MapLayer::CreateModelMatrices()
{
    // for each zoom
    magic_enum::enum_for_each<Zoom>([&](const Zoom t_zoom) {

        // to store the model matrices for each rotation
        Model_Matrices_For_Each_Rotation matricesForRotations;

        // for each rotation in the zoom create the model matrices
        magic_enum::enum_for_each<Rotation>([&](const Rotation t_rotation) {

            // to store the model matrices
            Model_Matrices matrices;

            // for each tile
            for (const auto& mapTile : sortedMapTiles.at(magic_enum::enum_integer(t_rotation)))
            {
                // get building
                const auto& building{ m_mapContent->context->originalResourcesManager->GetBuildingById(mapTile.buildingId) };

                // copy position
                auto screenPosition{ mapTile.screenPositions.at(magic_enum::enum_integer(t_zoom)).at(rotation_to_int(t_rotation)) };

                // get gfx
                auto buildingRotation{ mapTile.rotation };
                if (building.rotate > 0)
                {
                    buildingRotation = buildingRotation + t_rotation;
                }
                const auto gfx{ mapTile.gfxs[rotation_to_int(buildingRotation)] };

                // get width && height
                const auto& stadtfldBshTextures{ m_mapContent->context->originalResourcesManager->GetStadtfldBshByZoom(t_zoom) };
                const auto w{ static_cast<float>(stadtfldBshTextures[gfx]->width) };
                const auto h{ static_cast<float>(stadtfldBshTextures[gfx]->height) };

                // get elevation
                auto elevation{ 0.0f };
                if (building.posoffs > 0)
                {
                    elevation = static_cast<float>(get_elevation(t_zoom));
                }

                screenPosition.y -= h - static_cast<float>(get_tile_height(t_zoom));
                screenPosition.y -= elevation;

                // store model matrix
                matrices.emplace_back(renderer::RenderUtils::GetModelMatrix(screenPosition, { w, h }));
            }

            matricesForRotations.at(rotation_to_int(t_rotation)) = matrices;
        });

        modelMatrices.at(magic_enum::enum_integer(t_zoom)) = matricesForRotations;
    });
}

void mdcii::map::MapLayer::CreateTextureInfo()
{
    // for each zoom
    magic_enum::enum_for_each<Zoom>([&](const Zoom t_zoom) {

        // zoom int
        const auto zoom{ magic_enum::enum_integer(t_zoom) };

        // atlas rows
        const auto atlasRows{ TileAtlas::ROWS.at(zoom) };

        // to store info for each rotation
        Texture_Atlas_Indices texIndicesForRotations(GetInstances());
        Texture_Offsets_For_Each_Rotation texOffsetsForRotations;
        Texture_Heights texHeightsForRotations(GetInstances());

        // for each rotation in the zoom
        magic_enum::enum_for_each<Rotation>([&](const Rotation t_rotation) {

            // to store texture offsets
            Texture_Offsets textureOffsets;

            // counter for vec4 types (0 = x, 1 = y, 2 = z, 3 = w)
            auto i{ 0 };

            // for each tile
            for (const auto& mapTile : sortedMapTiles.at(magic_enum::enum_integer(t_rotation)))
            {
                // get gfx
                const auto& building{ m_mapContent->context->originalResourcesManager->GetBuildingById(mapTile.buildingId) };
                auto buildingRotation{ mapTile.rotation };
                if (building.rotate > 0)
                {
                    buildingRotation = buildingRotation + t_rotation;
                }
                const auto gfx{ mapTile.gfxs[rotation_to_int(buildingRotation)] };

                // texture indices
                texIndicesForRotations.at(i)[magic_enum::enum_integer(t_rotation)] = gfx / (atlasRows * atlasRows);

                // offsets
                const auto index{ gfx % (atlasRows * atlasRows) };
                auto offset{ TileAtlas::GetTextureOffset(index, atlasRows) };
                textureOffsets.push_back(offset.x);
                textureOffsets.push_back(offset.y);

                // heights
                const auto& stadtfldBshTextures{ m_mapContent->context->originalResourcesManager->GetStadtfldBshByZoom(t_zoom) };
                const auto h{ static_cast<float>(stadtfldBshTextures.at(gfx)->height) };
                texHeightsForRotations.at(i)[magic_enum::enum_integer(t_rotation)] = h;

                i++;
            }

            texOffsetsForRotations.at(rotation_to_int(t_rotation)) = textureOffsets;
        });

        textureAtlasIndices.at(zoom) = texIndicesForRotations;
        offsets.at(zoom) = texOffsetsForRotations;
        heights.at(zoom) = texHeightsForRotations;
    });
}

//-------------------------------------------------
// Add/replace tile
//-------------------------------------------------

void mdcii::map::MapLayer::AddTileFromJson(const nlohmann::json& t_json)
{
    mapTiles.emplace_back(t_json.get<MapTile>());
}

void mdcii::map::MapLayer::ReplaceTile(const MapTile& t_mapTile)
{
    mapTiles.at(m_mapContent->GetMapIndex(t_mapTile.mapX, t_mapTile.mapY)) = t_mapTile;
}

//-------------------------------------------------
// Ecs
//-------------------------------------------------

void mdcii::map::MapLayer::CreateGridEntity(const int t_mapX, const int t_mapY)
{
    MDCII_ASSERT(layerType == LayerType::TERRAIN, "[MapLayer::CreateGridEntity()] Wrong layer type.")

    // skip tiles without building Id
    if (!GetTile(t_mapX, t_mapY).HasBuilding())
    {
        return;
    }

    // create an EnTT entity
    const auto entity{ ecs::EcsUtils::CreatePlainEntity() };

    // store entity handle
    GetTile(t_mapX, t_mapY).gridEntity = entity;

    // add GridComponent
    Game::ecs.emplace<ecs::GridComponent>(
        entity,
        GetTile(t_mapX, t_mapY),
        ogl::resource::ResourceManager::LoadTexture(m_gridFileNames.at(magic_enum::enum_integer(m_mapContent->zoom))).id
    );
}

void mdcii::map::MapLayer::CreateTerrainLayerEntity(const int t_mapX, const int t_mapY)
{
    MDCII_ASSERT(layerType == LayerType::TERRAIN, "[MapLayer::CreateTerrainLayerEntity()] Wrong layer type.")

    // skip tiles without building Id
    if (!GetTile(t_mapX, t_mapY).HasBuilding())
    {
        return;
    }

    // create an EnTT entity
    const auto entity{ ecs::EcsUtils::CreatePlainEntity() };

    // store entity handle
    GetTile(t_mapX, t_mapY).entity = entity;

    // add TerrainLayerTileComponent
    Game::ecs.emplace<ecs::TerrainLayerTileComponent>(
        entity,
        GetTile(t_mapX, t_mapY),
        m_mapContent->context->originalResourcesManager->GetBuildingById(GetTile(t_mapX, t_mapY).buildingId)
    );
}

void mdcii::map::MapLayer::AddBuildingsLayerComponent(const int t_mapX, const int t_mapY)
{
    MDCII_ASSERT(layerType == LayerType::BUILDINGS, "[MapLayer::AddBuildingsLayerComponent()] Wrong layer type.")

    const auto& terrainTile{ m_mapContent->GetLayer(LayerType::TERRAIN).GetTile(t_mapX, t_mapY) };

    // mandatory
    if (!terrainTile.HasBuilding())
    {
        return;
    }

    // skip tiles without building Id
    if (!GetTile(t_mapX, t_mapY).HasBuilding())
    {
        return;
    }

    // store entity handle
    GetTile(t_mapX, t_mapY).entity = terrainTile.entity;
    GetTile(t_mapX, t_mapY).gridEntity = terrainTile.gridEntity;

    // add BuildingsLayerTileComponent
    Game::ecs.emplace<ecs::BuildingsLayerTileComponent>(
        GetTile(t_mapX, t_mapY).entity,
        GetTile(t_mapX, t_mapY),
        m_mapContent->context->originalResourcesManager->GetBuildingById(GetTile(t_mapX, t_mapY).buildingId)
    );
}

//-------------------------------------------------
// Init
//-------------------------------------------------

void mdcii::map::MapLayer::Init()
{
    // create and store the filenames to show an isometric grid for each zoom
    magic_enum::enum_for_each<Zoom>([&](const Zoom t_zoom) {
        const auto zoomStr{ to_lower_case(std::string(magic_enum::enum_name<Zoom>(t_zoom))) };
        const auto fileName{ "textures/" + zoomStr + "/red_" + zoomStr + ".png" };
        m_gridFileNames.at(magic_enum::enum_integer(t_zoom)) = fileName;
    });
}
