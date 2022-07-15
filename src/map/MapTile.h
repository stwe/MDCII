#pragma once

#include <array>
#include <vector>
#include <glm/vec2.hpp>
#include "ecs/entt.hpp"

namespace mdcii::map
{
    //-------------------------------------------------
    // MapTile
    //-------------------------------------------------

    /**
     * MapTile represents a part of a layer.
     */
    struct MapTile
    {
        /**
         * The width of a tile.
         */
        static constexpr auto TILE_WIDTH{ 64 };

        /**
         * The half width of a tile.
         */
        static constexpr auto TILE_WIDTH_HALF{ 32 };

        /**
         * The height of a tile.
         */
        static constexpr auto TILE_HEIGHT{ 32 };

        /**
         * The half height of a tile.
         */
        static constexpr auto TILE_HEIGHT_HALF{ 16 };

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
         * The x position of the tile in local/object space of the building.
         */
        int32_t x{ 0 };

        /**
         * The y position of the tile in local/object space of the building.
         */
        int32_t y{ 0 };

        /**
         * The x position on the world map.
         */
        int32_t mapX{ -1 };

        /**
         * The y position on the world map.
         */
        int32_t mapY{ -1 };

        /**
         * An isometric position on the screen for each
         * rotation (world space).
         */
        std::array<glm::vec2, 4> screenPositions{};

        /**
         * The index for each rotation is needed for sorting.
         */
        std::array<int, 4> indices{};

        /**
         * The Bsh graphic for each rotation.
         * In some cases the same gfx is used for all rotations.
         */
        std::vector<int> gfxs{};

        /**
         * Stores the entity handle created for this tile.
         */
        entt::entity entity{ entt::null };

        /**
         * Stores another entity handle created for this tile.
         * This is needed to show a grid for debugging purposes.
         */
        entt::entity gridEntity{ entt::null };

        /**
         * For better readability/convenience.
         *
         * @return True if a valid building Id is present.
         */
        [[nodiscard]] bool HasBuilding() const { return buildingId >= 0; }
    };
}
