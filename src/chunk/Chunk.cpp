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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

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
