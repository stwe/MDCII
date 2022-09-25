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

#include "WorldLayer.h"
#include "World.h"
#include "MdciiAssert.h"
#include "state/State.h"
#include "file/OriginalResourcesManager.h"
#include "renderer/RenderUtils.h"
#include "map/TileAtlas.h"

//-------------------------------------------------
// Json
//-------------------------------------------------

void mdcii::world::to_json(nlohmann::json& t_json, const Tile& t_tile)
{
    t_json = nlohmann::json{
        { "id", t_tile.buildingId },
        { "rotation", magic_enum::enum_integer(t_tile.rotation) },
        { "x", t_tile.x },
        { "y", t_tile.y }
    };
}

void mdcii::world::from_json(const nlohmann::json& t_json, Tile& t_tile)
{
    if (t_json.count("id"))
    {
        t_json.at("id").get_to(t_tile.buildingId);
    }

    if (t_json.count("rotation"))
    {
        auto r{ 0 };
        t_json.at("rotation").get_to(r);
        t_tile.rotation = map::int_to_rotation(r);
    }

    if (t_json.count("x"))
    {
        t_json.at("x").get_to(t_tile.x);
    }

    if (t_json.count("y"))
    {
        t_json.at("y").get_to(t_tile.y);
    }
}

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::WorldLayer::WorldLayer(World* t_world)
    : m_world{ t_world }
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::WorldLayer()] Create WorldLayer.");

    MDCII_ASSERT(t_world, "[WorldLayer::WorldLayer()] Null pointer.")
}

mdcii::world::WorldLayer::~WorldLayer() noexcept
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::~WorldLayer()] Destruct WorldLayer.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

const mdcii::world::Tile& mdcii::world::WorldLayer::GetTile(const int t_x, const int t_y) const
{
    return tiles.at(m_world->GetMapIndex(t_x, t_y));
}

mdcii::world::Tile& mdcii::world::WorldLayer::GetTile(const int t_x, const int t_y)
{
    return tiles.at(m_world->GetMapIndex(t_x, t_y));
}

const mdcii::world::WorldLayer::Model_Matrices_For_Each_Rotation& mdcii::world::WorldLayer::GetModelMatrices(const map::Zoom t_zoom) const
{
    return modelMatrices.at(magic_enum::enum_integer(t_zoom));
}

//-------------------------------------------------
// Layer type
//-------------------------------------------------

void mdcii::world::WorldLayer::SetLayerTypeByString(const std::string& t_layerType)
{
    const auto type{ magic_enum::enum_cast<WorldLayerType>(to_upper_case(t_layerType)) };
    if (type.has_value())
    {
        const auto t{ type.value() };
        layerType = t;
    }
    else
    {
        throw MDCII_EXCEPTION("[WorldLayer::SetLayerTypeByString()] Invalid layer type " + t_layerType + " given.");
    }
}

//-------------------------------------------------
// Add/replace tile
//-------------------------------------------------

void mdcii::world::WorldLayer::AddTileFromJson(const nlohmann::json& t_json)
{
    tiles.emplace_back(t_json.get<Tile>());
}

//-------------------------------------------------
// Prepare rendering
//-------------------------------------------------

void mdcii::world::WorldLayer::PrepareRendering()
{
    SortTiles();
    CreateModelMatrices();
    CreateTextureInfo();
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::world::WorldLayer::SortTiles()
{
    // for each rotation
    magic_enum::enum_for_each<map::Rotation>([&](const map::Rotation t_rotation) {
        // sort tiles by index
        std::sort(tiles.begin(), tiles.end(), [&](const Tile& t_a, const Tile& t_b) {
            return t_a.indices[magic_enum::enum_integer(t_rotation)] < t_b.indices[magic_enum::enum_integer(t_rotation)];
        });

        // copy sorted tiles
        sortedTiles.at(magic_enum::enum_integer(t_rotation)) = tiles;
    });
}

void mdcii::world::WorldLayer::CreateModelMatrices()
{
    // for each zoom
    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        // to store the model matrices for each rotation
        Model_Matrices_For_Each_Rotation matricesForRotations;

        // for each rotation in the zoom create the model matrices
        magic_enum::enum_for_each<map::Rotation>([&](const map::Rotation t_rotation) {
            // to store the model matrices
            Model_Matrices matrices;

            // for each tile
            for (const auto& mapTile : sortedTiles.at(magic_enum::enum_integer(t_rotation)))
            {
                // get building
                int32_t buildingId{ GRASS_BUILDING_ID }; // to definitely create a position
                if (mapTile.HasBuilding())
                {
                    buildingId = mapTile.buildingId;
                }

                const auto& building{ m_world->context->originalResourcesManager->GetBuildingById(buildingId) };

                // copy position
                auto screenPosition{ mapTile.screenPositions.at(magic_enum::enum_integer(t_zoom)).at(magic_enum::enum_integer(t_rotation)) };

                // get gfx
                auto buildingRotation{ mapTile.rotation };
                if (building.rotate > 0)
                {
                    buildingRotation = buildingRotation + t_rotation;
                }

                int32_t gfx{ GRASS_GFX }; // to definitely create a position
                if (mapTile.HasBuilding())
                {
                    gfx = mapTile.gfxs[magic_enum::enum_integer(buildingRotation)];
                }

                // todo: create method
                if (building.size.w > 1)
                {
                    // default: orientation 0
                    auto rp{ glm::ivec2(mapTile.x, mapTile.y) };

                    if (mapTile.rotation == map::Rotation::DEG270)
                    {
                        rp = rotate_position(
                            mapTile.x, mapTile.y,
                            building.size.w, building.size.h,
                            map::Rotation::DEG90
                        );
                    }

                    if (mapTile.rotation == map::Rotation::DEG180)
                    {
                        rp = rotate_position(
                            mapTile.x, mapTile.y,
                            building.size.w, building.size.h,
                            map::Rotation::DEG180
                        );
                    }

                    if (mapTile.rotation == map::Rotation::DEG90)
                    {
                        rp = rotate_position(
                            mapTile.x, mapTile.y,
                            building.size.w, building.size.h,
                            map::Rotation::DEG270
                        );
                    }

                    const auto offset{ rp.y * building.size.w + rp.x };
                    gfx += offset;
                }

                // get width && height
                const auto& stadtfldBshTextures{ m_world->context->originalResourcesManager->GetStadtfldBshByZoom(t_zoom) };
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

            matricesForRotations.at(magic_enum::enum_integer(t_rotation)) = matrices;
        });

        modelMatrices.at(magic_enum::enum_integer(t_zoom)) = matricesForRotations;
    });
}

