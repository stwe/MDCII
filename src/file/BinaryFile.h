#pragma once

#include <memory>
#include <string>
#include <vector>

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::chunk
{
    class Chunk;
}

//-------------------------------------------------
// BinaryFile
//-------------------------------------------------

namespace mdcii::file
{
    /**
     * Common stuff for all BinaryFile objects.
     */
    class BinaryFile
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The path to the file to load.
         */
        std::string filePath;

        /**
         * Each file has one or many Chunk objects.
         */
        std::vector<std::unique_ptr<chunk::Chunk>> chunks;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        BinaryFile() = delete;

        /**
         * Constructs a new BinaryFile object from a given path.
         *
         * @param t_filePath The path to the file to load.
         */
        explicit BinaryFile(std::string t_filePath);

        BinaryFile(const BinaryFile& t_other) = delete;
        BinaryFile(BinaryFile&& t_other) noexcept = delete;
        BinaryFile& operator=(const BinaryFile& t_other) = delete;
        BinaryFile& operator=(BinaryFile&& t_other) noexcept = delete;

        virtual ~BinaryFile() noexcept;

        //-------------------------------------------------
        // Read Chunks
        //-------------------------------------------------

        /**
         * Opens and reads the file.
         * Chunk objects are created from the file content.
         */
        void ReadChunksFromFile();

        //-------------------------------------------------
        // Override
        //-------------------------------------------------

        /**
         * Reads the data from the Chunk objects.
         */
        virtual void ReadDataFromChunks() = 0;

    protected:

    private:

    };
}
