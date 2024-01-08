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
#include "world/Zoom.h"

namespace mdcii::resource
{
    //-------------------------------------------------
    // BshTileAtlas
    //-------------------------------------------------

    /**
     * @brief Base class for all Tile Atlas types.
     */
    class BshTileAtlas
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        BshTileAtlas() = delete;

        /**
         * @brief Constructs a new BshTileAtlas object.
         *
         * @param t_atlasPath The relative path to the Tile Atlas.
         * @param t_sgfxImages Number of SGFX Tile Atlas images.
         * @param t_mgfxImages Number of MGFX Tile Atlas images.
         * @param t_gfxImages Number of GFX Tile Atlas images.
         */
        BshTileAtlas(std::string_view t_atlasPath, int t_sgfxImages, int t_mgfxImages, int t_gfxImages);

        BshTileAtlas(const BshTileAtlas& t_other) = delete;
        BshTileAtlas(BshTileAtlas&& t_other) noexcept = delete;
        BshTileAtlas& operator=(const BshTileAtlas& t_other) = delete;
        BshTileAtlas& operator=(BshTileAtlas&& t_other) noexcept = delete;

        virtual ~BshTileAtlas() noexcept;

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
         * @brief Determines where a gfx is located on a Texture Atlas.
         *
         * @param t_gfx The sprite index.
         * @param t_rows The number of rows.
         *
         * @return The offset coordinates.
         */
        [[nodiscard]] static olc::vi2d GetAtlasOffset(int t_gfx, int t_rows);

    protected:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief Array of vectors containing the real heights of each image.
         */
        std::array<std::vector<int>, world::NR_OF_ZOOMS> m_heights;

        /**
         * @brief Contains the Tile Atlas images for each zoom as a renderable object.
         */
        std::array<std::vector<std::unique_ptr<const olc::Renderable>>, world::NR_OF_ZOOMS> m_atlas;

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * @brief The relative path to the Tile Atlas.
         */
        std::string_view m_atlasPath;

        /**
         * @brief The number of Tile Atlas images for each zoom.
         */
        std::array<int, world::NR_OF_ZOOMS> m_nrImages{};

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * @brief Initialize the Tile Atlas.
         */
        void Init();

        //-------------------------------------------------
        // Load
        //-------------------------------------------------

        /**
         * @brief Load Tile Atlas images.
         */
        void LoadAtlasImages();

        /**
         * @brief Load Tile Atlas images by given zoom.
         *
         * @param t_zoom The specified zoom.
         */
        void LoadAtlasImagesByZoom(world::Zoom t_zoom);

        /**
         * @brief Load all image heights by given zoom from file.
         *
         * @param t_zoom The specified zoom.
         */
        void LoadHeightsByZoom(world::Zoom t_zoom);
    };
}
