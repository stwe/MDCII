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

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
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
        { "y", t_tile.y },
        { "connected", t_tile.connectedTiles }
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

    if (t_json.count("connected"))
    {
        t_json.at("connected").get_to(t_tile.connectedTiles);
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

const mdcii::world::WorldLayer::Model_Matrices_For_Each_Rotation& mdcii::world::WorldLayer::GetModelMatrices(const map::Zoom t_zoom) const
{
    return modelMatrices.at(magic_enum::enum_integer(t_zoom));
}

//-------------------------------------------------
// Setter
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

glm::mat4 mdcii::world::WorldLayer::GetModelMatrix(const Tile& t_tile, const map::Zoom t_zoom, const map::Rotation t_rotation) const
{
    // to definitely create a position
    int32_t gfx{ GRASS_GFX };
    auto posoffs{ m_world->context->originalResourcesManager->GetBuildingById(GRASS_BUILDING_ID).posoffs };

    // override gfx && posoffs
    if (t_tile.HasBuilding())
    {
        gfx = CalcGfx(t_tile, t_rotation);
        posoffs = m_world->context->originalResourcesManager->GetBuildingById(t_tile.buildingId).posoffs;
    }

    // get width && height
    const auto& stadtfldBshTextures{ m_world->context->originalResourcesManager->GetStadtfldBshByZoom(t_zoom) };
    const auto w{ static_cast<float>(stadtfldBshTextures[gfx]->width) };
    const auto h{ static_cast<float>(stadtfldBshTextures[gfx]->height) };

    // get elevation
    auto elevation{ 0.0f };
    if (posoffs > 0)
    {
        elevation = static_cast<float>(get_elevation(t_zoom));
    }

    // screen position
    auto screenPosition{ t_tile.screenPositions.at(magic_enum::enum_integer(t_zoom)).at(magic_enum::enum_integer(t_rotation)) };
    screenPosition.y -= h - static_cast<float>(get_tile_height(t_zoom));
    screenPosition.y -= elevation;

    // return model matrix
    return renderer::RenderUtils::GetModelMatrix(screenPosition, { w, h });
}

int mdcii::world::WorldLayer::GetTextureAtlasNr(const Tile& t_tile, const map::Zoom t_zoom, const map::Rotation t_rotation) const
{
    const auto atlasRows{ map::TileAtlas::ROWS.at(magic_enum::enum_integer(t_zoom)) };
    return t_tile.HasBuilding() ? CalcGfx(t_tile, t_rotation) / (atlasRows * atlasRows) : -1;
}

glm::vec2 mdcii::world::WorldLayer::GetTextureOffset(const Tile& t_tile, const map::Zoom t_zoom, const map::Rotation t_rotation) const
{
    const auto atlasRows{ map::TileAtlas::ROWS.at(magic_enum::enum_integer(t_zoom)) };
    const auto gfx{ CalcGfx(t_tile, t_rotation) };
    const auto index{ gfx % (atlasRows * atlasRows) };

    return map::TileAtlas::GetTextureOffset(index, atlasRows);
}

float mdcii::world::WorldLayer::GetTextureHeight(const Tile& t_tile, const map::Zoom t_zoom, const map::Rotation t_rotation) const
{
    const auto gfx{ CalcGfx(t_tile, t_rotation) };
    const auto& stadtfldBshTextures{ m_world->context->originalResourcesManager->GetStadtfldBshByZoom(t_zoom) };

    return static_cast<float>(stadtfldBshTextures.at(gfx)->height);
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::world::WorldLayer::SortTiles()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::SortTiles()] Sorting tiles by index.");

    magic_enum::enum_for_each<map::Rotation>([&](const map::Rotation t_rotation) {
        // enum to int
        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

        // sort tiles by index
        std::sort(tiles.begin(), tiles.end(), [&](const Tile& t_a, const Tile& t_b) {
            return t_a.indices[rotationInt] < t_b.indices[rotationInt];
        });

        // copy sorted tiles
        sortedTiles.at(rotationInt) = tiles;
    });

    // revert tiles sorting = sortedTiles DEG0
    tiles = sortedTiles.at(magic_enum::enum_integer(map::Rotation::DEG0));
}

