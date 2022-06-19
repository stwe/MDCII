#pragma once

#include <vector>
#include "ecs/entt.hpp"

namespace mdcii::map
{
    //-------------------------------------------------
    // MapTile
    //-------------------------------------------------

    /**
     * MapTile represents the structure of how map data
     * is read and written from a file.
     * In the ExampleMap below, an array is used for the map data.
     * The entity stores the EnTT entity handle created for this tile.
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

    //-------------------------------------------------
    // ExampleMap
    //-------------------------------------------------

    struct ExampleMap
    {
        static constexpr MapTile CORNER0{ 1051, 0 };
        static constexpr MapTile CORNER1{ 1051, 1 };
        static constexpr MapTile CORNER2{ 1051, 2 };
        static constexpr MapTile CORNER3{ 1051, 3 };

        static constexpr MapTile COAST0{ 1011, 0 };
        static constexpr MapTile COAST1{ 1011, 1 };
        static constexpr MapTile COAST2{ 1011, 2 };
        static constexpr MapTile COAST3{ 1011, 3 };

        static constexpr MapTile GRASS{ 101, 0 };

        static constexpr MapTile FISHING{ 1075, 0 };

        // bakery 2x2 tiles
        static constexpr MapTile BAKERY0{ 503, 0, 2, 2, 0, 0 };
        static constexpr MapTile BAKERY1{ 503, 0, 2, 2, 1, 0 };
        static constexpr MapTile BAKERY2{ 503, 0, 2, 2, 0, 1 };
        static constexpr MapTile BAKERY3{ 503, 0, 2, 2, 1, 1 };

        static constexpr auto EXAMPLE_WIDTH{ 8 };
        static constexpr auto EXAMPLE_HEIGHT{ 12 };

        inline static const std::vector<MapTile> EXAMPLE_ISLAND
        {
            CORNER1,  COAST2,  COAST2, COAST2, COAST2, COAST2,  COAST2,  CORNER2,
            COAST1,   GRASS,   GRASS,  GRASS,  GRASS,  GRASS,   GRASS,   COAST3,
            COAST1,   GRASS,   GRASS,  GRASS,  GRASS,  BAKERY0, BAKERY1, COAST3,
            COAST1,   GRASS,   GRASS,  GRASS,  GRASS,  BAKERY2, BAKERY3, COAST3,
            COAST1,   GRASS,   GRASS,  GRASS,  GRASS,  GRASS,   GRASS,   COAST3,
            COAST1,   GRASS,   GRASS,  GRASS,  GRASS,  GRASS,   GRASS,   COAST3,
            COAST1,   GRASS,   GRASS,  GRASS,  GRASS,  GRASS,   GRASS,   COAST3,
            COAST1,   GRASS,   GRASS,  GRASS,  GRASS,  GRASS,   GRASS,   COAST3,
            COAST1,   GRASS,   GRASS,  GRASS,  GRASS,  GRASS,   GRASS,   COAST3,
            COAST1,   GRASS,   GRASS,  GRASS,  GRASS,  GRASS,   GRASS,   COAST3,
            COAST1,   GRASS,   GRASS,  GRASS,  GRASS,  GRASS,   GRASS,   COAST3,
            CORNER0,  FISHING, COAST0, COAST0, COAST0, COAST0,  COAST0,  CORNER3,
        };
    };
}
