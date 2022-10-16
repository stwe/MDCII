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

#include <array>
#include <vector>
#include <glm/vec2.hpp>
#include "Zoom.h"

//-------------------------------------------------
// TileAtlas
//-------------------------------------------------

namespace mdcii::world
{
    /**
     * Creates OpenGL textures from Tile Atlas png files.
     *
     * After initialization, there is an OpenGL texture array
     * for each zoom level. In the array are several textures,
     * each representing a tile atlas.
     */
    class TileAtlas
    {
    public:
        //-------------------------------------------------
        // Constants SGFX
        //-------------------------------------------------

        /**
         * Number of SGFX atlas images.
         * 2 images a (64 rows * 64 rows)
         */
        static constexpr auto NR_OF_SGFX_ATLAS_IMAGES{ 2 };

        /**
         * Number of rows in SGFX atlas image.
         */
        static constexpr auto NR_OF_SGFX_ROWS{ 64 };

        /**
         * Width of the largest SGFX picture in the stadtfld.bsh.
         */
        static constexpr auto MAX_SGFX_WIDTH{ 16.0f };

        /**
         * Height of the largest SGFX picture in the stadtfld.bsh.
         */
        static constexpr auto MAX_SGFX_HEIGHT{ 71.0f };

        //-------------------------------------------------
        // Constants MGFX
        //-------------------------------------------------

        /**
         * Number of MGFX atlas images.
         * 6 images a (32 rows * 32 rows)
         */
        static constexpr auto NR_OF_MGFX_ATLAS_IMAGES{ 6 };

        /**
         * Number of rows in MGFX atlas image.
         */
        static constexpr auto NR_OF_MGFX_ROWS{ 32 };

        /**
         * Width of the largest MGFX picture in the stadtfld.bsh.
         */
        static constexpr auto MAX_MGFX_WIDTH{ 32.0f };

        /**
         * Height of the largest MGFX picture in the stadtfld.bsh.
         */
        static constexpr auto MAX_MGFX_HEIGHT{ 143.0f };

        //-------------------------------------------------
        // Constants GFX
        //-------------------------------------------------

        /**
         * Number of GFX atlas images.
         * 24 images a (16 rows * 16 rows)
         */
        static constexpr auto NR_OF_GFX_ATLAS_IMAGES{ 24 };

        /**
         * Number of rows in GFX atlas image.
         */
        static constexpr auto NR_OF_GFX_ROWS{ 16 };

        /**
         * Width of the largest GFX picture in the stadtfld.bsh.
         */
        static constexpr auto MAX_GFX_WIDTH{ 64.0f };

        /**
         * Height of the largest GFX picture in the stadtfld.bsh.
         */
        static constexpr auto MAX_GFX_HEIGHT{ 286.0f };

        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        static constexpr std::array<int, NR_OF_ZOOMS> IMAGES{ NR_OF_SGFX_ATLAS_IMAGES, NR_OF_MGFX_ATLAS_IMAGES, NR_OF_GFX_ATLAS_IMAGES };
        static constexpr std::array<int, NR_OF_ZOOMS> ROWS{ NR_OF_SGFX_ROWS, NR_OF_MGFX_ROWS, NR_OF_GFX_ROWS };
        static constexpr std::array<float, NR_OF_ZOOMS> WIDTHS{ MAX_SGFX_WIDTH, MAX_MGFX_WIDTH, MAX_GFX_WIDTH };
        static constexpr std::array<float, NR_OF_ZOOMS> HEIGHTS{ MAX_SGFX_HEIGHT, MAX_MGFX_HEIGHT, MAX_GFX_HEIGHT };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The created texture handles for all three zoom levels.
         */
        std::array<uint32_t, NR_OF_ZOOMS> textureIds{ 0, 0, 0 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        TileAtlas();

        TileAtlas(const TileAtlas& t_other) = delete;
        TileAtlas(TileAtlas&& t_other) noexcept = delete;
        TileAtlas& operator=(const TileAtlas& t_other) = delete;
        TileAtlas& operator=(TileAtlas&& t_other) noexcept = delete;

        ~TileAtlas() noexcept;

        //-------------------------------------------------
        // Offset
        //-------------------------------------------------

        /**
         * Determines where a texture is located on a atlas image/texture.
         *
         * @param t_textureIndex The texture index.
         * @param t_nrOfRows The number of rows.
         *
         * @return The offset coordinates.
         */
        static glm::vec2 GetTextureOffset(int t_textureIndex, int t_nrOfRows);

    protected:

    private:
        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes assets.
         */
        void Init();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Get the x offset on a atlas image/texture.
         *
         * @param t_textureIndex The texture index.
         * @param t_nrOfRows The number of rows.
         *
         * @return The x position.
         */
        static float GetTextureXOffset(int t_textureIndex, int t_nrOfRows);

        /**
         * Get the y offset on a atlas image/texture.
         *
         * @param t_textureIndex The texture index.
         * @param t_nrOfRows The number of rows.
         *
         * @return The y position.
         */
        static float GetTextureYOffset(int t_textureIndex, int t_nrOfRows);

        //-------------------------------------------------
        // Images
        //-------------------------------------------------

        /**
         * Creates a texture array from png atlas images.
         *
         * @param t_zoom Specifies the zoom for which the texture array should be created.
         * @param t_nrOfImages The number of png atlas images to load from a file.
         */
        void LoadAtlasImages(Zoom t_zoom, int t_nrOfImages);

        /**
         * Free the loaded png atlas images after creating the texture arrays.
         *
         * @param t_nrOfImages The number of atlas images to delete.
         * @param t_images The images to delete.
         */
        static void DeleteAtlasImages(int t_nrOfImages, const std::vector<unsigned char*>& t_images);

        //-------------------------------------------------
        // Texture array
        //-------------------------------------------------

        /**
         * Creates a texture array from atlas images for a given zoom.
         *
         * @param t_zoom The zoom.
         * @param t_images The atlas images.
         */
        void CreateTextureArray(Zoom t_zoom, const std::vector<unsigned char*>& t_images);

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        /**
         * Clean up.
         */
        void CleanUp() const;
    };
}
