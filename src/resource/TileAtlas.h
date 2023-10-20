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

#include <array>
#include <memory>
#include <vector>

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

namespace mdcii::world
{
    /**
     * @brief Forward declaration enum class Zoom.
     */
    enum class Zoom;
}

namespace mdcii::resource
{
    //-------------------------------------------------
    // TileAtlas
    //-------------------------------------------------

    /**
     * @brief Provides the TileAtlas for each zoom.
     */
    class TileAtlas
    {
    public:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * @brief The number of Tile Atlas. One for each zoom.
         */
        static constexpr auto NR_OF_TILE_ATLAS{ 3 };

        //-------------------------------------------------
        // Constants SGFX
        //-------------------------------------------------

        /**
         * @brief Number of SGFX atlas images.
         *
         * 2 images a (64 rows * 64 rows)
         */
        static constexpr auto NR_OF_SGFX_ATLAS_IMAGES{ 2 };

        /**
         * @brief Number of rows in SGFX atlas image.
         */
        static constexpr auto NR_OF_SGFX_ROWS{ 64 };

        /**
         * @brief Width of the largest SGFX picture in the stadtfld.bsh.
         */
        static constexpr auto MAX_SGFX_WIDTH{ 16.0f };

        /**
         * @brief Height of the largest SGFX picture in the stadtfld.bsh.
         */
        static constexpr auto MAX_SGFX_HEIGHT{ 71.0f };

        //-------------------------------------------------
        // Constants MGFX
        //-------------------------------------------------

        /**
         * @brief Number of MGFX atlas images.
         *
         * 6 images a (32 rows * 32 rows)
         */
        static constexpr auto NR_OF_MGFX_ATLAS_IMAGES{ 6 };

        /**
         * @brief Number of rows in MGFX atlas image.
         */
        static constexpr auto NR_OF_MGFX_ROWS{ 32 };

        /**
         * @brief Width of the largest MGFX picture in the stadtfld.bsh.
         */
        static constexpr auto MAX_MGFX_WIDTH{ 32.0f };

        /**
         * @brief Height of the largest MGFX picture in the stadtfld.bsh.
         */
        static constexpr auto MAX_MGFX_HEIGHT{ 143.0f };

        //-------------------------------------------------
        // Constants GFX
        //-------------------------------------------------

        /**
         * @brief Number of GFX atlas images.
         *
         * 24 images a (16 rows * 16 rows)
         */
        static constexpr auto NR_OF_GFX_ATLAS_IMAGES{ 24 };

        /**
         * @brief Number of rows in GFX atlas image.
         */
        static constexpr auto NR_OF_GFX_ROWS{ 16 };

        /**
         * @brief Width of the largest GFX picture in the stadtfld.bsh.
         */
        static constexpr auto MAX_GFX_WIDTH{ 64.0f };

        /**
         * @brief Height of the largest GFX picture in the stadtfld.bsh.
         */
        static constexpr auto MAX_GFX_HEIGHT{ 286.0f };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        std::array<std::unique_ptr<olc::Renderable>, NR_OF_SGFX_ATLAS_IMAGES> sgfxAtlas;
        std::array<std::unique_ptr<olc::Renderable>, NR_OF_MGFX_ATLAS_IMAGES> mgfxAtlas;
        std::array<std::unique_ptr<olc::Renderable>, NR_OF_GFX_ATLAS_IMAGES> gfxAtlas;

        std::array<std::vector<int32_t>, NR_OF_TILE_ATLAS> heights;

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
         * @brief Initialize the TileAtlas.
         */
        void Init();

        /**
         * @brief Load atlas images.
         */
        void LoadAtlasImages();

        /**
         * @brief Method to load all images heights by given Zoom.
         *
         * @param t_zoom The desired zoom.
         */
        void LoadHeightsByZoom(world::Zoom t_zoom);
    };
}
