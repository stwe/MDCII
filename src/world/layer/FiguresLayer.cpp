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

#include "FiguresLayer.h"
#include "MdciiAssert.h"
#include "Game.h"
#include "MdciiException.h"
#include "world/World.h"
#include "state/State.h"
#include "resource/OriginalResourcesManager.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::layer::FiguresLayer::FiguresLayer(const World* t_world, const int t_width, const int t_height)
    : Layer(t_world, LayerType::FIGURES, t_width, t_height)
{
    MDCII_LOG_DEBUG("[FiguresLayer::FiguresLayer()] Create FiguresLayer.");
}

mdcii::world::layer::FiguresLayer::~FiguresLayer() noexcept
{
    MDCII_LOG_DEBUG("[FiguresLayer::~FiguresLayer()] Destruct FiguresLayer.");
}

//-------------------------------------------------
// Override
//-------------------------------------------------

void mdcii::world::layer::FiguresLayer::CreateLayerTiles(const nlohmann::json& t_json)
{
    MDCII_LOG_DEBUG("[FiguresLayer::CreateLayerTiles()] Create figures layer tiles.");

    for (const auto& [k, tileJson] : t_json.items())
    {
        tiles.emplace_back(CreateLayerTile(tileJson));
    }

    MDCII_ASSERT(tiles.size() == width * static_cast<size_t>(height), "[FiguresLayer::CreateLayerTiles()] Invalid number of tiles.")
}

void mdcii::world::layer::FiguresLayer::InitLayerTiles()
{
    MDCII_LOG_DEBUG("[FiguresLayer::InitLayerTiles()] Start initializing figures layer tiles ...");

    MDCII_ASSERT(!tiles.empty(), "[FiguresLayer::InitLayerTiles()] Invalid number of tiles.")

    for (auto h{ 0 }; h < height; ++h)
    {
        for (auto w{ 0 }; w < width; ++w)
        {
            auto& tile{ tiles.at(h * width + w) };
            tile.posX = w;
            tile.posY = h;

            tile.SetRenderIndices(width, height);
        }
    }

    // todo
    //SortTilesForRendering();

    MDCII_LOG_DEBUG("[FiguresLayer::InitLayerTiles()] The figures layer tiles were initialized successfully.");
}

//-------------------------------------------------
// Create Tile
//-------------------------------------------------

mdcii::world::tile::FigureTile mdcii::world::layer::FiguresLayer::CreateLayerTile(const nlohmann::json& t_json)
{
    tile::FigureTile tile;

    if (t_json.count("id") && t_json.at("id") > 0) // 0 = UNSET
    {
        if (m_world)
        {
            const auto& figure{ m_world->state->game->originalResourcesManager->GetFigureById(t_json.at("id")) };
            tile.figure = &figure;
        }
        else
        {
            throw MDCII_EXCEPTION("[FiguresLayer::CreateLayerTile()] Null pointer.");
        }
    }

    if (t_json.count("rotation"))
    {
        tile.rotation = t_json.at("rotation");
    }

    return tile;
}
