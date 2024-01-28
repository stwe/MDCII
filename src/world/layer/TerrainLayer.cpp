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

#include "TerrainLayer.h"
#include "MdciiAssert.h"
#include "Game.h"
#include "world/World.h"
#include "state/State.h"
#include "resource/OriginalResourcesManager.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::layer::TerrainLayer::TerrainLayer(const World* t_world, const LayerType t_layerType, const int t_width, const int t_height)
    : Layer(t_world, t_layerType, t_width, t_height)
{
    MDCII_LOG_DEBUG("[TerrainLayer::TerrainLayer()] Create TerrainLayer of type {}.", magic_enum::enum_name(layerType));
}

mdcii::world::layer::TerrainLayer::~TerrainLayer() noexcept
{
    MDCII_LOG_DEBUG("[TerrainLayer::~TerrainLayer()] Destruct TerrainLayer.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

mdcii::world::tile::TerrainTile& mdcii::world::layer::TerrainLayer::GetSortedTile(const int t_x, const int t_y, const Rotation t_rotation)
{
    const auto rotationInt{ magic_enum::enum_integer(t_rotation) };
    const auto idx{ sortedIndices.at(rotationInt).at(tile::Tile::GetRenderIndex(t_x, t_y, width, height, t_rotation)) };

    return sortedTiles.at(rotationInt).at(idx);
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::world::layer::TerrainLayer::CreateLayerTiles(const nlohmann::json& t_json)
{
    MDCII_LOG_DEBUG("[TerrainLayer::CreateLayerTiles()] Create tiles for layer of type {}.", magic_enum::enum_name(layerType));

    for (const auto& [k, tileJson] : t_json.items())
    {
        tiles.emplace_back(CreateLayerTile(tileJson));
    }

    MDCII_ASSERT(tiles.size() == width * static_cast<size_t>(height), "[TerrainLayer::CreateLayerTiles()] Invalid number of tiles.")
}

void mdcii::world::layer::TerrainLayer::InitLayerTiles()
{
    MDCII_LOG_DEBUG("[TerrainLayer::InitLayerTiles()] Start initializing layer tiles for {} ...", magic_enum::enum_name(layerType));

    MDCII_ASSERT(!tiles.empty(), "[TerrainLayer::InitLayerTiles()] Invalid number of tiles.")

    for (auto h{ 0 }; h < height; ++h)
    {
        for (auto w{ 0 }; w < width; ++w)
        {
            InitLayerTile(w, h);
        }
    }

    SortTilesForRendering();

    MDCII_LOG_DEBUG("[TerrainLayer::InitLayerTiles()] The layer tiles were initialized successfully.");
}

//-------------------------------------------------
// Create / Init Tile
//-------------------------------------------------

mdcii::world::tile::TerrainTile mdcii::world::layer::TerrainLayer::CreateLayerTile(const nlohmann::json& t_json)
{
    // todo: create from_json -> TerrainTile

    tile::TerrainTile tile;

    if (t_json.count("id") && t_json.at("id") >= 0)
    {
        if (m_world)
        {
            const auto& building{ m_world->state->game->originalResourcesManager->GetBuildingById(t_json.at("id")) };
            tile.building = &building;
        }
        else
        {
            throw MDCII_EXCEPTION("[TerrainLayer::CreateLayerTile()] Null pointer.");
        }
    }

    if (t_json.count("rotation"))
    {
        tile.rotation = t_json.at("rotation");
        if (tile.HasBuilding() && tile.building->IsRotatable())
        {
            if (tile.rotation < 0 || tile.rotation > 3)
            {
                throw MDCII_EXCEPTION("[TerrainLayer::CreateLayerTile()] Invalid building rotation.");
            }
        }
    }

    if (t_json.count("x"))
    {
        tile.x = t_json.at("x");
    }

    if (t_json.count("y"))
    {
        tile.y = t_json.at("y");
    }

    return tile;
}

void mdcii::world::layer::TerrainLayer::InitLayerTile(const int t_x, const int t_y)
{
    auto& tile{ tiles.at(t_y * width + t_x) };
    tile.posX = t_x;
    tile.posY = t_y;

    if (tile.HasBuilding())
    {
        tile.CalculateGfx();
    }

    tile.SetRenderIndices(width, height);
}

//-------------------------------------------------
// Add Tiles
//-------------------------------------------------

void mdcii::world::layer::TerrainLayer::AddBuilding(std::vector<tile::TerrainTile>& t_buildingTiles)
{
    MDCII_ASSERT(!t_buildingTiles.empty(), "[TerrainLayer::AddBuilding()] Invalid number of tiles.")

    MDCII_LOG_DEBUG("[TerrainLayer::AddBuilding()] Add {} tiles for a building to the {} layer.", t_buildingTiles.size(), magic_enum::enum_name(layerType));

    for (auto& tile : t_buildingTiles)
    {
        MDCII_ASSERT(tile.HasBuildingAboveWaterAndCoast(), "[TerrainLayer::AddBuilding()] Null pointer or invalid building.")

        tile.SetRenderIndices(width, height);

        // todo: refactor

        tiles.at(tile.renderIndices[0]) = tile;

        auto idx0{ sortedIndices.at(magic_enum::enum_integer(Rotation::DEG0)).at(tile.renderIndices[0]) };
        auto idx90{ sortedIndices.at(magic_enum::enum_integer(Rotation::DEG90)).at(tile.renderIndices[1]) };
        auto idx180{ sortedIndices.at(magic_enum::enum_integer(Rotation::DEG180)).at(tile.renderIndices[2]) };
        auto idx270{ sortedIndices.at(magic_enum::enum_integer(Rotation::DEG270)).at(tile.renderIndices[3]) };

        sortedTiles.at(magic_enum::enum_integer(Rotation::DEG0)).at(idx0) = tile;
        sortedTiles.at(magic_enum::enum_integer(Rotation::DEG90)).at(idx90) = tile;
        sortedTiles.at(magic_enum::enum_integer(Rotation::DEG180)).at(idx180) = tile;
        sortedTiles.at(magic_enum::enum_integer(Rotation::DEG270)).at(idx270) = tile;
    }
}

//-------------------------------------------------
// Sort
//-------------------------------------------------

void mdcii::world::layer::TerrainLayer::SortTilesForRendering()
{
    MDCII_LOG_DEBUG("[TerrainLayer::SortTilesForRendering()] Sort tiles for rendering.");

    for (const auto rotation : magic_enum::enum_values<Rotation>())
    {
        const auto rotationInt{ magic_enum::enum_integer(rotation) };

        // sort tiles by index
        std::ranges::sort(tiles, [&](const tile::TerrainTile& t_a, const tile::TerrainTile& t_b)
        {
            return t_a.renderIndices[rotationInt] < t_b.renderIndices[rotationInt];
        });

        // copy sorted tiles
        sortedTiles.at(rotationInt) = tiles;

        // the render-index can be negative; saves where the new index (i) can be found in sortedTiles
        auto i{ 0 };
        for (const auto& t : sortedTiles.at(rotationInt))
        {
            sortedIndices.at(rotationInt).emplace(t.renderIndices.at(rotationInt), i);
            i++;
        }
    }

    // revert tiles sorting = sortedTiles DEG0
    tiles = sortedTiles.at(magic_enum::enum_integer(Rotation::DEG0));
}
