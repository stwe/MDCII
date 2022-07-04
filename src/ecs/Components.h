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
        int32_t mapX{ 0 };
        int32_t mapY{ 0 };

        int32_t tileWidth{ 0 };
        int32_t tileHeight{ 0 };

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
        int32_t mapX{ 0 };
        int32_t mapY{ 0 };

        std::vector<int32_t> indices;
        std::vector<glm::vec2> screenPositions;
        std::vector<int32_t> gfx;
    };

    struct SelectedComponent
    {
        int32_t index{ -1 };
    };

    struct MouseCursorComponent
    {
        std::vector<int> gfx;
        data::Building building;
    };
}
