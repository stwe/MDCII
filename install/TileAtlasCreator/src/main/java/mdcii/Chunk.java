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

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Objects;

/**
 * Represents a Chunk. The Chunks file format is used for all binary type files.
 * The Chunk header consists of a type ID as an ASCII string that is padded with
 * junk to fill a full 16 bytes. Then comes a 4-byte integer that indicates the size
 * of the data block without the header.
 */
public class Chunk {

    //-------------------------------------------------
    // Constants
    //-------------------------------------------------

    /**
     * The size in bytes of the Chunk identification.
     */
    private static final int ID_SIZE_IN_BYTES = 16;

    /**
     * The size in bytes that indicates the size of the Chunk data.
     */
    private static final int DATA_LENGTH_SIZE_IN_BYTES = 4;

    //-------------------------------------------------
    // Member
    //-------------------------------------------------

    /**
     * The Chunk type identifier.
     */
    private final byte[] id = new byte[ID_SIZE_IN_BYTES];

    /**
     * The Chunk type identifier as String.
     */
    private String idStr;

    /**
     * The size of the Chunk, excluding the header.
     */
    private final byte[] dataLength = new byte[DATA_LENGTH_SIZE_IN_BYTES];

    /**
     * The size of the Chunk, excluding the header as Int.
     */
    private int dataLengthInt;

    /**
     * The {@link ByteBuffer} with the Chunk data.
     */
    private ByteBuffer byteBuffer;

    //-------------------------------------------------
    // Ctors.
    //-------------------------------------------------

    /**
     * Constructs a new {@link Chunk} object.
     *
     * @param inputStream {@link InputStream}
     * @throws IOException If an I/O error is thrown.
     */
    public Chunk(InputStream inputStream) throws IOException {
        readId(inputStream);
        readDataLength(inputStream);
        readData(inputStream);
    }

    //-------------------------------------------------
    // Getter
    //-------------------------------------------------

    /**
     * Get {@link #id}.
     *
     * @return {@link #id}
     */
    public String getId() {
        return idStr;
    }

    /**
     * Get {@link #byteBuffer}.
     *
     * @return {@link #byteBuffer}
     */
    public ByteBuffer getData() {
        return byteBuffer;
    }

    //-------------------------------------------------
    // Helper
    //-------------------------------------------------

    /**
     * Read {@link #id} and create {@link #idStr} from the InputStream.
     *
     * @param inputStream {@link InputStream}
     * @throws IOException If an I/O error is thrown.
     */
    private void readId(InputStream inputStream) throws IOException {
        var result = Objects.requireNonNull(inputStream, "inputStream must not be null").read(id, 0, id.length);
        checkNumberOfBytesRead(result, id.length);

        idStr = new String(id).split("\0")[0];
    }

    /**
     * Read {@link #dataLength} and create {@link #dataLengthInt} from the InputStream.
     *
     * @param inputStream {@link InputStream}
     * @throws IOException If an I/O error is thrown.
     */
    private void readDataLength(InputStream inputStream) throws IOException {
        var result = Objects.requireNonNull(inputStream, "inputStream must not be null").read(dataLength, 0, dataLength.length);
        checkNumberOfBytesRead(result, dataLength.length);

        dataLengthInt = ByteBuffer.wrap(dataLength).order(ByteOrder.nativeOrder()).getInt();
    }

    /**
     * Read {@link #byteBuffer} from the InputStream.
     *
     * @param inputStream {@link InputStream}
     * @throws IOException If an I/O error is thrown.
     */
    private void readData(InputStream inputStream) throws IOException {
        byte[] bytes = new byte[dataLengthInt];

        var offset = 0;
        while (offset < bytes.length) {
            var result = Objects.requireNonNull(inputStream, "inputStream must not be null").read(bytes, offset, dataLengthInt - offset);
            if (result == -1) {
                break;
            }
            offset += result;
        }

        checkNumberOfBytesRead(offset, dataLengthInt);

        byteBuffer = ByteBuffer.wrap(bytes).order(ByteOrder.nativeOrder());
    }

    /**
     * Checks that the correct number of bytes were read.
     *
     * @param result Number of bytes read.
     * @param length The expected number of bytes.
     */
    private void checkNumberOfBytesRead(int result, int length) {
        if (result != length) {
            if (idStr != null) {
                throw new RuntimeException("Wrong total number of bytes read. Id: " + idStr + ", result: " + result + ", expected length: " + length);
            }

            throw new RuntimeException("Wrong total number of bytes read. result: " + result + ", expected length: " + length);
        }
    }
}
