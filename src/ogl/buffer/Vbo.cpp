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

#include "Vbo.h"
#include "MdciiAssert.h"
#include "ogl/OpenGL.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::ogl::buffer::Vbo::Vbo()
{
    Log::MDCII_LOG_DEBUG("[Vbo::Vbo()] Create Vbo.");

    CreateId();
}

mdcii::ogl::buffer::Vbo::~Vbo() noexcept
{
    Log::MDCII_LOG_DEBUG("[Vbo::~Vbo()] Destruct Vbo.");

    CleanUp();
}

//-------------------------------------------------
// Bind / unbind
//-------------------------------------------------

void mdcii::ogl::buffer::Vbo::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void mdcii::ogl::buffer::Vbo::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//-------------------------------------------------
// Data
//-------------------------------------------------

void mdcii::ogl::buffer::Vbo::ReserveMemory(const uint32_t t_size)
{
    glBufferData(GL_ARRAY_BUFFER, t_size, nullptr, GL_DYNAMIC_DRAW);
}

void mdcii::ogl::buffer::Vbo::StoreStaticData(const uint32_t t_size, const void* t_data)
{
    glBufferData(GL_ARRAY_BUFFER, t_size, t_data, GL_STATIC_DRAW);
}

void mdcii::ogl::buffer::Vbo::StoreData(const int32_t t_offset, const uint32_t t_size, const void* t_data)
{
    glBufferSubData(GL_ARRAY_BUFFER, t_offset, t_size, t_data);
}

//-------------------------------------------------
// Attributes
//-------------------------------------------------

void mdcii::ogl::buffer::Vbo::AddFloatAttribute(
    const uint32_t t_index,
    const int32_t t_nrOfFloatComponents,
    const int32_t t_nrOfAllFloats,
    const uint64_t t_startPoint,
    const bool t_instancing
)
{
    glEnableVertexAttribArray(t_index);
    glVertexAttribPointer(
        t_index,
        t_nrOfFloatComponents,
        GL_FLOAT,
        GL_FALSE,
        t_nrOfAllFloats * static_cast<int32_t>(sizeof(float)),
        reinterpret_cast<uintptr_t*>(t_startPoint * sizeof(float)) // NOLINT(performance-no-int-to-ptr)
    );

    if (t_instancing)
    {
        glVertexAttribDivisor(t_index, 1);
    }
}

void mdcii::ogl::buffer::Vbo::AddIntAttribute(
    const uint32_t t_index,
    const int32_t t_nrOfIntComponents,
    const int32_t t_nrOfAllInts,
    const uint64_t t_startPoint,
    const bool t_instancing
)
{
    glEnableVertexAttribArray(t_index);
    glVertexAttribIPointer(
        t_index,
        t_nrOfIntComponents,
        GL_INT,
        t_nrOfAllInts * static_cast<int32_t>(sizeof(int32_t)),
        reinterpret_cast<uintptr_t*>(t_startPoint * sizeof(int32_t)) // NOLINT(performance-no-int-to-ptr)
    );

    if (t_instancing)
    {
        glVertexAttribDivisor(t_index, 1);
    }
}

//-------------------------------------------------
// Create
//-------------------------------------------------

void mdcii::ogl::buffer::Vbo::CreateId()
{
    glGenBuffers(1, &id);
    MDCII_ASSERT(id, "[Vbo::CreateId()] Error while creating a new Vbo handle.")

    Log::MDCII_LOG_DEBUG("[Vbo::CreateId()] A new Vbo handle was created. The Id is {}.", id);
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::ogl::buffer::Vbo::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[Vbo::CleanUp()] Clean up Vbo Id {}.", id);

    Unbind();

    if (id)
    {
        glDeleteBuffers(1, &id);
        Log::MDCII_LOG_DEBUG("[Vbo::CleanUp()] Vbo Id {} was deleted.", id);
    }
}
