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

#include <algorithm>
#include "Layer.h"
#include "Log.h"
#include "Rotation.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::world::Layer::Layer(LayerType t_layerType)
    : layerType{ t_layerType }
{
    MDCII_LOG_DEBUG("[Layer::Layer()] Create Layer of type {}.", magic_enum::enum_name(layerType));
}

mdcii::world::Layer::~Layer() noexcept
{
    MDCII_LOG_DEBUG("[Layer::~Layer()] Destruct Layer.");
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
            return t_a.indices[rotationInt] < t_b.indices[rotationInt];
        });

        // copy sorted tiles
        sortedTiles.at(rotationInt) = tiles;
    }

    // revert tiles sorting = sortedTiles DEG0
    tiles = sortedTiles.at(magic_enum::enum_integer(Rotation::DEG0));
}
