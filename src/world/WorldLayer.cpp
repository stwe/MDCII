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
#include "WorldLayer.h"
#include "World.h"
#include "TileAtlas.h"
#include "MdciiAssert.h"
#include "state/State.h"
#include "file/OriginalResourcesManager.h"
#include "renderer/RenderUtils.h"

//-------------------------------------------------
// Json
//-------------------------------------------------

void mdcii::world::to_json(nlohmann::json& t_json, const std::shared_ptr<Tile>& t_tile)
{
    if (t_tile)
    {
        t_json = nlohmann::json{
            { "id", t_tile->buildingId },
            { "rotation", magic_enum::enum_integer(t_tile->rotation) },
            { "x", t_tile->x },
            { "y", t_tile->y },
            { "connected", t_tile->connectedTiles }
        };
    }
    else
    {
        t_json = nullptr;
    }
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
        t_tile.rotation = int_to_rotation(r);
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

const mdcii::world::WorldLayer::Model_Matrices_For_Each_Rotation& mdcii::world::WorldLayer::GetModelMatrices(const Zoom t_zoom) const
{
    return modelMatrices.at(magic_enum::enum_integer(t_zoom));
}

const mdcii::world::Tile& mdcii::world::WorldLayer::GetTile(const int t_x, const int t_y) const
{
    return *tiles.at(m_world->GetMapIndex(t_x, t_y));
}

bool mdcii::world::WorldLayer::IsAlreadyBuildingOnPosition(const int t_x, const int t_y, const data::Building& t_building, const Rotation t_buildingRotation) const
{
    for (auto y{ 0 }; y < t_building.size.h; ++y)
    {
        for (auto x{ 0 }; x < t_building.size.w; ++x)
        {
            auto rp{ world::rotate_position(x, y, t_building.size.h, t_building.size.w, t_buildingRotation) };
            if (t_buildingRotation == world::Rotation::DEG0 || t_buildingRotation == world::Rotation::DEG180)
            {
                rp = world::rotate_position(x, y, t_building.size.w, t_building.size.h, t_buildingRotation);
            }

            if (GetTile(t_x + rp.x, t_y + rp.y).HasBuilding())
            {
                return true;
            }
        }
    }

    return false;
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
// Tiles
//-------------------------------------------------

void mdcii::world::WorldLayer::AddTileFromJson(const nlohmann::json& t_json)
{
    tiles.emplace_back(std::make_unique<Tile>(t_json.get<Tile>()));
}

void mdcii::world::WorldLayer::ResetTilePointersAt(const std::array<int32_t, NR_OF_ROTATIONS>& t_instanceIds)
{
    const auto id0{ t_instanceIds.at(magic_enum::enum_integer(Rotation::DEG0)) };
    MDCII_ASSERT(tiles.at(id0), "[WorldLayer::ResetTilePointers()] Null pointer.")
    tiles.at(id0).reset();

    magic_enum::enum_for_each<Rotation>([this, &t_instanceIds](const Rotation t_rotation) {
        const auto r{ magic_enum::enum_integer(t_rotation) };
        MDCII_ASSERT(sortedTiles.at(r).at(t_instanceIds.at(r)), "[WorldLayer::ResetTilePointers()] Null pointer.")
        sortedTiles.at(r).at(t_instanceIds.at(r)).reset();
    });
}

void mdcii::world::WorldLayer::StoreTile(std::unique_ptr<Tile> t_tile)
{
    const auto id0{ t_tile->instanceIds.at(magic_enum::enum_integer(Rotation::DEG0)) };
    MDCII_ASSERT(!tiles.at(id0), "[WorldLayer::ReplaceTileWith()] Invalid pointer.")
    tiles.at(id0) = std::move(t_tile);

    magic_enum::enum_for_each<Rotation>([this, &id0](const Rotation t_rotation) {
        const auto r{ magic_enum::enum_integer(t_rotation) };
        MDCII_ASSERT(!sortedTiles.at(r).at(tiles.at(id0)->instanceIds.at(r)), "[WorldLayer::ReplaceTileWith()] Invalid pointer.")
        sortedTiles.at(r).at(tiles.at(id0)->instanceIds.at(r)) = tiles.at(id0);
    });
}

//-------------------------------------------------
// Helper
//-------------------------------------------------

void mdcii::world::WorldLayer::PrepareRendering()
{
    MDCII_ASSERT(instancesToRender >= 0, "[WorldLayer::PrepareRendering()] Invalid number of instances to render.")

    SortTiles();

    CreateModelMatrices();
    CreateHeightInfo();
    CreateGfxInfo();
    CreateBuildingInfo();
    CreateAnimationInfo();
}

glm::mat4 mdcii::world::WorldLayer::GetModelMatrix(const Tile& t_tile, const Zoom t_zoom, const Rotation t_rotation) const
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

int32_t mdcii::world::WorldLayer::CalcGfx(const Tile& t_tile, const Rotation t_rotation) const
{
    const auto& building{ m_world->context->originalResourcesManager->GetBuildingById(t_tile.buildingId) };
    auto buildingRotation{ t_tile.rotation };
    if (building.rotate > 0)
    {
        buildingRotation = buildingRotation + t_rotation;
    }
    auto gfx{ t_tile.gfxs[magic_enum::enum_integer(buildingRotation)] };

    if (building.size.w > 1 || building.size.h > 1)
    {
        // default: orientation 0
        auto rp{ glm::ivec2(t_tile.x, t_tile.y) };

        if (t_tile.rotation == Rotation::DEG270)
        {
            rp = rotate_position(
                t_tile.x, t_tile.y,
                building.size.w, building.size.h,
                Rotation::DEG90
            );
        }

        if (t_tile.rotation == Rotation::DEG180)
        {
            rp = rotate_position(
                t_tile.x, t_tile.y,
                building.size.w, building.size.h,
                Rotation::DEG180
            );
        }

        if (t_tile.rotation == Rotation::DEG90)
        {
            rp = rotate_position(
                t_tile.x, t_tile.y,
                building.size.w, building.size.h,
                Rotation::DEG270
            );
        }

        const auto offset{ rp.y * building.size.w + rp.x };
        gfx += offset;
    }

    return gfx;
}

void mdcii::world::WorldLayer::SortTiles()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::SortTiles()] Sorting tiles by index for Layer type {}.", magic_enum::enum_name(layerType));

    magic_enum::enum_for_each<Rotation>([this](const Rotation t_rotation) {
        // enum to int
        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

        // sort tiles by index
        std::sort(tiles.begin(), tiles.end(), [&](const std::shared_ptr<Tile>& t_a, const std::shared_ptr<Tile>& t_b) {
            return t_a->indices[rotationInt] < t_b->indices[rotationInt];
        });

        // copy sorted tiles
        sortedTiles.at(rotationInt) = tiles;
    });

    // revert tiles sorting = sortedTiles DEG0
    tiles = sortedTiles.at(magic_enum::enum_integer(Rotation::DEG0));
}