void mdcii::world::WorldLayer::CreateTextureInfo()
{
    // for each zoom
    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        // zoom int
        const auto zoom{ magic_enum::enum_integer(t_zoom) };

        // atlas rows
        const auto atlasRows{ map::TileAtlas::ROWS.at(zoom) };

        // to store info for each rotation
        Texture_Atlas_Indices texIndicesForRotations(GetInstances());
        Texture_Offsets_For_Each_Rotation texOffsetsForRotations;
        Texture_Heights texHeightsForRotations(GetInstances());

        // for each rotation in the zoom
        magic_enum::enum_for_each<map::Rotation>([&](const map::Rotation t_rotation) {
            // to store texture offsets
            Texture_Offsets textureOffsets;

            auto instance{ 0 };

            // for each tile
            for (const auto& mapTile : sortedTiles.at(magic_enum::enum_integer(t_rotation)))
            {
                if (mapTile.HasBuilding())
                {
                    // get gfx
                    const auto& building{ m_world->context->originalResourcesManager->GetBuildingById(mapTile.buildingId) };
                    auto buildingRotation{ mapTile.rotation };
                    if (building.rotate > 0)
                    {
                        buildingRotation = buildingRotation + t_rotation;
                    }
                    auto gfx{ mapTile.gfxs[magic_enum::enum_integer(buildingRotation)] };

                    // todo: create method
                    if (building.size.w > 1)
                    {
                        // default: orientation 0
                        auto rp{ glm::ivec2(mapTile.x, mapTile.y) };

                        if (mapTile.rotation == map::Rotation::DEG270)
                        {
                            rp = rotate_position(
                                mapTile.x, mapTile.y,
                                building.size.w, building.size.h,
                                map::Rotation::DEG90
                            );
                        }

                        if (mapTile.rotation == map::Rotation::DEG180)
                        {
                            rp = rotate_position(
                                mapTile.x, mapTile.y,
                                building.size.w, building.size.h,
                                map::Rotation::DEG180
                            );
                        }

                        if (mapTile.rotation == map::Rotation::DEG90)
                        {
                            rp = rotate_position(
                                mapTile.x, mapTile.y,
                                building.size.w, building.size.h,
                                map::Rotation::DEG270
                            );
                        }

                        const auto offset{ rp.y * building.size.w + rp.x };
                        gfx += offset;
                    }

                    // texture indices
                    texIndicesForRotations.at(instance)[magic_enum::enum_integer(t_rotation)] = gfx / (atlasRows * atlasRows);

                    // offsets
                    const auto index{ gfx % (atlasRows * atlasRows) };
                    textureOffsets.push_back(map::TileAtlas::GetTextureOffset(index, atlasRows));

                    // heights
                    const auto& stadtfldBshTextures{ m_world->context->originalResourcesManager->GetStadtfldBshByZoom(t_zoom) };
                    const auto h{ static_cast<float>(stadtfldBshTextures.at(gfx)->height) };
                    texHeightsForRotations.at(instance)[magic_enum::enum_integer(t_rotation)] = h;

                    instance++;
                }
                else // no building
                {
                    texIndicesForRotations.at(instance)[magic_enum::enum_integer(t_rotation)] = -1;

                    // todo: array -> remove else
                    textureOffsets.push_back(glm::vec2(0.0f));

                    texHeightsForRotations.at(instance)[magic_enum::enum_integer(t_rotation)] = -1.0f;

                    instance++;
                }
            }

            texOffsetsForRotations.at(magic_enum::enum_integer(t_rotation)) = textureOffsets;
        });

        textureAtlasIndices.at(zoom) = texIndicesForRotations;
        offsets.at(zoom) = texOffsetsForRotations;
        heights.at(zoom) = texHeightsForRotations;
    });
}
