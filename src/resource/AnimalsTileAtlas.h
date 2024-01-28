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

    /**
     * @brief Forward declaration class Island.
     */
    class Island;
}

namespace mdcii::world::tile
{
    /**
     * @brief Forward declaration struct FigureTile.
     */
    struct FigureTile;
}

namespace mdcii::resource
{
    //-------------------------------------------------
    // AnimalsTileAtlas
    //-------------------------------------------------

    /**
     * @brief Provides a Tile Atlas for all the animal figures.
     */
    class AnimalsTileAtlas : public BshTileAtlas
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        AnimalsTileAtlas() = delete;

        /**
         * @brief Constructs a new AnimalsTileAtlas object.
         *
         * @param t_world Pointer to the parent World.
         */
        explicit AnimalsTileAtlas(const world::World* t_world);

        AnimalsTileAtlas(const AnimalsTileAtlas& t_other) = delete;
        AnimalsTileAtlas(AnimalsTileAtlas&& t_other) noexcept = delete;
        AnimalsTileAtlas& operator=(const AnimalsTileAtlas& t_other) = delete;
        AnimalsTileAtlas& operator=(AnimalsTileAtlas&& t_other) noexcept = delete;

        ~AnimalsTileAtlas() noexcept override;

        //-------------------------------------------------
        // Logic
        //-------------------------------------------------

        void Render(int t_startX, int t_startY, const world::tile::FigureTile* t_tile, const olc::Pixel& t_tint) const;
        static void CalcAnimationFrame(float t_elapsedTime);
        void RenderIsland(world::Island* t_island) const;
        void RenderIslands() const;

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * @brief Width and height of the largest image.
         */
        static constexpr std::array<std::pair<world::Zoom, std::pair<float, float>>, world::NR_OF_ZOOMS> LARGEST_SIZE{
            { std::make_pair<world::Zoom, std::pair<float, float>>(world::Zoom::SGFX, std::make_pair<float, float>(17.0f, 16.0f)),
              std::make_pair<world::Zoom, std::pair<float, float>>(world::Zoom::MGFX, std::make_pair<float, float>(35.0f, 32.0f)),
              std::make_pair<world::Zoom, std::pair<float, float>>(world::Zoom::GFX, std::make_pair<float, float>(71.0f, 65.0f)) }
        };

        /**
         * @brief Number of rows in each Animals Tile Atlas.
         */
        static constexpr auto NR_OF_ROWS{ 16 };

        /**
         * @brief Number of Animals Tile Atlas images for each zoom.
         */
        static constexpr auto NR_OF_ATLAS_IMAGES{ 3 };

        /**
         * @brief The name and directory of the Animals Tile Atlas.
         */
        static constexpr std::string_view TILE_ATLAS_NAME{ "animals" };

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
         * @brief Timers to measure the times for the next frame.
         *
         * The animation speeds of the animals are finally defined.
         * So after 75, 110, 130, 135, 195, 300, 500, 750 and 1000 milliseconds the next frame from the animation is used.
         */
        inline static std::array m_timer_values{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

        /**
         * @brief When one of the timers expires, the respective frame is counted up.
         */
        inline static std::array m_frame_values{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        [[nodiscard]] float CalcOffset(int t_gfx) const;
    };
}
