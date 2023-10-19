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

package mdcii;

import java.awt.image.BufferedImage;
import java.util.Objects;

/**
 * Represents a BshTexture.
 * Contains a {@link BufferedImage} for each Bsh image.
 */
public class BshTexture {

    //-------------------------------------------------
    // Member
    //-------------------------------------------------

    /**
     * A {@link BufferedImage} object.
     */
    private final BufferedImage bufferedImage;

    //-------------------------------------------------
    // Ctors.
    //-------------------------------------------------

    /**
     * Constructs a new {@link BshTexture} object.
     *
     * @param bufferedImage {@link BufferedImage}
     */
    public BshTexture(BufferedImage bufferedImage) {
        this.bufferedImage = Objects.requireNonNull(bufferedImage, "bufferedImage must not be null");
    }

    //-------------------------------------------------
    // Getter
    //-------------------------------------------------

    /**
     * Get {@link #bufferedImage}.
     *
     * @return {@link #bufferedImage}
     */
    public BufferedImage getBufferedImage() {
        return bufferedImage;
    }

    //-------------------------------------------------
    // Clean up
    //-------------------------------------------------

    /**
     * Clean up.
     */
    public void cleanUp() {
        bufferedImage.getGraphics().dispose();
    }
}
