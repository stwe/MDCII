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

package mdcii;

/**
 * Constants for Tile Atlas Images creation.
 */
public class TileAtlas {

    //-------------------------------------------------
    // Constants GFX
    //-------------------------------------------------

    /**
     * Number of GFX atlas images.
     * 24 images a (16 rows * 16 rows)
     */
    public static final int NR_OF_GFX_ATLAS_IMAGES = 24;

    /**
     * Number of rows in GFX atlas image.
     */
    public static final int NR_OF_GFX_ROWS = 16;

    /**
     * Width of the largest GFX picture in the <i>stadtfld.bsh</i>.
     * Can also be read with the BennoFiles class.
     */
    public static final float MAX_GFX_WIDTH = 64.0f;

    /**
     * Height of the largest GFX picture in the <i>stadtfld.bsh</i>.
     * Can also be read with the BennoFiles class.
     */
    public static final float MAX_GFX_HEIGHT = 286.0f;

    //-------------------------------------------------
    // Constants MGFX
    //-------------------------------------------------

    /**
     * Number of MGFX atlas images.
     * 6 images a (32 rows * 32 rows)
     */
    public static final int NR_OF_MGFX_ATLAS_IMAGES = 6;

    /**
     * Number of rows in MGFX atlas image.
     */
    public static final int NR_OF_MGFX_ROWS = 32;

    /**
     * Width of the largest MGFX picture in the <i>stadtfld.bsh</i>.
     * Can also be read with the BennoFiles class.
     */
    public static final float MAX_MGFX_WIDTH = 32.0f;

    /**
     * Height of the largest MGFX picture in the <i>stadtfld.bsh</i>.
     * Can also be read with the BennoFiles class.
     */
    public static final float MAX_MGFX_HEIGHT = 143.0f;

    //-------------------------------------------------
    // Constants SGFX
    //-------------------------------------------------

    /**
     * Number of SGFX atlas images.
     * 2 images a (64 rows * 64 rows)
     */
    public static final int NR_OF_SGFX_ATLAS_IMAGES = 2;

    /**
     * Number of rows in SGFX atlas image.
     */
    public static final int NR_OF_SGFX_ROWS = 64;

    /**
     * Width of the largest SGFX picture in the <i>stadtfld.bsh</i>.
     * Can also be read with the BennoFiles class.
     */
    public static final float MAX_SGFX_WIDTH = 16.0f;

    /**
     * Height of the largest SGFX picture in the <i>stadtfld.bsh</i>.
     * Can also be read with the BennoFiles class.
     */
    public static final float MAX_SGFX_HEIGHT = 71.0f;
}
