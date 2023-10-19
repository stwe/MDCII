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

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Objects;

import static mdcii.Log.LOGGER;

/**
 * Common stuff for all {@link BinaryFile} objects.
 */
public abstract class BinaryFile implements BinaryFileInterface {

    //-------------------------------------------------
    // Member
    //-------------------------------------------------

    /**
     * The {@link Path} to the file.
     */
    private final Path path;

    /**
     * An {@link InputStream}.
     */
    private InputStream inputStream = null;

    /**
     * Each file has one or many {@link Chunk} objects.
     */
    private final ArrayList<Chunk> chunks = new ArrayList<>();

    //-------------------------------------------------
    // Ctors.
    //-------------------------------------------------

    /**
     * Constructs a new {@link BinaryFile} object from {@link Path}.
     *
     * @param path The {@link Path} to the file.
     * @throws IOException If an I/O error is thrown.
     */
    public BinaryFile(Path path) throws IOException {
        this.path = Objects.requireNonNull(path, "path must not be null");

        LOGGER.debug("Reading Chunks from {}.", path);

        readChunksFromFile();
    }

    //-------------------------------------------------
    // Getter
    //-------------------------------------------------

    /**
     * Get {@link #path}.
     *
     * @return {@link #path}
     */
    public Path getPath() {
        return path;
    }

    /**
     * Get {@link #chunks}.
     *
     * @return {@link #chunks}
     */
    public ArrayList<Chunk> getChunks() {
        return chunks;
    }

    /**
     * Get size (number of elements) of {@link #chunks}.
     *
     * @return int
     */
    public int getNumberOfChunks() {
        return chunks.size();
    }

    /**
     * Get {@link Chunk} by index.
     *
     * @param chunkIndex The given index.
     *
     * @return {@link Chunk}
     */
    public Chunk getChunk(int chunkIndex) {
        return chunks.get(chunkIndex);
    }

    /**
     * Checks whether a {@link Chunk} has the given id.
     *
     * @param chunkIndex The index of the {@link Chunk} in {@link #chunks}.
     * @param chunkId The ID.
     *
     * @return boolean
     */
    boolean chunkIndexHasId(int chunkIndex, String chunkId) {
        return getChunk(chunkIndex).getId().equals(chunkId);
    }

    //-------------------------------------------------
    // BinaryFileInterface
    //-------------------------------------------------

    @Override
    public void readChunksFromFile() throws IOException {
        if (inputStream == null) {
            if (path == null) {
                throw new RuntimeException("No path given.");
            }

            inputStream = new FileInputStream(path.toFile());
        }

        while (inputStream.available() > 0) {
            var chunk = new Chunk(inputStream);
            chunks.add(chunk);
        }
    }

    @Override
    public abstract void readDataFromChunks() throws Exception;
}
