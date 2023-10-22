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

#include <memory>
#include <vector>
#include "world/Zoom.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace olc
{
    /**
     * @brief Forward declaration class Renderable.
     */
    class Renderable;
}

namespace mdcii::resource
{
    //-------------------------------------------------
    // TileAtlas
    //-------------------------------------------------

    /**
     * @brief Provides a Tile Atlas for each zoom.
     */
    class TileAtlas
    {
    public:
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
         * Number of rows in each Tile Atlas.
         */
        static constexpr std::array<int, world::NR_OF_ZOOMS> NR_OF_ROWS{ 64, 32, 16 };

        /**
         * @brief Number of SGFX Tile Atlas images.
         *
         * 2 images a (64 rows * 64 rows)
         */
        static constexpr auto NR_OF_SGFX_ATLAS_IMAGES{ 2 };

        /**
         * @brief Number of MGFX Tile Atlas images.
         *
         * 6 images a (32 rows * 32 rows)
         */
        static constexpr auto NR_OF_MGFX_ATLAS_IMAGES{ 6 };

        /**
         * @brief Number of GFX Tile Atlas images.
         *
         * 24 images a (16 rows * 16 rows)
         */
        static constexpr auto NR_OF_GFX_ATLAS_IMAGES{ 24 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
          * @brief Contains the Tile Atlas images for each zoom as a renderable object.
          */
        std::array<std::vector<std::unique_ptr<olc::Renderable>>, 3> atlas;

        /**
         * @brief Array of vectors containing the real heights of each image.
         */
        std::array<std::vector<int>, world::NR_OF_ZOOMS> heights;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        TileAtlas();

        TileAtlas(const TileAtlas& t_other) = delete;
        TileAtlas(TileAtlas&& t_other) noexcept = delete;
        TileAtlas& operator=(const TileAtlas& t_other) = delete;
        TileAtlas& operator=(TileAtlas&& t_other) noexcept = delete;

        ~TileAtlas() noexcept;

    protected:

    private:
        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * @brief Initialize the Tile Atlas.
         */
        void Init();

        /**
         * @brief Load Tile Atlas images.
         */
        void LoadAtlasImages();

        /**
         * @brief Method to load all Tile Atlas images heights by given Zoom.
         *
         * @param t_zoom The zoom.
         */
        void LoadHeightsByZoom(world::Zoom t_zoom);
    };
}
