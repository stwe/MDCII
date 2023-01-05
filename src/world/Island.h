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

#include "layer/Tile.h"

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
    //-------------------------------------------------
    // Forward declarations
    //-------------------------------------------------

    /**
     * Forward declaration class Terrain.
     */
    class Terrain;

    //-------------------------------------------------
    // Island
    //-------------------------------------------------

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
         * The start x world position.
         */
        int32_t startWorldX{ -1 };

        /**
         * The start y world position.
         */
        int32_t startWorldY{ -1 };

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
         * A Layer object to show a grid of the terrain.
         */
        std::unique_ptr<layer::GridLayer> gridLayer;

        /**
         * Pointer to the currently selected Tile object.
         */
        layer::Tile* currentSelectedTile{ nullptr };

        /**
         * Pointer to the currently Tile object under the mouse.
         */
        layer::Tile* currentTileUnderMouse{ nullptr };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Island() = delete;

        /**
         * Constructs an new Island object.
         *
         * @param t_context Access to shared objects.
         * @param t_terrain The parent Terrain object.
         */
        Island(std::shared_ptr<state::Context> t_context, Terrain* t_terrain);

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
         * Checks whether a given world position is in the Aabb of this island.
         *
         * @param t_position The world position to be checked.
         *
         * @return True or false.
         */
        [[nodiscard]] bool IsWorldPositionInAabb(const glm::ivec2& t_position) const;

        /**
         * Takes a world position and determines the island position from it.
         *
         * @param t_position The world position to convert.
         *
         * @return The island position.
         */
        [[nodiscard]] glm::ivec2 GetIslandPositionFromWorldPosition(const glm::ivec2& t_position) const;

        //-------------------------------------------------
        // Render
        //-------------------------------------------------

        /**
         * Render ImGui menu.
         */
        void RenderImGui() const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * To have access to the shared objects (Window, Camera, Original-Assets).
         */
        std::shared_ptr<state::Context> m_context;

        /**
         * The parent Terrain object.
         */
        Terrain* m_terrain{ nullptr };

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

    //-------------------------------------------------
    // Json
    //-------------------------------------------------

    void to_json(nlohmann::json& t_json, const Island& t_island);
}
