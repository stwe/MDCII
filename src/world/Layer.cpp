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

#include "Layer.h"
#include "Rotation.h"
#include "World.h"
#include "MdciiAssert.h"
#include "Game.h"
#include "state/State.h"
#include "resource/OriginalResourcesManager.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::Layer::Layer(const World* t_world, LayerType t_layerType, const int t_width, const int t_height)
    : layerType{ t_layerType }
    , m_world{ t_world }
    , m_width{ t_width }
    , m_height{ t_height }
{
    MDCII_LOG_DEBUG("[Layer::Layer()] Create Layer of type {}.", magic_enum::enum_name(layerType));

    MDCII_ASSERT(m_world, "[Layer::Layer()] Null pointer.")
}

mdcii::world::Layer::~Layer() noexcept
{
    MDCII_LOG_DEBUG("[Layer::~Layer()] Destruct Layer.");
}

//-------------------------------------------------
// Getter
//-------------------------------------------------

mdcii::world::Tile& mdcii::world::Layer::GetSortedTile(const int t_x, const int t_y, const Rotation t_rotation)
{
    const auto rotationInt{ magic_enum::enum_integer(t_rotation) };
    const auto idx{ sortedIndices.at(rotationInt).at(GetMapIndex(t_x, t_y, t_rotation )) };

    return sortedTiles.at(rotationInt).at(idx);
}

//-------------------------------------------------
// Create Tiles
//-------------------------------------------------

void mdcii::world::Layer::CreateLayerTiles(const nlohmann::json& t_json)
{
    MDCII_LOG_DEBUG("[Layer::CreateLayerTiles()] Create tiles for layer of type {}.", magic_enum::enum_name(layerType));

    for (const auto& [k, tileJson] : t_json.items())
    {
        tiles.emplace_back(CreateTile(tileJson));
    }

    MDCII_ASSERT(tiles.size() == m_width * static_cast<size_t>(m_height), "[Layer::CreateLayerTiles()] Invalid number of tiles.")
}

mdcii::world::Tile mdcii::world::Layer::CreateTile(const nlohmann::json& t_json)
{
    Tile tile;

    if (t_json.count("id") && t_json.at("id") >= 0)
    {
        if (m_world)
        {
            const auto& building{ m_world->state->game->originalResourcesManager->GetBuildingById(t_json.at("id")) };
            tile.building = &building;
        }
        else
        {
            throw MDCII_EXCEPTION("[Layer::CreateTile()] Null pointer.");
        }
    }

    if (t_json.count("rotation"))
    {
        tile.rotation = t_json.at("rotation");
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

void mdcii::world::Layer::InitLayerTiles()
{
    MDCII_LOG_DEBUG("[Layer::InitLayerTiles()] Start initializing layer tiles for {} ...", magic_enum::enum_name(layerType));

    MDCII_ASSERT(!tiles.empty(), "[Layer::InitLayerTiles()] Invalid number of tiles.")

    for (auto h{ 0 }; h < m_height; ++h)
    {
        for (auto w{ 0 }; w < m_width; ++w)
        {
            InitTileDetails(w, h);
        }
    }

    SortTilesForRendering();

    MDCII_LOG_DEBUG("[Layer::InitLayerTiles()] The layer tiles were initialized successfully.");
}

void mdcii::world::Layer::InitTileDetails(const int t_x, const int t_y)
{
    auto& tile{ tiles.at(t_y * m_width + t_x) };
    tile.posX = t_x;
    tile.posY = t_y;

    if (tile.HasBuilding())
    {
        tile.CalculateGfx();
    }

    tile.renderIndices[0] = GetMapIndex(tile.posX, tile.posY, Rotation::DEG0);
    tile.renderIndices[1] = GetMapIndex(tile.posX, tile.posY, Rotation::DEG90);
    tile.renderIndices[2] = GetMapIndex(tile.posX, tile.posY, Rotation::DEG180);
    tile.renderIndices[3] = GetMapIndex(tile.posX, tile.posY, Rotation::DEG270);
}

//-------------------------------------------------
// Add
//-------------------------------------------------

void mdcii::world::Layer::AddBuilding(std::vector<Tile>& t_buildingTiles)
{
    MDCII_ASSERT(!t_buildingTiles.empty(), "[Layer::AddBuilding()] Invalid number of tiles.")

    MDCII_LOG_DEBUG("[Layer::AddBuilding()] Add {} tiles for a building to the {} layer.", t_buildingTiles.size(), magic_enum::enum_name(layerType));

    for (auto& tile : t_buildingTiles)
    {
        MDCII_ASSERT(tile.HasBuildingAboveWaterAndCoast(), "[Layer::AddBuilding()] Null pointer or invalid building.")

        tile.renderIndices[0] = GetMapIndex(tile.posX, tile.posY, Rotation::DEG0);
        tile.renderIndices[1] = GetMapIndex(tile.posX, tile.posY, Rotation::DEG90);
        tile.renderIndices[2] = GetMapIndex(tile.posX, tile.posY, Rotation::DEG180);
        tile.renderIndices[3] = GetMapIndex(tile.posX, tile.posY, Rotation::DEG270);

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

void mdcii::world::Layer::SortTilesForRendering()
{
    MDCII_LOG_DEBUG("[Layer::SortTilesForRendering()] Sort tiles for rendering.");

    for (const auto rotation : magic_enum::enum_values<Rotation>())
    {
        const auto rotationInt{ magic_enum::enum_integer(rotation) };

        // sort tiles by index
        std::ranges::sort(tiles, [&](const Tile& t_a, const Tile& t_b)
        {
            return t_a.renderIndices[rotationInt] < t_b.renderIndices[rotationInt];
        });

        // copy sorted tiles
        sortedTiles.at(rotationInt) = tiles;

        // the render-index can be negative; saves where the index can be found in sortedTiles
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

//-------------------------------------------------
// Helper
//-------------------------------------------------

int mdcii::world::Layer::GetMapIndex(
    const int t_x, const int t_y,
    const int t_width, const int t_height,
    const Rotation t_rotation
)
{
    MDCII_ASSERT(t_x >= 0 && t_x < t_width, "[Layer::GetMapIndex()] Invalid x position given.")
    MDCII_ASSERT(t_y >= 0 && t_y < t_height, "[Layer::GetMapIndex()] Invalid y position given.")

    const auto position{ rotate_position(t_x, t_y, t_width, t_height, t_rotation) };

    if (t_rotation == world::Rotation::DEG0 || t_rotation == world::Rotation::DEG180)
    {
        return position.y * t_width + position.x;
    }

    return position.y * t_height + position.x;
}

int mdcii::world::Layer::GetMapIndex(const int t_x, const int t_y, const Rotation t_rotation) const
{
    return GetMapIndex(t_x, t_y, m_width, m_height, t_rotation);
}
