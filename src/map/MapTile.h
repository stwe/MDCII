#pragma once

#include <vector>

namespace mdcii::map
{
    //-------------------------------------------------
    // MapTile
    //-------------------------------------------------

    /**
     * Represents a building with a specific orientation.
     */
    struct MapTile
    {
        /**
         * The building Id from the haeuser.cod.
         */
        int buildingId{ -1 };

        /**
         * The orientation of the building.
         */
        int orientation{ 0 };

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
        int width{ 1 };

        /**
         * Specifies the size of the building in y direction in tiles.
         */
        int height{ 1 };

        /**
         * The x position of the tile in object space.
         */
        int x{ 0 };

        /**
         * The y position of the tile in object space.
         */
        int y{ 0 };
    };

    //-------------------------------------------------
    // Example
    //-------------------------------------------------

    static constexpr MapTile corner0{ 1051, 0 };
    static constexpr MapTile corner1{ 1051, 1 };
    static constexpr MapTile corner2{ 1051, 2 };
    static constexpr MapTile corner3{ 1051, 3 };

    static constexpr MapTile coast0{ 1011, 0 };
    static constexpr MapTile coast1{ 1011, 1 };
    static constexpr MapTile coast2{ 1011, 2 };
    static constexpr MapTile coast3{ 1011, 3 };

    static constexpr MapTile grass{ 101, 0 };

    static constexpr MapTile fishing{ 1075, 0 };

    // bakery 2x2 tiles
    static constexpr MapTile bakery0{ 503, 0, 2, 2, 0, 0 };
    static constexpr MapTile bakery1{ 503, 0, 2, 2, 1, 0 };
    static constexpr MapTile bakery2{ 503, 0, 2, 2, 0, 1 };
    static constexpr MapTile bakery3{ 503, 0, 2, 2, 1, 1 };

    static constexpr auto EXAMPLE_WIDTH{ 8 };
    static constexpr auto EXAMPLE_HEIGHT{ 12 };

    static const std::vector<MapTile> EXAMPLE_ISLAND
    {
        corner1,  coast2,  coast2, coast2, coast2, coast2,  coast2,  corner2,
        coast1,   grass,   grass,  grass,  grass,  grass,   grass,   coast3,
        coast1,   grass,   grass,  grass,  grass,  bakery0, bakery1, coast3,
        coast1,   grass,   grass,  grass,  grass,  bakery2, bakery3, coast3,
        coast1,   grass,   grass,  grass,  grass,  grass,   grass,   coast3,
        coast1,   grass,   grass,  grass,  grass,  grass,   grass,   coast3,
        coast1,   grass,   grass,  grass,  grass,  grass,   grass,   coast3,
        coast1,   grass,   grass,  grass,  grass,  grass,   grass,   coast3,
        coast1,   grass,   grass,  grass,  grass,  grass,   grass,   coast3,
        coast1,   grass,   grass,  grass,  grass,  grass,   grass,   coast3,
        coast1,   grass,   grass,  grass,  grass,  grass,   grass,   coast3,
        corner0,  fishing, coast0, coast0, coast0, coast0,  coast0,  corner3,
    };
}