void mdcii::world::WorldLayer::CreateModelMatrices()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::CreateModelMatrices()] Create model matrices.");

    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        Model_Matrices_For_Each_Rotation matricesForRotations;

        // for each rotation in the zoom create the model matrices
        magic_enum::enum_for_each<map::Rotation>([&](const map::Rotation t_rotation) {
            const auto rotationInt{ magic_enum::enum_integer(t_rotation) };
            Model_Matrices matrices;
            int32_t instance{ 0 };
            for (auto& tile : sortedTiles.at(rotationInt))
            {
                matrices.emplace_back(GetModelMatrix(tile, t_zoom, t_rotation));
                tile.instanceIds.at(rotationInt) = instance;

                instance++;
            }

            matricesForRotations.at(rotationInt) = matrices;
        });

        modelMatrices.at(magic_enum::enum_integer(t_zoom)) = matricesForRotations;
    });

    // store instances Ids
    magic_enum::enum_for_each<map::Rotation>([&](const map::Rotation t_rotation) {
        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };
        for (auto& tile : sortedTiles.at(rotationInt))
        {
            instanceIds.emplace(glm::ivec3(tile.worldX, tile.worldY, rotationInt), tile.instanceIds.at(rotationInt));
        }
    });
}

void mdcii::world::WorldLayer::CreateTextureInfo()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::CreateTextureInfo()] Create texture info.");

    MDCII_ASSERT(instances >= 0, "[WorldLayer::CreateTextureInfo()] Invalid number of instances.")

    // for each zoom
    magic_enum::enum_for_each<map::Zoom>([&](const map::Zoom t_zoom) {
        Texture_Atlas_Indices texIndicesForRotations(instances, glm::ivec4(-1));
        Texture_Offsets_For_Each_Rotation texOffsetsForRotations;
        Texture_Heights texHeightsForRotations(instances, glm::vec4(-1.0f));

        // for each rotation in the zoom
        magic_enum::enum_for_each<map::Rotation>([&](const map::Rotation t_rotation) {
            const auto rotation{ magic_enum::enum_integer(t_rotation) };
            Texture_Offsets textureOffsets(instances, glm::vec2(0.0f));

            auto instance{ 0 };
            for (const auto& tile : sortedTiles.at(rotation))
            {
                if (tile.HasBuilding())
                {
                    texIndicesForRotations.at(instance)[rotation] = GetTextureAtlasNr(tile, t_zoom, t_rotation);
                    textureOffsets.at(instance) = GetTextureOffset(tile, t_zoom, t_rotation);
                    texHeightsForRotations.at(instance)[rotation] = GetTextureHeight(tile, t_zoom, t_rotation);
                }

                instance++;
            }

            texOffsetsForRotations.at(rotation) = textureOffsets;
        });

        const auto zoom{ magic_enum::enum_integer(t_zoom) };
        textureAtlasIndices.at(zoom) = texIndicesForRotations;
        offsets.at(zoom) = texOffsetsForRotations;
        heights.at(zoom) = texHeightsForRotations;
    });
}

int32_t mdcii::world::WorldLayer::CalcGfx(const Tile& t_tile, const map::Rotation t_rotation) const
{
    const auto& building{ m_world->context->originalResourcesManager->GetBuildingById(t_tile.buildingId) };
    auto buildingRotation{ t_tile.rotation };
    if (building.rotate > 0)
    {
        buildingRotation = buildingRotation + t_rotation;
    }
    auto gfx{ t_tile.gfxs[magic_enum::enum_integer(buildingRotation)] };

    if (building.size.w > 1)
    {
        // default: orientation 0
        auto rp{ glm::ivec2(t_tile.x, t_tile.y) };

        if (t_tile.rotation == map::Rotation::DEG270)
        {
            rp = rotate_position(
                t_tile.x, t_tile.y,
                building.size.w, building.size.h,
                map::Rotation::DEG90
            );
        }

        if (t_tile.rotation == map::Rotation::DEG180)
        {
            rp = rotate_position(
                t_tile.x, t_tile.y,
                building.size.w, building.size.h,
                map::Rotation::DEG180
            );
        }

        if (t_tile.rotation == map::Rotation::DEG90)
        {
            rp = rotate_position(
                t_tile.x, t_tile.y,
                building.size.w, building.size.h,
                map::Rotation::DEG270
            );
        }

        const auto offset{ rp.y * building.size.w + rp.x };
        gfx += offset;
    }

    return gfx;
}
