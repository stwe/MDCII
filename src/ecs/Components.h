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

    struct TerrainLayerTileComponent
    {
        map::MapTile mapTile;
        data::Building building;
    };

    struct BuildingsLayerTileComponent
    {
        map::MapTile mapTile;
        data::Building building;
    };

    struct BuildingUpdatedComponent
    {
        bool writtenToLayer{ false };
    };
}
