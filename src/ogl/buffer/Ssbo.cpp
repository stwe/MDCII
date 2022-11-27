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

#include "Ssbo.h"
#include "MdciiAssert.h"
#include "ogl/OpenGL.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::ogl::buffer::Ssbo::Ssbo()
{
    Log::MDCII_LOG_DEBUG("[Ssbo::Ssbo()] Create Ssbo.");

    CreateId();
}

mdcii::ogl::buffer::Ssbo::Ssbo(std::string t_name)
    : name{ std::move(t_name) }
{
    Log::MDCII_LOG_DEBUG("[Ssbo::Ssbo()] Create Ssbo {}.", name);

    CreateId();
}

mdcii::ogl::buffer::Ssbo::~Ssbo() noexcept
{
    Log::MDCII_LOG_DEBUG("[Ssbo::~Ssbo()] Destruct Ssbo.");

    CleanUp();
}

//-------------------------------------------------
// Bind / unbind
//-------------------------------------------------

void mdcii::ogl::buffer::Ssbo::Bind() const
{
    MDCII_ASSERT(id, "[Ssbo::Bind()] Invalid Ssbo handle.")
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
}

void mdcii::ogl::buffer::Ssbo::Unbind()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

//-------------------------------------------------
// Data
//-------------------------------------------------

void mdcii::ogl::buffer::Ssbo::StoreData(const uint32_t t_size, const void* t_data)
{
    glBufferData(GL_SHADER_STORAGE_BUFFER, t_size, t_data, GL_DYNAMIC_DRAW);
}

void mdcii::ogl::buffer::Ssbo::StoreSubData(const int32_t t_offset, const uint32_t t_size, const void* t_data)
{
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, t_offset, t_size, t_data);
}

//-------------------------------------------------
// Create
//-------------------------------------------------

void mdcii::ogl::buffer::Ssbo::CreateId()
{
    glGenBuffers(1, &id);
    MDCII_ASSERT(id, "[Ssbo::CreateId()] Error while creating a new Ssbo handle.")

    Log::MDCII_LOG_DEBUG("[Ssbo::CreateId()] A new Ssbo handle was created. The Id is {}.", id);
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::ogl::buffer::Ssbo::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[Ssbo::CleanUp()] Clean up Ssbo {} Id {}.", name, id);

    Unbind();

    if (id)
    {
        glDeleteBuffers(1, &id);
        Log::MDCII_LOG_DEBUG("[Ssbo::CleanUp()] Ssbo {} Id {} was deleted.", name, id);
    }
}
