#pragma once

#include "ecs/entt.hpp"

namespace mdcii::map
{
    //-------------------------------------------------
    // MapTile
    //-------------------------------------------------

    /**
     * MapTile represents a single part of the map.
     */
    struct MapTile
    {
        /**
         * The building Id from the haeuser.cod.
         */
        int32_t buildingId{ -1 };

        /**
         * The orientation of the building.
         */
        int32_t orientation{ 0 };

        /**
         * Example: Bakery
         *     width = 2
         *     height = 2
         *
         *     object space:
         *     -------------------------------
         *     | x = 0, y = 0 | x = 1, y = 0 |
         *     -------------------------------
         *     | x = 0, y = 1 | x = 1, y = 1 |
         *     -------------------------------
         */

        /**
         * Specifies the size of the building in x direction in tiles.
         */
        int32_t width{ 1 };

        /**
         * Specifies the size of the building in y direction in tiles.
         */
        int32_t height{ 1 };

        /**
         * The x position of the tile in object space.
         */
        int32_t x{ 0 };

        /**
         * The y position of the tile in object space.
         */
        int32_t y{ 0 };

        /**
         * Stores the entity handle created for this tile.
         */
        entt::entity entity{ entt::null };

        /**
         * Stores the entity handle created for this tile
         * to show a grid for debugging purposes.
         */
        entt::entity gridEntity{ entt::null };
    };
}