//-------------------------------------------------
// Create Gpu data
//-------------------------------------------------

void mdcii::world::WorldLayer::CreateModelMatrices()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::CreateModelMatrices()] Create model matrices for Layer type {}.", magic_enum::enum_name(layerType));

    magic_enum::enum_for_each<Zoom>([this](const Zoom t_zoom) {
        Model_Matrices_For_Each_Rotation matricesForRotations;

        magic_enum::enum_for_each<Rotation>([this, &t_zoom, &matricesForRotations](const Rotation t_rotation) {
            const auto rotationInt{ magic_enum::enum_integer(t_rotation) };
            Model_Matrices matrices;
            int32_t instance{ 0 };
            for (const auto& tile : sortedTiles.at(rotationInt))
            {
                matrices.emplace_back(GetModelMatrix(*tile, t_zoom, t_rotation));
                tile->instanceIds.at(rotationInt) = instance;

                instance++;
            }

            matricesForRotations.at(rotationInt) = matrices;
        });

        modelMatrices.at(magic_enum::enum_integer(t_zoom)) = matricesForRotations;
    });

    // create a hashmap to find the instance ID for each rotated position
    magic_enum::enum_for_each<Rotation>([this](const Rotation t_rotation) {
        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };
        for (const auto& tile : sortedTiles.at(rotationInt))
        {
            // the position at Deg0 is at a different instance depending on the rotation
            instanceIds.emplace(glm::ivec3(tile->worldXDeg0, tile->worldYDeg0, rotationInt), tile->instanceIds.at(rotationInt));
        }
    });
}

void mdcii::world::WorldLayer::CreateHeightInfo()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::CreateHeightInfo()] Create height info for Layer type {}.", magic_enum::enum_name(layerType));

    magic_enum::enum_for_each<Zoom>([this](const Zoom t_zoom) {
        const auto& stadtfldBshTextures{ m_world->context->originalResourcesManager->GetStadtfldBshByZoom(t_zoom) };

        Texture_Heights textureHeights(stadtfldBshTextures.size(), 0);
        auto i{ 0 };
        for (const auto& bshTexture : stadtfldBshTextures)
        {
            textureHeights.at(i) = static_cast<float>(bshTexture->height);
            i++;
        }

        heights.at(magic_enum::enum_integer(t_zoom)) = textureHeights;
    });
}

void mdcii::world::WorldLayer::CreateGfxInfo()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::CreateGfxInfo()] Create gfx info for Layer type {}.", magic_enum::enum_name(layerType));

    Gfx_Info info(instancesToRender, glm::ivec4(-1));

    magic_enum::enum_for_each<Rotation>([this, &info](const Rotation t_rotation) {
        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

        auto instance{ 0 };
        for (const auto& tile : sortedTiles.at(rotationInt))
        {
            if (tile->HasBuilding())
            {
                info.at(instance)[rotationInt] = CalcGfx(*tile, t_rotation);
            }

            instance++;
        }
    });

    gfxInfo = info;
}

void mdcii::world::WorldLayer::CreateBuildingInfo()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::CreateBuildingInfo()] Create building info for Layer type {}.", magic_enum::enum_name(layerType));

    Building_Info info(instancesToRender, glm::ivec4(-1));

    magic_enum::enum_for_each<Rotation>([this, &info](const Rotation t_rotation) {
        const auto rotationInt{ magic_enum::enum_integer(t_rotation) };

        auto instance{ 0 };
        for (const auto& tile : sortedTiles.at(rotationInt))
        {
            if (tile->HasBuilding())
            {
                info.at(instance)[rotationInt] = tile->buildingId;
            }

            instance++;
        }
    });

    buildingInfo = info;
}

void mdcii::world::WorldLayer::CreateAnimationInfo()
{
    Log::MDCII_LOG_DEBUG("[WorldLayer::CreateAnimationInfo()] Create animation info for Layer type {}.", magic_enum::enum_name(layerType));

    const auto& buildings{ m_world->context->originalResourcesManager->buildings->buildingsMap };

    Animation_Info info(buildings.rbegin()->first + 1, glm::ivec4(-1));

    for (const auto& [k, v] : buildings)
    {
        info.at(k) = glm::ivec4(v.animAnz, v.animTime, v.animFrame, v.animAdd);
    }

    animationInfo = info;
}
