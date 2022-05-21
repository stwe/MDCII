#include <fstream>
#include "BinaryFile.h"
#include "Log.h"
#include "MdciiException.h"
#include "chunk/Chunk.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::file::BinaryFile::BinaryFile(std::string t_filePath)
    : filePath{ std::move(t_filePath) }
{
    Log::MDCII_LOG_DEBUG("[BinaryFile::BinaryFile()] Create BinaryFile.");

    ReadChunksFromFile();
}

mdcii::file::BinaryFile::~BinaryFile() noexcept
{
    Log::MDCII_LOG_DEBUG("[BinaryFile::~BinaryFile()] Destruct BinaryFile.");
}

//-------------------------------------------------
// Read Chunks
//-------------------------------------------------

void mdcii::file::BinaryFile::ReadChunksFromFile()
{
    Log::MDCII_LOG_DEBUG("[BinaryFile::ReadChunksFromFile()] Reading Chunks from {}.", filePath);

    std::ifstream input(filePath, std::ios::binary);

    if (!input.is_open())
    {
        throw MDCII_EXCEPTION("[BinaryFile::ReadChunksFromFile()] Error while opening file " + filePath + ".");
    }

    while (input.peek() != EOF)
    {
        chunks.emplace_back(std::make_unique<chunk::Chunk>(input));
    }

    input.close();

    if (chunks.empty())
    {
        Log::MDCII_LOG_WARN("[BinaryFile::ReadChunksFromFile()] No Chunks were found.");
    }
    else
    {
        Log::MDCII_LOG_DEBUG("[BinaryFile::ReadChunksFromFile()] {} Chunk(s) were successfully read.", chunks.size());
    }
}
