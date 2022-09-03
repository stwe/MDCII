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

#include "TextureUtils.h"
#include "MdciiAssert.h"
#include "ogl/OpenGL.h"

//-------------------------------------------------
// Generate && delete
//-------------------------------------------------

uint32_t mdcii::ogl::resource::TextureUtils::GenerateNewTextureId()
{
    uint32_t textureId{ 0 };

    glGenTextures(1, &textureId);
    MDCII_ASSERT(textureId, "[TextureUtils::GenerateNewTextureId()] Invalid texture Id.")

    return textureId;
}

void mdcii::ogl::resource::TextureUtils::DeleteTexture(const uint32_t t_textureId)
{
    if (t_textureId)
    {
        glDeleteTextures(1, &t_textureId);
    }
}

//-------------------------------------------------
// Filter
//-------------------------------------------------

void mdcii::ogl::resource::TextureUtils::UseNoFilter(const uint32_t t_target)
{
    glTexParameteri(t_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(t_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void mdcii::ogl::resource::TextureUtils::UseNoFilter()
{
    UseNoFilter(GL_TEXTURE_2D);
}

void mdcii::ogl::resource::TextureUtils::UseBilinearFilter(const uint32_t t_target)
{
    glTexParameteri(t_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(t_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void mdcii::ogl::resource::TextureUtils::UseBilinearFilter()
{
    UseBilinearFilter(GL_TEXTURE_2D);
}

void mdcii::ogl::resource::TextureUtils::UseBilinearMipmapFilter(const uint32_t t_target)
{
    glTexParameteri(t_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(t_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void mdcii::ogl::resource::TextureUtils::UseBilinearMipmapFilter()
{
    UseBilinearMipmapFilter(GL_TEXTURE_2D);
}

//-------------------------------------------------
// Wrapping
//-------------------------------------------------

void mdcii::ogl::resource::TextureUtils::UseRepeatWrapping(const uint32_t t_target)
{
    glTexParameteri(t_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(t_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void mdcii::ogl::resource::TextureUtils::UseRepeatWrapping()
{
    UseRepeatWrapping(GL_TEXTURE_2D);
}

void mdcii::ogl::resource::TextureUtils::UseClampToEdgeWrapping(const uint32_t t_target)
{
    glTexParameteri(t_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(t_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(t_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void mdcii::ogl::resource::TextureUtils::UseClampToEdgeWrapping()
{
    UseClampToEdgeWrapping(GL_TEXTURE_2D);
}

//-------------------------------------------------
// Bind / Unbind
//-------------------------------------------------

void mdcii::ogl::resource::TextureUtils::Bind(const uint32_t t_textureId, const uint32_t t_target)
{
    MDCII_ASSERT(t_textureId, "[TextureUtils::Bind()] Invalid texture Id.")

    // make sure that the OpenGL constants are used here
    MDCII_ASSERT(t_target == GL_TEXTURE_2D || t_target == GL_TEXTURE_3D || t_target == GL_TEXTURE_CUBE_MAP || t_target == GL_TEXTURE_2D_ARRAY, "[TextureUtils::Bind()] Invalid texture target.")

    glBindTexture(t_target, t_textureId);
}

void mdcii::ogl::resource::TextureUtils::Bind(const uint32_t t_textureId)
{
    Bind(t_textureId, GL_TEXTURE_2D);
}

void mdcii::ogl::resource::TextureUtils::Unbind(const uint32_t t_target)
{
    // make sure that the OpenGL constants are used here
    MDCII_ASSERT(t_target == GL_TEXTURE_2D || t_target == GL_TEXTURE_3D || t_target == GL_TEXTURE_CUBE_MAP || t_target == GL_TEXTURE_2D_ARRAY, "[TextureUtils::Unbind()] Invalid texture target.")

    glBindTexture(t_target, 0);
}

void mdcii::ogl::resource::TextureUtils::Unbind()
{
    Unbind(GL_TEXTURE_2D);
}

void mdcii::ogl::resource::TextureUtils::BindForReading(const uint32_t t_textureId, const uint32_t t_textureUnit, const uint32_t t_target)
{
    // make sure that the OpenGL constants are used here
    MDCII_ASSERT(t_textureUnit >= GL_TEXTURE0 && t_textureUnit <= GL_TEXTURE15, "[TextureUtils::BindForReading()] Invalid texture unit value.")

    glActiveTexture(t_textureUnit);
    Bind(t_textureId, t_target);
}

void mdcii::ogl::resource::TextureUtils::BindForReading(const uint32_t t_textureId, const uint32_t t_textureUnit)
{
    BindForReading(t_textureId, t_textureUnit, GL_TEXTURE_2D);
}
