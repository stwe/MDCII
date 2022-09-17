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

#include "Vao.h"
#include "Vbo.h"
#include "MdciiAssert.h"
#include "ogl/OpenGL.h"

//-------------------------------------------------
// Ctors. / Dtor.
//-------------------------------------------------

mdcii::ogl::buffer::Vao::Vao()
{
    Log::MDCII_LOG_DEBUG("[Vao::Vao()] Create Vao.");

    CreateId();
}

mdcii::ogl::buffer::Vao::~Vao() noexcept
{
    Log::MDCII_LOG_DEBUG("[Vao::~Vao()] Destruct Vao.");

    CleanUp();
}

//-------------------------------------------------
// Bind / unbind
//-------------------------------------------------

void mdcii::ogl::buffer::Vao::Bind() const
{
    glBindVertexArray(id);
}

void mdcii::ogl::buffer::Vao::Unbind()
{
    glBindVertexArray(0);
}

//-------------------------------------------------
// Draw
//-------------------------------------------------

void mdcii::ogl::buffer::Vao::DrawPrimitives(const uint32_t t_drawMode, const int32_t t_first) const
{
    MDCII_ASSERT(drawCount, "[Vao::DrawPrimitives()] Invalid draw count.")
    glDrawArrays(t_drawMode, t_first, drawCount);
}

void mdcii::ogl::buffer::Vao::DrawPrimitives(const uint32_t t_drawMode) const
{
    DrawPrimitives(t_drawMode, 0);
}

void mdcii::ogl::buffer::Vao::DrawPrimitives() const
{
    DrawPrimitives(GL_TRIANGLES);
}

void mdcii::ogl::buffer::Vao::DrawInstanced(const uint32_t t_drawMode, const int32_t t_first, const int32_t t_instances) const
{
    MDCII_ASSERT(drawCount, "[Vao::DrawInstanced()] Invalid draw count.")
    glDrawArraysInstanced(t_drawMode, t_first, drawCount, t_instances);
}

void mdcii::ogl::buffer::Vao::DrawInstanced(const uint32_t t_drawMode, const int32_t t_instances) const
{
    DrawInstanced(t_drawMode, 0, t_instances);
}

void mdcii::ogl::buffer::Vao::DrawInstanced(const int32_t t_instances) const
{
    DrawInstanced(GL_TRIANGLES, 0, t_instances);
}

//-------------------------------------------------
// Create
//-------------------------------------------------

void mdcii::ogl::buffer::Vao::CreateId()
{
    glGenVertexArrays(1, &id);
    MDCII_ASSERT(id, "[Vao::CreateId()] Error while creating a new Vao handle.")

    Log::MDCII_LOG_DEBUG("[Vao::CreateId()] A new Vao handle was created. The Id is {}.", id);
}

//-------------------------------------------------
// Clean up
//-------------------------------------------------

void mdcii::ogl::buffer::Vao::CleanUp() const
{
    Log::MDCII_LOG_DEBUG("[Vao::CleanUp()] Clean up Vao Id {}.", id);

    Unbind();

    if (id)
    {
        glDeleteVertexArrays(1, &id);
        Log::MDCII_LOG_DEBUG("[Vao::CleanUp()] Vao Id {} was deleted.", id);
    }
}
