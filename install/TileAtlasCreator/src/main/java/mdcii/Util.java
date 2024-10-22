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

package mdcii;

/**
 * Contains some utils.
 */
public class Util {

    //-------------------------------------------------
    // Emulating unsigned arithmetic
    //-------------------------------------------------

    /*
    There are no primitive unsigned bytes in Java.
    The usual thing is to cast it to bigger type:
    */

    public static int byteToInt(byte value) {
        return (int) value & 0xFF;
    }

    //-------------------------------------------------
    // Color
    //-------------------------------------------------

    public static int rgbToInt(int red, int green, int blue) {
        var alpha = 255;

        return (alpha << 24) |
                (red << 16) |
                (green << 8) |
                (blue);
    }
}
