#pragma once

#include <string>
#include <vector>

//-------------------------------------------------
// Chunk
//-------------------------------------------------

namespace mdcii::chunk
{
    /**
     * Represents a Chunk. The Chunks file format is used for all binary type files.
     * The Chunk header consists of a type Id as an ASCII string that is padded with
     * junk to fill a full 16 bytes. Then comes a 4-byte integer that indicates the size
     * of the data block without the header.
     */
    class Chunk
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The Chunk type identifier.
         */
        std::string id;

        /**
         * The size of the Chunk, excluding the header.
         */
        uint32_t length{ 0u };

        /**
         * The Chunk data.
         */
        std::vector<uint8_t> data;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Chunk() = delete;

        /**
         * Constructs a new Chunk object.
         *
         * @param t_input The content of a file.
         */
        explicit Chunk(std::ifstream& t_input);

        Chunk(const Chunk& t_other) = delete;
        Chunk(Chunk&& t_other) noexcept = delete;
        Chunk& operator=(const Chunk& t_other) = delete;
        Chunk& operator=(Chunk&& t_other) noexcept = delete;

        ~Chunk() noexcept;

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * The size in bytes of the Chunk type identifier.
         */
        static constexpr auto ID_SIZE_IN_BYTES{ 16 };
    };
}
