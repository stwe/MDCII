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

#include "BshTileAtlas.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * @brief Forward declaration class World.
     */
    class World;
}

namespace mdcii::world::tile
{
    /**
     * @brief Forward declaration struct TerrainTile.
     */
    struct TerrainTile;
}

namespace mdcii::resource
{
    //-------------------------------------------------
    // TileAtlas
    //-------------------------------------------------

    /**
     * @brief Provides a Tile Atlas for all the Stadtfld graphics.
     */
    class TileAtlas : public BshTileAtlas
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief When one of the five timers expires, the respective frame is counted up.
         */
        inline static std::array frame_values{ 0, 0, 0, 0, 0 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        TileAtlas() = delete;

        /**
         * @brief Constructs a new TileAtlas object.
         *
         * @param t_world Pointer to the parent World.
         */
        explicit TileAtlas(const world::World* t_world);

        TileAtlas(const TileAtlas& t_other) = delete;
        TileAtlas(TileAtlas&& t_other) noexcept = delete;
        TileAtlas& operator=(const TileAtlas& t_other) = delete;
        TileAtlas& operator=(TileAtlas&& t_other) noexcept = delete;

        ~TileAtlas() noexcept override;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void RenderTile(int t_startX, int t_startY, const world::tile::TerrainTile* t_tile, const olc::Pixel& t_tint) const;
        static void CalcAnimationFrame(float t_elapsedTime);

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * @brief Width and height of the largest image.
         */
        static constexpr std::array<std::pair<world::Zoom, std::pair<float, float>>, world::NR_OF_ZOOMS> LARGEST_SIZE{
            { std::make_pair<world::Zoom, std::pair<float, float>>(world::Zoom::SGFX, std::make_pair<float, float>(16.0f, 71.0f)),
              std::make_pair<world::Zoom, std::pair<float, float>>(world::Zoom::MGFX, std::make_pair<float, float>(32.0f, 143.0f)),
              std::make_pair<world::Zoom, std::pair<float, float>>(world::Zoom::GFX, std::make_pair<float, float>(64.0f, 286.0f)) }
        };

        /**
         * @brief Number of rows in each Tile Atlas.
         */
        static constexpr std::array<int, world::NR_OF_ZOOMS> NR_OF_ROWS{ 64, 32, 16 };

        /**
         * @brief Number of SGFX Tile Atlas images.
         */
        static constexpr auto NR_OF_SGFX_ATLAS_IMAGES{ 2 };

        /**
         * @brief Number of MGFX Tile Atlas images.
         */
        static constexpr auto NR_OF_MGFX_ATLAS_IMAGES{ 6 };

        /**
         * @brief Number of GFX Tile Atlas images.
         */
        static constexpr auto NR_OF_GFX_ATLAS_IMAGES{ 24 };

        /**
         * @brief The name and directory of the Tile Atlas.
         */
        static constexpr std::string_view TILE_ATLAS_NAME{ "stadtfld" };

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

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        [[nodiscard]] int GetGfxForCurrentRotation(const world::tile::TerrainTile* t_tile) const;
        [[nodiscard]] float CalcOffset(const world::tile::TerrainTile* t_tile, int t_gfx) const;
    };
}
