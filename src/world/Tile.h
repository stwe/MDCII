#pragma once

#include <cstdint>

namespace mdcii::world
{
    class Tile
    {
    public:
        int32_t buildingId{ -1 };
        int32_t gfx{ -1 };
        int32_t rotation{ 0 };
        int32_t x{ 0 };
        int32_t y{ 0 };

    protected:

    private:

    };
}
