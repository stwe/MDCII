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

import java.io.IOException;
import java.nio.file.Path;
import java.util.Objects;

import static mdcii.Log.LOGGER;

/**
 * Represents a PaletteFile.
 * The class loads the pallet values from the <i>stadtfld.col</i> file.
 */
public class PaletteFile extends BinaryFile {

    //-------------------------------------------------
    // Constants
    //-------------------------------------------------

    /**
     * The number of palette colors.
     */
    private static final int NUMBER_OF_COLORS = 256;

    /**
     * The number of expected {@link Chunk} objects in the file.
     */
    private static final int NUMBER_OF_CHUNKS = 1;

    /**
     * The ID of the {@link Chunk}.
     */
    private static final String CHUNK_ID = "COL";

    //-------------------------------------------------
    // Member
    //-------------------------------------------------

    /**
     * An array with the palette color values.
     * The color values are saved as int.
     */
    private final int[] palette = new int[NUMBER_OF_COLORS];

    //-------------------------------------------------
    // Ctors.
    //-------------------------------------------------

    /**
     * Constructs a new {@link PaletteFile} object.
     *
     * @param path The {@link Path} to the palette file.
     * @throws IOException If an I/O error is thrown.
     */
    PaletteFile(Path path) throws IOException {
        super(Objects.requireNonNull(path, "path must not be null"));

        LOGGER.debug("Creates PaletteFile object from file {}.", path);

        if (getNumberOfChunks() != NUMBER_OF_CHUNKS) {
            throw new RuntimeException("Invalid number of Chunks.");
        }

        if (!chunkIndexHasId(0, CHUNK_ID)) {
            throw new RuntimeException("Invalid Chunk Id.");
        }

        readDataFromChunks();
    }

    //-------------------------------------------------
    // Getter
    //-------------------------------------------------

    /**
     * Get {@link #palette}.
     *
     * @return {@link #palette}
     */
    public int[] getPalette() {
        return palette;
    }

    //-------------------------------------------------
    // BinaryFileInterface
    //-------------------------------------------------

    @Override
    public void readDataFromChunks() {
        LOGGER.debug("Start reading Palette data from Chunks...");

        var chunk0 = getChunk(0);

        for (int i = 0; i < palette.length; i++) {
            int red = Util.byteToInt(chunk0.getData().get());
            int green = Util.byteToInt(chunk0.getData().get());
            int blue = Util.byteToInt(chunk0.getData().get());

            chunk0.getData().get(); // skip next byte

            palette[i] = Util.rgbToInt(red, green, blue);
        }

        LOGGER.debug("Palette data read successfully.");
    }
}

