#pragma once

#include "data/Buildings.h"
#include "map/MapTile.h"

//-------------------------------------------------
// Components
//-------------------------------------------------

namespace mdcii::ecs
{
    struct GridComponent
    {
        map::MapTile mapTile;
        uint32_t textureId{ 0 };
    };

    struct TileComponent
    {
        map::MapTile mapTile;
        data::Building building;
    };

    struct TerrainLayerComponent
    {
        int i{ 0 };
    };

    struct BuildingsLayerComponent
    {
        int i{ 0 };
    };
}
