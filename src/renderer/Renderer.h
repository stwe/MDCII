// This file is part of the MDCII project.
//
// Copyright (c) 2024. stwe <https://github.com/stwe/MDCII>
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

#include "vendor/olc/olcPixelGameEngine.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * @brief Forward declaration class World.
     */
    class World;

    /**
     * @brief Forward declaration class Island.
     */
    class Island;
}

namespace mdcii::world::tile
{
    /**
     * @brief Forward declaration struct Tile.
     */
    struct Tile;

    /**
     * @brief Forward declaration struct TerrainTile.
     */
    struct TerrainTile;

    /**
     * @brief Forward declaration struct FigureTile.
     */
    struct FigureTile;
}

namespace mdcii::world::layer
{
    /**
     * @brief Forward declaration enum class LayerType.
     */
    enum class LayerType;
}

namespace mdcii::resource
{
    /**
     * @brief Forward declaration enum class Asset.
     */
    enum class Asset;
}

namespace mdcii::renderer
{
    //-------------------------------------------------
    // Renderer
    //-------------------------------------------------

    /**
     * @brief Represents a Renderer.
     */
    class Renderer
    {
    public:
        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        /**
         * @brief Renders a specific asset on the screen.
         *
         * @param t_asset The asset to render.
         * @param t_startX The x offset.
         * @param t_startY The y offset.
         * @param t_world Pointer to the World object.
         * @param t_tile Pointer to the TerrainTile object associated with the asset.
         * @param t_minusElevation Indicates whether to adjust for elevation.
         * @param t_tint Color tint for the asset (default is white).
         */
        static void RenderAsset(
            resource::Asset t_asset,
            int t_startX,
            int t_startY,
            const world::World* t_world,
            const world::tile::Tile* t_tile,
            bool t_minusElevation,
            const olc::Pixel& t_tint = olc::WHITE
        );

        /**
         * @brief Renders various parts of the world, such as deep water and islands.
         *
         * @param t_world Pointer to the World object.
         * @param t_elapsedTime The elapsed time for animations.
         */
        static void RenderWorldParts(const world::World* t_world, float t_elapsedTime);

        /**
         * @brief Renders the entire world, including deep water, islands and figures.
         *
         * @param t_world Pointer to the World object.
         * @param t_elapsedTime The elapsed time for animations.
         */
        static void RenderWorld(const world::World* t_world, float t_elapsedTime);

    protected:

    private:
        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * @brief Renders an island based on the specified layer type.
         *
         * @param t_world Pointer to the World object.
         * @param t_island Pointer to the Island object to be rendered.
         * @param t_layerType The type of layer to render.
         */
        static void RenderIsland(const world::World* t_world, world::Island* t_island, world::layer::LayerType t_layerType);

        static void UpdateAnimations(float t_elapsedTime);
        static void RenderDeepWater(const world::World* t_world);
        static void RenderFigureOnTopOfTerrainTile(const world::World* t_world, world::Island* t_island, const world::tile::TerrainTile* t_terrainTile);
        static void RenderTerrainTile(const world::World* t_world, const world::Island* t_island, world::tile::TerrainTile* t_terrainTile, const olc::Pixel& t_tint = olc::WHITE);
        static void RenderFigureTile(const world::World* t_world, const world::Island* t_island, world::tile::FigureTile* t_figureTile);
    };
}
