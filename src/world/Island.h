#pragma once

#include <vector>
#include "Tile.h"

namespace mdcii::world
{
    class Island
    {
    public:
        int32_t width{ -1 };
        int32_t height{ -1 };
        int32_t x{ -1 };
        int32_t y{ -1 };

        std::vector<Tile> tiles;

    protected:

    private:

    };
}
