#pragma once

#include <glm/vec2.hpp>
#include "data/Buildings.h"
#include "map/MapTile.h"

//-------------------------------------------------
// Components
//-------------------------------------------------

namespace mdcii::ecs
{
    struct GridComponent
    {
        int mapX{ 0 };
        int mapY{ 0 };

        int tileWidth{ 0 };
        int tileHeight{ 0 };

        uint32_t textureId{ 0 };

        std::vector<glm::vec2> screenPositions;
    };

    struct BuildingComponent
    {
        map::MapTile mapTile;
        data::Building building;
    };

    struct PositionComponent
    {
        int mapX{ 0 };
        int mapY{ 0 };

        std::vector<int> indices;
        std::vector<glm::vec2> screenPositions;
        std::vector<int> gfx;
    };

    struct SelectedComponent
    {
        int index{ -1 };
    };
}
