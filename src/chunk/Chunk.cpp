#include <fstream>
#include "Chunk.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::chunk::Chunk::Chunk(std::ifstream& t_input)
{
    t_input.read(id.data(), ID_SIZE_IN_BYTES);
    t_input.read(reinterpret_cast<char*>(&length), sizeof(length));

    if (length > 0)
    {
        data.resize(length);
        t_input.read(reinterpret_cast<char*>(data.data()), length);
    }
}

mdcii::chunk::Chunk::~Chunk() noexcept
{
}
