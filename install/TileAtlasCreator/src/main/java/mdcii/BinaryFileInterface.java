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

import java.io.IOException;

/**
 * Interface for loading BinaryFiles.
 */
public interface BinaryFileInterface {

    /**
     * Opens and reads the file. {@link Chunk} objects are created from the file content.
     *
     * @throws IOException If an I/O error is thrown.
     */
    void readChunksFromFile() throws IOException;

    /**
     * Reads the data from the {@link Chunk} objects.
     *
     * @throws Exception If an error is thrown.
     */
    void readDataFromChunks() throws Exception;
}
