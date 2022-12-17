// This file is part of the MDCII project.
//
// Copyright (c) 2022. stwe <https://github.com/stwe/MDCII>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#pragma once

#include <glm/vec2.hpp>
#include "data/json.hpp"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::state
{
    /**
     * Forward declaration struct Context.
     */
    struct Context;
}

namespace mdcii::physics
{
    /**
     * Forward declaration struct Aabb.
     */
    struct Aabb;
}

namespace mdcii::layer
{
    /**
     * Forward declaration class TerrainLayer.
     */
    class TerrainLayer;

    /**
     * Forward declaration class GridLayer.
     */
    class GridLayer;
}

//-------------------------------------------------
// Island
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * Represents an island.
     */
    class Island
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The island width.
         */
        int32_t width{ -1 };

        /**
         * The island height.
         */
        int32_t height{ -1 };

        /**
         * The world x position.
         */
        int32_t worldX{ -1 };

        /**
         * The world y position.
         */
        int32_t worldY{ -1 };

        /**
         * An Aabb (axis-aligned bounding box) object for collision detection.
         */
        std::unique_ptr<physics::Aabb> aabb;

        /**
         * A Layer object containing only the coast of the island.
         */
        std::unique_ptr<layer::TerrainLayer> coastLayer;

        /**
         * A Layer object containing only the terrain of the island without additional buildings.
         */
        std::unique_ptr<layer::TerrainLayer> terrainLayer;

        /**
         * A Layer object containing only additional buildings.
         */
        std::unique_ptr<layer::TerrainLayer> buildingsLayer;

        /**
         * A Layer object combining terrainLayer and buildingsLayer.
         */
        std::unique_ptr<layer::TerrainLayer> mixedLayer;

        /**
         * A Layer object to show a grid.
         */
        std::unique_ptr<layer::GridLayer> gridLayer;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Island() = delete;

        /**
         * Constructs an new Island object.
         *
         * @param t_context Access to shared objects.
         */
        explicit Island(std::shared_ptr<state::Context> t_context);

        Island(const Island& t_other) = delete;
        Island(Island&& t_other) noexcept = delete;
        Island& operator=(const Island& t_other) = delete;
        Island& operator=(Island&& t_other) noexcept = delete;

        ~Island() noexcept;

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes this Island from a Json value.
         *
         * @param t_json The Json value.
         */
        void InitValuesFromJson(const nlohmann::json& t_json);

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * Checks whether a given position in this Island.
         * The Aabb of the Island is used for this test.
         *
         * @param t_position The position to be checked.
         *
         * @return True or false.
         */
        [[nodiscard]] bool IsPositionOnIsland(const glm::ivec2& t_position) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * To have access to the shared objects (Window, Camera, Original-Assets).
         */
        std::shared_ptr<state::Context> m_context;

        //-------------------------------------------------
        // Layer
        //-------------------------------------------------

        /**
         * Creates the Layer objects from a Json value.
         *
         * @param t_json The Json value.
         */
        void CreateLayersFromJson(const nlohmann::json& t_json);
    };
}
