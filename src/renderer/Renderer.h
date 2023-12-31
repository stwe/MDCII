// This file is part of the MDCII project.
//
// Copyright (c) 2023. stwe <https://github.com/stwe/MDCII>
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
     * @brief Forward declaration enum class LayerType.
     */
    enum class LayerType;

    /**
     * @brief Forward declaration class Island.
     */
    class Island;

    /**
     * @brief Forward declaration struct Tile.
     */
    struct Tile;

    /**
     * @brief Forward declaration class World.
     */
    class World;
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
        // Ctors. / Dtor.
        //-------------------------------------------------

        Renderer() = delete;

        /**
         * @brief Constructs a new Renderer object.
         *
         * @param t_world Pointer to the parent World.
         */
        explicit Renderer(const world::World* t_world);

        Renderer(const Renderer& t_other) = delete;
        Renderer(Renderer&& t_other) noexcept = delete;
        Renderer& operator=(const Renderer& t_other) = delete;
        Renderer& operator=(Renderer&& t_other) noexcept = delete;

        ~Renderer() noexcept;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        [[nodiscard]] float CalcOffset(const world::Tile* t_tile, int t_gfx) const;
        [[nodiscard]] int GetGfxForCurrentRotation(const world::Tile* t_tile) const;
        void RenderBuilding(int t_startX, int t_startY, const world::Tile* t_tile, const olc::Pixel& t_tint = olc::WHITE) const;

        void RenderAsset(
                resource::Asset t_asset,
                int t_startX,
                int t_startY,
                const world::Tile* t_tile,
                bool t_minusElevation,
                const olc::Pixel& t_tint = olc::WHITE
        ) const;

        void RenderIsland(const world::Island* t_island, world::LayerType t_layerType, bool t_renderGrid) const;
        static void CalcAnimationFrame(float t_elapsedTime);
        void RenderIslands(bool t_renderGrid) const;
        void RenderDeepWater(bool t_renderGrid) const;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * @brief Returns the number of the Texture Atlas on which the given gfx is located.
         *
         * @param t_gfx The sprite index.
         * @param t_rows The number of rows.
         *
         * @return The number of the Texture Atlas.
         */
        [[nodiscard]] static int GetAtlasIndex(int t_gfx, int t_rows);

        /**
         * @brief Determines where a sprite is located on an atlas image/texture.
         *
         * @param t_gfx The sprite index.
         * @param t_rows The number of rows.
         *
         * @return The offset coordinates.
         */
        [[nodiscard]] static olc::vi2d GetAtlasOffset(int t_gfx, int t_rows);

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * @brief To avoid m_frame_values[] will exceed that value.
         */
        static constexpr int MAX_FRAME_VALUE{ 1000 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Pointer to the parent World.
         */
        const world::World* m_world{ nullptr };

        /**
         * @brief Five timers to measure the times for the next frame.
         *
         * The animation speeds of the buildings are finally defined.
         * So after 90, 130, 150, 180, 220 milliseconds the next frame from the animation is used.
         */
        inline static std::array m_timer_values{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

        /**
         * @brief When one of the five timers expires, the respective frame is counted up.
         */
        inline static std::array m_frame_values{ 0, 0, 0, 0, 0 };
    };
}
